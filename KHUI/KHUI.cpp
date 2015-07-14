#include "stdafx.h"
#include "utilitylist.h"
#include "omp.h"
#include <Psapi.h>

using namespace std;

typedef struct Sort_OneItemTWU {
	bool operator()(pair<int, float> &left, pair<int, float> &right) {
		return left.second <right.second;
	}
} Sort_OneItemTWU;

typedef struct Revise_Trans{
	bool operator()(pair<pair<int, float>, int > &left, pair<pair<int, float>, int > &right) {
		return left.second <right.second;
	}
} Revise_Trans;

struct Check_TopK{
	Check_TopK(set<int> val) : toFind(val) {}
	bool operator()(const pair<set<int>,float>& elem) {
		return  elem.first == toFind;
	}
	set<int> toFind;
};


struct Find_UL{
	Find_UL(int val) : val_(val) {}
	bool operator()(const UL& elem) {
		return  *elem.Itemset.begin() == val_;
	}
private:
	int val_;
};

struct Find_Element{
	Find_Element(int val) : val_(val) {}
	bool operator()(const Element& elem) {
		return  elem.tid == val_;
	}
private:
	int val_;
};

typedef struct TopK_Sort {
	bool operator()(pair<set<int>, float> &left, pair<set<int>, float> &right) {
		return left.second < right.second;
	}
} TopK_Sort;

void Scan_Database();
void KHUI(UL& P, int pos);
void Output_Result();
void Output_Itemset(vector<int> I, float IU);
void Update_TopK(set<int> Itemset, float IU);
UL Combination(UL& Px, UL& Py);

//Global Variables
float cm_utility = 0,p1_threshold,p2_threshold;
int tid = 0, MAU_Prune_count = 0;
int prune_count = 0;
int candidate =0;
unsigned int k;
char *filename = "";

map<pair<int,int>, float> TwoItem_IU;
map<int, float> OneItem_TWU;
map<int, float> OneItem_Utility;

vector<UL> OneItem_ULs;
map<int, int> R_Map;
LARGE_INTEGER Start, End, P2Start, P2End, P3Start, P3End, fre; // exe time computation
double P1_time,P2_time,P3_time; // record execution time
vector<pair<set<int>, float>> TopK;
SIZE_T physMemUsedByMe;

int main(int argc, char *argv[]){
	if (argv[1] && argv[2] && argv[3]){
		cm_utility = atof(argv[2]);
		filename = argv[1];
		k = atoi(argv[3]);
		QueryPerformanceFrequency(&fre);
	}
	Scan_Database();
	QueryPerformanceCounter(&P2End);
	P2_time = ((double)P2End.QuadPart - (double)P2Start.QuadPart) / fre.QuadPart;

	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	physMemUsedByMe = pmc.WorkingSetSize;
	QueryPerformanceCounter(&P2End);
	P2_time = ((double)P2End.QuadPart - (double)P2Start.QuadPart) / fre.QuadPart;

	cout << "Phase 3: KHUI Verifying" << endl;
	QueryPerformanceCounter(&P3Start);
	for (auto a = OneItem_ULs.begin(); a != OneItem_ULs.end(); a++){
		int pos = a - OneItem_ULs.begin();
		KHUI(*a, pos);
	}
	QueryPerformanceCounter(&P3End);
	P3_time = ((double)P3End.QuadPart - (double)P3Start.QuadPart) / fre.QuadPart;
	Output_Result();
	return 0;
}


void Scan_Database(){
	QueryPerformanceCounter(&Start);
	cout << "Phase 1: Scan database" << endl;
	string line;
	fstream fin;
	fin.open(filename, ios::in);
	string a, b;
	int item_count, item;
	float utility, tu;
	while (fin.good()){
		vector<pair<int, float>> iup; //ItemUtilityPair
		fin >> line;
		tid = stoi(line);
		fin >> line;
		item_count = stoi(line);
		fin >> line;
		tu = stof(line);
		for (int i = 0; i < item_count; i++){
			fin >> line;
			item = stoi(line);
			fin >> line;
			utility = stof(line);
			iup.push_back(make_pair(item,utility));
			OneItem_Utility[item] += utility;
			OneItem_TWU[item] += tu;
		}
	}
	for (auto it = OneItem_Utility.begin(); it != OneItem_Utility.end();it++){
		if (it->second >= cm_utility||TopK.size()<k)
		{
			set<int> Itemset;
			Itemset.insert(it->first);
			Update_TopK(Itemset,it->second);
		}
	}
	set<int> pkhui_one_item;
	for (auto a = TopK.begin(); a != TopK.end();a++){
		pkhui_one_item.insert(*a->first.begin());
	}
	vector<pair<int, float>> V_OneItem_TWU(OneItem_TWU.begin(), OneItem_TWU.end()); //Create a vector point to OneItem_TWU.map;

	sort(V_OneItem_TWU.begin(), V_OneItem_TWU.end(), Sort_OneItemTWU());

	for (auto it = V_OneItem_TWU.begin(); it != V_OneItem_TWU.end(); it++){
		if (it->second >= cm_utility){
			UL TempUL;
			TempUL.CTWU.resize(V_OneItem_TWU.end()-it);
			TempUL.Itemset.push_back(it->first);
			OneItem_ULs.push_back(TempUL);
		}
		else{
			V_OneItem_TWU.erase(it);
			--it;
		}
	}
	for (auto it = V_OneItem_TWU.begin(); it != V_OneItem_TWU.end(); it++){
		it->second = float(it - V_OneItem_TWU.begin());
	}
	for (vector<pair<int, float>>::iterator it = V_OneItem_TWU.begin(); it != V_OneItem_TWU.end(); it++){
		R_Map[it->first] = float(it->second);
	}

	fin.close();
	QueryPerformanceCounter(&End);
	P1_time = ((double)End.QuadPart - (double)Start.QuadPart) / fre.QuadPart;
	p1_threshold = cm_utility;

	QueryPerformanceCounter(&P2Start);
	cout << "Phase 2: Scan database Again" << endl;

	fin.open(filename, ios::in);
	while (fin.good()){
		fin >> line;
		tid = stoi(line);
		fin >> line;
		item_count = stoi(line);
		fin >> line;
		tu = stof(line);
		vector < pair<pair<int, float>, int>> R_Trans;
		float RU = 0;
		for (int i = 0; i < item_count; i++){
			fin >> line;
			item = stoi(line);
			fin >> line;
			utility = stof(line);
			if (OneItem_TWU[item] >= cm_utility){
				R_Trans.push_back(make_pair(make_pair(item, utility), R_Map[item]));
				RU += utility;
			}
		}
		sort(R_Trans.begin(), R_Trans.end(), Revise_Trans()); //sort R_Trans;
		
		for (auto i = R_Trans.begin(); i != R_Trans.end(); i++){
			RU -= i->first.second;
			auto k = OneItem_ULs.begin() + R_Map[i->first.first];
			Element E;
			E.tid = tid;
			E.iu = i->first.second;
			E.ru = RU;
			k->Add_Element(E);
			auto x = pkhui_one_item.find(i->first.first);
			for (auto j = i + 1; j != R_Trans.end(); j++){
				if (x != pkhui_one_item.end()){
					TwoItem_IU[make_pair(i->first.first, j->first.first)] += i->first.second + j->first.second;
				}
				k->CTWU[R_Map[j->first.first]-R_Map[i->first.first]-1] += tu;
			}
		}
	}
	for (auto a = TwoItem_IU.begin(); a != TwoItem_IU.end(); a++){
		if (a->second > cm_utility){
			set<int> Itemset;
			Itemset.insert(a->first.first);
			Itemset.insert(a->first.second);
			Update_TopK(Itemset,a->second);
		}
	}
	p2_threshold = cm_utility;
}

void KHUI(UL& P, int pos){
	pos += 1;
	if (P.Sum_IU >= cm_utility){
		set<int> Itemset;
		for (auto i = P.Itemset.begin(); i != P.Itemset.end(); i++){
			Itemset.insert(*i);
		}
		Update_TopK(Itemset, P.Sum_IU);
	}
	if (P.Sum_IU + P.Sum_RU >= cm_utility){
		for (auto a = OneItem_ULs.begin() + pos; a != OneItem_ULs.end(); a++){
			if (P.CTWU[R_Map[*a->Itemset.begin()]-R_Map[*P.Itemset.rbegin()]-1] < cm_utility){
				continue;
			}
			if (P.Sum_IU + min(a->Sum_IU, a->Mau*min(a->Elements.size(), P.Elements.size())) + a->Sum_RU < cm_utility){
				continue;
			}
			pos = a - OneItem_ULs.begin();
			KHUI(Combination(P, *a), pos);
		}
	}
	P.CTWU.clear();
}

UL Combination(UL& Px, UL& Py){
	UL Pxy;
	candidate++;
	Pxy.CTWU = Py.CTWU;
	for (int i = 0; i != Pxy.CTWU.size(); i++){
		if (Px.CTWU[R_Map[*Py.Itemset.begin()] - R_Map[*Px.Itemset.rbegin()]+i] < Pxy.CTWU[i]){
			Pxy.CTWU[i] = Px.CTWU[R_Map[*Py.Itemset.begin()] - R_Map[*Px.Itemset.rbegin()] + i];
		}
	}
	Pxy.Itemset = Px.Itemset;
	Pxy.Itemset.push_back(*Py.Itemset.rbegin());
	auto Last_Y_Pos = Py.Elements.begin();
	for (auto x = Px.Elements.begin(); x != Px.Elements.end(); x++){
		for (auto y = Last_Y_Pos; y != Py.Elements.end(); y++){
			if (x->tid == y->tid){
				Last_Y_Pos = y + 1;
				Element E = *y;
				E.iu += x->iu;
				Pxy.Add_Element(E);
				break;
			}
			else if (y->tid > x->tid){
				Last_Y_Pos = y;
				break;
			}
		}
	}
	return Pxy;
}

void Output_Result(){
	fstream file;
	string filename2 = filename;
	filename2 = filename2.substr(0,5);
	filename2+="_Results.txt";
	file.open(filename2, ios::app);
	file << filename << " "<<p1_threshold <<" "<<p2_threshold<<" "<< cm_utility << " " << fixed << setprecision(5) << k << " " << P1_time <<" "<<P2_time<<" "<<P3_time <<" "<<P1_time+P2_time+P3_time<< " "<< physMemUsedByMe <<" " <<candidate<<endl;
	//for (auto it = TopK.begin(); it != TopK.end(); it++){
	//	for (auto jt = it->first.begin(); jt != it->first.end(); jt++){
	//		file << *jt << "\t";
	//	}
	//	file<<":"<<it->second<<endl;
	//}
	file.close();
}

void Update_TopK(set<int> Itemset, float IU){
	auto it=find_if(TopK.begin(), TopK.end(), Check_TopK(Itemset));
	if (it == TopK.end()){
		TopK.push_back(make_pair(Itemset, IU));
		sort(TopK.begin(), TopK.end(), TopK_Sort());
	}
	if (TopK.size() > k){
		TopK.erase(TopK.begin());
	}
	cm_utility = TopK.begin()->second;
}