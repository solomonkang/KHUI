#include "stdafx.h"
#include "utilitylist.h"
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

void Scan_Database_First();
void Scan_Database_Second();
void Increasing_Min_Value();
void KHUI(UL P, vector<UL> P_ULs);
void Output_Result();
void Output_Itemset(vector<int> I, int IU);
UL Combination(UL& P, UL& Px, UL& Py);

//Global Variables
int min_value=0,tid=0,hui_count=0;
unsigned int k;
char *filename = "";
set<int> A_Items;
vector<pair<int, int>> OneItem_TWU;
vector<UL> OneItem_ULs;
map<int, int> R_Map;
map <pair<int, int>, int > IU_Matrix;
LARGE_INTEGER Start, End, P2Start, P2End, P3Start, P3End, fre;
double total_time;
//
//typedef struct TopK_Sort {
//    bool operator()(pair<vector<int>,int> &left, pair<vector<int>,int> &right) {
//        return left.second < right.second;
//    }
//} TopK_Sort;
//
//

//
//
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
//
//
//void Fill_TopK(UL P, vector<UL> P_ULs);
//void Find_TopK(UL P, vector<UL> P_ULs);
//void Update_TopK(vector<int> Itemset, int IU);
//UL Combination(UL& P,UL& Px, UL& Py);
//pair<int,int> Find_Element(map<int, pair<int,int>>, int TID);
//
//
//unsigned int TopK_count=0,k;
//typedef pair<vector<int>,  int> ivpair;
//typedef pair<int, int> kvpair;
//typedef std::vector<kvpair> vec_kvpair;
//typedef pair<vector<int>, int> ItemsetToValue;
//typedef pair<int,int> Trans;
//int min_value=0;;
//
//
//double times;
//char *filename="";
//vector<UL> OneItem_ULs;
//ItemsetToTWU OneItem_TWU;
//vector<ItemsetToValue> TopK;
//
int main(int argc, char *argv[]){
	if (argv[1] && argv[2] && argv[3]){
		min_value = atoi(argv[2]);
		filename = argv[1];
		k = atoi(argv[3]);
		QueryPerformanceFrequency(&fre);
	}
	QueryPerformanceCounter(&Start);
	Scan_Database_First();
	Scan_Database_Second();
	Increasing_Min_Value();
	UL empty;
	KHUI(empty, OneItem_ULs);
	QueryPerformanceCounter(&End);
	Output_Result();
	return 0;
}

void Scan_Database_First(){
	string line;
	fstream fin;
	fin.open(filename, ios::in);

	while (getline(fin, line)){
		istringstream input(line);
		int n = 0, tu = 0, count = 0;
		vector<int> I, U;
		while (input >> n)
		{
			if (count == 0){
				I.push_back(n);
			}
			else if (count == 1){
				tu = n;
			}
			else{
				U.push_back(n);
			}
			if (input.peek() == ':'){
				input.ignore();
				count++;
			}
		}
		for (vector<int>::iterator it = I.begin(); it != I.end(); it++){
			vector<pair<int, int>>::iterator mt = find_if(OneItem_TWU.begin(), OneItem_TWU.end(),Find_OneItemTWU(*it)); //check if the Item is in the vector
			if (mt == OneItem_TWU.end()){
				OneItem_TWU.push_back(make_pair(*it, tu)); //push_back as a new pair;
			}
			else{
				mt->second += tu; //update pair->second value;
			}
		}
	}
	//sort OneItem_TWU and save Available items
	sort(OneItem_TWU.begin(), OneItem_TWU.end(), Sort_OneItemTWU());

	for (vector<pair<int, int>>::iterator it = OneItem_TWU.begin(); it != OneItem_TWU.end(); it++){
		if (it->second >= min_value){
			UL TempUL;
			TempUL.Itemset.push_back (it->first);
			OneItem_ULs.push_back(TempUL);
			A_Items.insert(it->first);
		}
		else{
			OneItem_TWU.erase(it);
			it--;
		}
	}
	for (vector<pair<int, int>>::iterator it = OneItem_TWU.begin(); it != OneItem_TWU.end(); it++){
		it->second = it - OneItem_TWU.begin();
	}

	for (vector<pair<int, int>>::iterator it = OneItem_TWU.begin(); it != OneItem_TWU.end(); it++){
		R_Map[it->first] = it->second;
	}
}

void Scan_Database_Second(){
	string line;
	fstream fin;
	fin.open(filename, ios::in);
	int tid = 0;
	while (getline(fin, line)){
		istringstream input(line);
		int n = 0, tu = 0, count = 0;
		vector<int> I, U;
		while (input >> n)
		{
			if (count == 0){
				I.push_back(n);
			}
			else if (count == 1){
				tu = n;
			}
			else{
				U.push_back(n);
			}
			if (input.peek() == ':'){
				input.ignore();
				count++;
			}
		}
		int RU=0;
		vector < pair<pair<int, int>,int >> R_Trans;
		for (vector<int>::iterator it = I.begin(); it != I.end(); it++){
			if (A_Items.find(*it) != A_Items.end()){
				R_Trans.push_back(make_pair(make_pair(*it,U[it-I.begin()]), R_Map[*it]));
				RU += U[it - I.begin()];
			}
		}
		sort(R_Trans.begin(), R_Trans.end(), Revise_Trans()); //sort R_Trans;
		for (vector<pair<pair<int, int>, int >>::iterator it = R_Trans.begin(); it != R_Trans.end(); it++){
			RU -= it->first.second;
			vector<UL>::iterator vt = find_if(OneItem_ULs.begin(), OneItem_ULs.end(), Find_UL(it->first.first));
			//for (vector<pair<pair<int, int>, int >>::iterator jt = it + 1; jt != R_Trans.end(); jt++){
			//	IU_Matrix[make_pair(it->first.first,jt->first.first)] += tu;
			//}
			Element E;
			E.tid = tid;
			E.iu = it->first.second;
			E.ru = RU;
			vt->Add_Element(E);
		}
		tid++;
	}
}

void Increasing_Min_Value(){
	cout << "" << endl;
}

void KHUI(UL P, vector<UL> P_ULs){
	for (vector<UL>::iterator it = P_ULs.begin(); it != P_ULs.end(); it++){
		if (it->Sum_IU >= min_value){
			//Output_Itemset(it->Itemset, it->Sum_IU);
			hui_count++;
		}
		if (it->Sum_IU + it->Sum_RU >= min_value){
			vector<UL> P_Extend_ULs;
			for (vector<UL>::iterator jt = it + 1; jt != P_ULs.end(); jt++){
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
					Last_Y_Pos = y+1;
					Element E = *y;
					E.iu += x->iu;
					Pxy.Add_Element(E);
					break;
				}
				else{
					for (vector<Element>::iterator z = Last_Z_Pos; z != P.Elements.end(); z++){
						if (x->tid == z->tid){
							Last_Z_Pos = z+1;
							Element E = *y;
							E.iu += (x->iu-z->iu);
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
				Last_Y_Pos=y;
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
	file.close();
}

void Output_Itemset(vector<int> I, int IU){
	fstream file;
	file.open("Result.txt", ios::app);
	for (vector<int>::iterator it = I.begin(); it != I.end(); it++){
		file << *it <<"\t";
	}
	file << "Item Utility = " << IU;
	file << endl;
	file.close();
}