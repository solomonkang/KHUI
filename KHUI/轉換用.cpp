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
    bool operator()(pair<vector<int>,int> &left, pair<vector<int>,int> &right) {
        return left.second < right.second;
    }
} TopK_Sort;

void Scan_Database_First();
void Scan_Database_Second();
void Increasing_Min_Value(UL P, vector<UL> P_ULs);
void KHUI(UL P, vector<UL> P_ULs);
void Output_Result();
void Output_Itemset(vector<int> I, int IU);
void Update_TopK(vector<int> Itemset, int IU);
void Split(const string& source, vector<int> I, vector<int> U, int tu);
void parse(string& source, int p);
UL Combination(UL& P, UL& Px, UL& Py);

//Global Variables
int min_value=0,tid=0,hui_count=0;
unsigned int k;
char *filename = "";
set<int> A_Items;
vector<pair<int, int>> OneItem_TWU;
vector<UL> OneItem_ULs;
map<int, int> R_Map;
map <set<int>, int > IU_Matrix;
LARGE_INTEGER Start, End, P2Start, P2End, P3Start, P3End, fre;
double total_time;
UL empty;
vector<pair<vector<int>, int>> TopK;



int main(int argc, char *argv[]){
	if (argv[1] && argv[2] && argv[3]){
		min_value = atoi(argv[2]);
		filename = argv[1];
		k = atoi(argv[3]);
		QueryPerformanceFrequency(&fre);
	}
	//QueryPerformanceCounter(&Start);
	Scan_Database_First();
	//Scan_Database_Second();
	//Increasing_Min_Value(empty, OneItem_ULs);
	//KHUI(empty, OneItem_ULs);
	//QueryPerformanceCounter(&End);
	//Output_Result();
	return 0;
}


void Scan_Database_First(){
	cout << "Phase 1: Scan database" << endl;
	string line;
	fstream fin;
	fstream file;
	fin.open(filename, ios::in);
	file.open("Modified.txt", ios::app);
	int tid = 0;
	while (getline(fin, line)){
		istringstream iss(line);
		int n = 0, tu = 0, count = 0, item_count=0;
		vector<int> items, utilities;
		while (iss >> n)
		{
			if (count == 0){
				items.push_back(n);
			}
			else if (count == 1){
				tu = n;
			}
			else {
				utilities.push_back(n);
			}
			if (iss.peek() == ':'){
				iss.ignore();
				count++;
			}
		}
		file << tid << " " << items.size();
		for (int i = 0; i < items.size();i++){
			file <<" "<< items[i] <<" "<< utilities[i];
		}
		file << endl;
		tid++;
	}
	fin.close();
	file.close();
}

void Scan_Database_Second(){
	string line;
	fstream fin;
	fin.open(filename, ios::in);
}


void Increasing_Min_Value(UL P, vector<UL> P_ULs){
	for (vector<UL>::reverse_iterator it = P_ULs.rbegin(); it != P_ULs.rend(); it++){
		if (it->Sum_IU>=min_value||TopK.size()<k){
			Update_TopK(it->Itemset, it->Sum_IU);
			Output_Itemset(it->Itemset, it->Sum_IU);
		}
		vector<UL> Px_Extend_ULs;
		for (vector<UL>::reverse_iterator jt = it + 1; jt != P_ULs.rend(); jt++){
			if (it->Sum_IU >= min_value&&jt->Sum_IU >= min_value){
				Px_Extend_ULs.push_back(Combination(P,*it, *jt));
			}
		}
		Increasing_Min_Value(*it, Px_Extend_ULs);
	}
}

void KHUI(UL P, vector<UL> P_ULs){
	for (vector<UL>::iterator it = P_ULs.begin(); it != P_ULs.end(); it++){
		if (it->Sum_IU >= min_value){
			Update_TopK(it->Itemset, it->Sum_IU);
			hui_count++;
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
	file<<filename<<" " << min_value <<" "<< fixed << setprecision(5) << hui_count <<" "<< total_time <<endl;
	//Write FileName, Min_value, Found_HUI, Total_Time
	file.close();
}

void Output_Itemset(vector<int> I, int IU){
	fstream file;
	file.open("Result.txt", ios::app);
	for (vector<int>::iterator it = I.begin(); it != I.end(); it++){
		cout << *it <<"\t";
	}
	cout << ":" << IU;
	cout << endl;
	file.close();
}
void Update_TopK(vector<int> Itemset, int IU){
	TopK.push_back(make_pair(Itemset, IU));
	sort(TopK.begin(), TopK.end(), TopK_Sort());
	if (TopK.size() > k){
		TopK.erase(TopK.begin());
	}
	min_value = TopK.begin()->second;
	cout << min_value<<endl;
}