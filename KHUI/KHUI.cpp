#include "stdafx.h"
#include "utilitylist.h"
#include "omp.h"

using namespace std;

typedef struct Find_OneItemTWU{
	Find_OneItemTWU(int val) : val_(val) {}
	bool operator()(const pair<int, float>& elem) {
		return  elem.first == val_;
	}
private:
	int val_;
} Find_OneItemTWU;

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
void KHUI(UL P, int pos);
//void KHUI(UL P, vector<UL> P_ULs);
void Output_Result();
void Output_Itemset(vector<int> I, float IU);
void Update_TopK(set<int> Itemset, float IU);
UL Combination(UL& Px, UL& Py);

//Global Variables
float min_value = 0;
int tid = 0, hui_count = 0;
int prune_count = 0;
unsigned int k;
char *filename = "";
set<int> A_Items;

map<int, float> OneItem_TWU;
map<int, float> OneItem_Utility;

vector<UL> OneItem_ULs;
map<int, int> R_Map;
LARGE_INTEGER Start, End, P2Start, P2End, P3Start, P3End, fre;
double times_1,times_2,times_3;
UL empty;
vector<pair<set<int>, float>> TopK;



int main(int argc, char *argv[]){
	if (argv[1] && argv[2] && argv[3]){
		min_value = atof(argv[2]);
		filename = argv[1];
		k = atoi(argv[3]);
		QueryPerformanceFrequency(&fre);
	}
	Scan_Database();
	QueryPerformanceCounter(&P2End);
	times_2 = ((double)P2End.QuadPart - (double)P2Start.QuadPart) / fre.QuadPart;

	cout << "Phase 3: Mining Top-K High Utility Itemsets" << endl;
	QueryPerformanceCounter(&P3Start);
	for (auto a = OneItem_ULs.begin(); a != OneItem_ULs.end(); a++){
		int pos = a - OneItem_ULs.begin();
		KHUI(*a, pos);
	}
	QueryPerformanceCounter(&P3End);
	times_3 = ((double)P3End.QuadPart - (double)P3Start.QuadPart) / fre.QuadPart;
	//KHUI(empty, OneItem_ULs);
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
		vector<int> Itemset;
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
			Itemset.push_back(item);
			OneItem_Utility[item] += utility;
		}
		for (auto it = Itemset.begin(); it != Itemset.end(); it++){
			OneItem_TWU[*it] += tu;
		}
	}
	for (auto it = OneItem_Utility.begin(); it != OneItem_Utility.end();it++){
		if (it->second >= min_value||TopK.size()<k)
		{
			set<int> Itemset;
			Itemset.insert(it->first);
			Update_TopK(Itemset,it->second);
		}
	}


	vector<pair<int, float>> V_OneItem_TWU(OneItem_TWU.begin(), OneItem_TWU.end()); //Create a vector point to OneItem_TWU.map;

	sort(V_OneItem_TWU.begin(), V_OneItem_TWU.end(), Sort_OneItemTWU());
	
	for (auto it = V_OneItem_TWU.begin(); it != V_OneItem_TWU.end(); it++){
		if (it->second >= min_value){
			UL TempUL;
			TempUL.Itemset.push_back(it->first);
			OneItem_ULs.push_back(TempUL);
			A_Items.insert(it->first);
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
	times_1 = ((double)End.QuadPart - (double)Start.QuadPart) / fre.QuadPart;



	QueryPerformanceCounter(&P2Start);
	cout << "Current Min_Value" << min_value << endl;
	cout << "Phase 2: Scan database Again" << endl;
	fin.open(filename, ios::in);
	while (fin.good()){
		vector<int> Itemset;
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
			if (OneItem_TWU[item] >= min_value){
				R_Trans.push_back(make_pair(make_pair(item, utility), R_Map[item]));
				RU += utility;
			}
		}
		sort(R_Trans.begin(), R_Trans.end(), Revise_Trans()); //sort R_Trans;
		for (auto i = R_Trans.begin(); i != R_Trans.end(); i++){
			RU -= i->first.second;
			vector<UL>::iterator vt = find_if(OneItem_ULs.begin(), OneItem_ULs.end(), Find_UL(i->first.first));
			Element E;
			E.tid = tid;
			E.iu = i->first.second;
			E.ru = RU;
			vt->Add_Element(E);
		}
	}
}

void KHUI(UL P, int pos){
	pos += 1;
	if (P.Sum_IU >= min_value){
		set<int> Itemset;
		for (auto i = P.Itemset.begin(); i != P.Itemset.end(); i++){
			Itemset.insert(*i);
		}
		Update_TopK(Itemset, P.Sum_IU);
	}
	if (P.Sum_IU + P.Sum_RU >= min_value){

		for (auto a = OneItem_ULs.begin() + pos; a != OneItem_ULs.end(); a++){
			if (P.Sum_IU + a->Sum_IU + a->Sum_RU < min_value){
				continue;
			}
			else {
				pos = a - OneItem_ULs.begin();
				KHUI(Combination(P, *a), pos);
			}
		}
	}
}

UL Combination(UL& Px, UL& Py){
	UL Pxy;
	Pxy.Itemset = Px.Itemset;
	Pxy.Itemset.push_back(*Py.Itemset.rbegin());
	vector<Element>::iterator Last_Y_Pos = Py.Elements.begin();
	for (vector<Element>::iterator x = Px.Elements.begin(); x != Px.Elements.end(); x++){
		for (vector<Element>::iterator y = Last_Y_Pos; y != Py.Elements.end(); y++){
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
	file.open("Result.txt", ios::app);
	file << filename << " " << min_value << " " << fixed << setprecision(5) << k << " " << times_1 <<" "<<times_2<<" "<<times_3<<endl;
	//for (auto it = TopK.begin(); it != TopK.end(); it++){
	//	for (auto jt = it->first.begin(); jt != it->first.end(); jt++)
	//	{
	//		file << *jt << "\t";
	//	}
	//	file<<":"<<it->second<<endl;
	//}
	//Write FileName, Min_value, Found_HUI, Total_Time
	file.close();
}

//void Output_Itemset(vector<int> I, float IU){
//	fstream file;
//	file.open("Result.txt", ios::app);
//	for (vector<int>::iterator it = I.begin(); it != I.end(); it++){
//		file << *it << "\t";
//	}
//	file << ":" << IU;
//	file << endl;
//	file.close();
//}
void Update_TopK(set<int> Itemset, float IU){
	auto it=find_if(TopK.begin(), TopK.end(), Check_TopK(Itemset));
	if (it == TopK.end()){
		TopK.push_back(make_pair(Itemset, IU));
		sort(TopK.begin(), TopK.end(), TopK_Sort());
	}
	if (TopK.size() > k){
		TopK.erase(TopK.begin());
	}
	min_value = TopK.begin()->second;
}