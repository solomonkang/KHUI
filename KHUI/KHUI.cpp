#include "stdafx.h"
#include "utilitylist.h"
#include "omp.h"

using namespace std;

typedef struct Find_OneItemTWU{
	Find_OneItemTWU(int val) : val_(val) {}
	bool operator()(const pair<int, int>& elem) {
		return  elem.first == val_;
	}
private:
	int val_;
} Find_OneItemTWU;

typedef struct Sort_OneItemTWU {
	bool operator()(pair<int, int> &left, pair<int, int> &right) {
		return left.second < right.second;
	}
} Sort_OneItemTWU;

typedef struct Revise_Trans{
	bool operator()(pair<pair<int, int>, int > &left, pair<pair<int, int>, int > &right) {
		return left.second < right.second;
	}
} Revise_Trans;

struct Check_TopK{
	Check_TopK(set<int> val) : toFind(val) {}
	bool operator()(const pair<set<int>,int>& elem) {
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
	bool operator()(pair<set<int>, int> &left, pair<set<int>, int> &right) {
		return left.second < right.second;
	}
} TopK_Sort;

void Scan_Database();
void Increasing_Min_Value(UL P, vector<UL> P_ULs);
void KHUI(UL P, vector<UL> P_ULs);
void Output_Result();
void Output_Itemset(vector<int> I, int IU);
void Update_TopK(set<int> Itemset, int IU);
void Split(const string& source, vector<int> I, vector<int> U, int tu);
void parse(string& source, int p);
UL Combination(UL& P, UL& Px, UL& Py);

//Global Variables
int min_value = 0, tid = 0, hui_count = 0;
unsigned int k;
char *filename = "";
set<int> A_Items;
map<int, int> OneItem_TWU;
map<int, int> OneItem_Utility;
vector<UL> OneItem_ULs;
map<int, int> R_Map;
map <set<int>, int > IU_Matrix;
LARGE_INTEGER Start, End, P2Start, P2End, P3Start, P3End, fre;
double total_time;
UL empty;
vector<pair<set<int>, int>> TopK;



int main(int argc, char *argv[]){
	if (argv[1] && argv[2] && argv[3]){
		min_value = atoi(argv[2]);
		filename = argv[1];
		k = atoi(argv[3]);
		QueryPerformanceFrequency(&fre);
	}
	QueryPerformanceCounter(&Start);
	Scan_Database();
	cout << "Current Min_Value" << min_value << endl;
	cout << "Phase 3: Increasing_Min_Value" << endl;
	Increasing_Min_Value(empty, OneItem_ULs);
	cout << "Current Min_Value" << min_value << endl;
	cout << "Phase 4: Mining Top-K High Utility Itemsets" << endl;
	KHUI(empty, OneItem_ULs);
	QueryPerformanceCounter(&End);
	Output_Result();
	return 0;
}


void Scan_Database(){
	cout << "Phase 1: Scan database" << endl;
	string line;
	fstream fin;
	fin.open(filename, ios::in);
	string a, b;
	int item_count, item, utility,tu;
	while (fin.good()){
		vector<int> Itemset;
		fin >> line;
		tid = stoi(line);
		fin >> line;
		item_count = stoi(line);
		fin >> line;
		tu = stoi(line);
		for (int i = 0; i < item_count; i++){
			fin >> line;
			item = stoi(line);
			fin >> line;
			utility = stoi(line);
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
	vector<pair<int, int>> V_OneItem_TWU(OneItem_TWU.begin(), OneItem_TWU.end()); //Create a vector point to OneItem_TWU.map;

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
		it->second = it - V_OneItem_TWU.begin();
	}
	for (vector<pair<int, int>>::iterator it = V_OneItem_TWU.begin(); it != V_OneItem_TWU.end(); it++){
		R_Map[it->first] = it->second;
	}

	fin.close();

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
		tu = stoi(line);
		vector < pair<pair<int, int>, int>> R_Trans;
		int RU = 0;
		for (int i = 0; i < item_count; i++){
			fin >> line;
			item = stoi(line);
			fin >> line;
			utility = stoi(line);
			if (OneItem_TWU[item] >= min_value){
				R_Trans.push_back(make_pair(make_pair(item, utility), R_Map[item]));
				RU += utility;
			}
		}
		sort(R_Trans.begin(), R_Trans.end(), Revise_Trans()); //sort R_Trans;
		for (vector<pair<pair<int, int>, int >>::iterator it = R_Trans.begin(); it != R_Trans.end(); it++){
			RU -= it->first.second;
			vector<UL>::iterator vt = find_if(OneItem_ULs.begin(), OneItem_ULs.end(), Find_UL(it->first.first));
			Element E;
			E.tid = tid;
			E.iu = it->first.second;
			E.ru = RU;
			vt->Add_Element(E);
		}
	}
}


void Increasing_Min_Value(UL P, vector<UL> P_ULs){
	for (vector<UL>::reverse_iterator it = P_ULs.rbegin(); it != P_ULs.rend(); it++){
		if (it->Sum_IU >= min_value || TopK.size()<k){
			set<int> Itemset;
			for (auto a = it->Itemset.begin(); a != it->Itemset.end(); a++){
				Itemset.insert(*a);
			}
			Update_TopK(Itemset, it->Sum_IU);
		}
		vector<UL> Px_Extend_ULs;
		for (vector<UL>::reverse_iterator jt = it + 1; jt != P_ULs.rend(); jt++){
			if (it->Sum_IU >= min_value&&jt->Sum_IU >= min_value){
				Px_Extend_ULs.push_back(Combination(P, *it, *jt));
			}
		}
		Increasing_Min_Value(*it, Px_Extend_ULs);
	}
}

void KHUI(UL P, vector<UL> P_ULs){
	for (vector<UL>::iterator it = P_ULs.begin(); it != P_ULs.end(); it++){
		if (it->Sum_IU >= min_value || TopK.size()<k){
			set<int> Itemset;
			for (auto a = it->Itemset.begin(); a != it->Itemset.end(); a++){
				Itemset.insert(*a);
			}
			Update_TopK(Itemset, it->Sum_IU);
		}
		if (it->Sum_IU + it->Sum_RU >= min_value){
			vector<UL> P_Extend_ULs;
			for (vector<UL>::iterator jt = it + 1; jt != P_ULs.end(); jt++){
				//if (IU_Matrix[make_pair(*it->Itemset.rbegin(),*it->Itemset.rbegin())]>=min_value)
				P_Extend_ULs.push_back(Combination(P, *it, *jt));
			}
			KHUI(*it, P_Extend_ULs);
		}
	}
}
UL Combination(UL& P, UL& Px, UL& Py){
	UL Pxy;
	Pxy.Itemset = Px.Itemset;
	Pxy.Itemset.push_back(*Py.Itemset.rbegin());
	//for (vector<int>::iterator it = Pxy.Itemset.begin(); it != Pxy.Itemset.end(); it++){
	//	cout << *it <<"\t";
	//}
	//cout << endl;
	vector<Element>::iterator Last_Y_Pos = Py.Elements.begin();
	vector<Element>::iterator Last_Z_Pos = P.Elements.begin();
	for (vector<Element>::iterator x = Px.Elements.begin(); x != Px.Elements.end(); x++){
		for (vector<Element>::iterator y = Last_Y_Pos; y != Py.Elements.end(); y++){
			if (x->tid == y->tid){
				if (P.Itemset.size() == 0){
					Last_Y_Pos = y + 1;
					Element E = *y;
					E.iu += x->iu;
					Pxy.Add_Element(E);
					break;
				}
				else{
					for (vector<Element>::iterator z = Last_Z_Pos; z != P.Elements.end(); z++){
						if (x->tid == z->tid){
							Last_Z_Pos = z + 1;
							Element E = *y;
							E.iu += (x->iu - z->iu);
							Pxy.Add_Element(E);
							break;
						}
						else if (z->tid > x->tid){
							Last_Z_Pos = z;
							break;
						}
					}
					break;
				}
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
	total_time = ((double)End.QuadPart - (double)Start.QuadPart) / fre.QuadPart;
	file << filename << " " << min_value << " " << fixed << setprecision(5) << k << " " << total_time << endl;
	for (auto it = TopK.begin(); it != TopK.end(); it++){
		for (auto jt = it->first.begin(); jt != it->first.end(); jt++)
		{
			file << *jt << "\t";
		}
		file<<":"<<it->second<<endl;
	}
	//Write FileName, Min_value, Found_HUI, Total_Time
	file.close();
}

//void Output_Itemset(vector<int> I, int IU){
//	fstream file;
//	file.open("Result.txt", ios::app);
//	for (vector<int>::iterator it = I.begin(); it != I.end(); it++){
//		file << *it << "\t";
//	}
//	file << ":" << IU;
//	file << endl;
//	file.close();
//}
void Update_TopK(set<int> Itemset, int IU){
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