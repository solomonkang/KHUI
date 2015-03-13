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
UL Combination(UL& P, UL& PX, UL& PY);
void KHUI(UL P, vector<UL> P_ULs);
//Global Variables
int min_value=0,tid=0,hui_count=0;
unsigned int k;
char *filename = "";
set<int> A_Items;
vector<pair<int, int>> OneItem_TWU;
vector<UL> OneItem_ULs;
map<int, int> R_Map;
int TWU_D;
LARGE_INTEGER P1Start, P1End, P2Start, P2End, P3Start, P3End, fre;
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
//void Output_Result();
//UL Combination(UL& P,UL& PX, UL& PY);
//pair<int,int> Find_Element(map<int, pair<int,int>>, int TID);
//
//map <pair<int,int>, int > TwoItem_TWU;
//
//unsigned int TopK_count=0,k;
//typedef unordered_map<int, int> ItemsetToTWU;
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
	Scan_Database_First();
	Scan_Database_Second();
	Increasing_Min_Value();
	UL empty;
	QueryPerformanceCounter(&P1Start);
	KHUI(empty, OneItem_ULs);
	QueryPerformanceCounter(&P1End);
	double times;
	times = ((double)P1End.QuadPart - (double)P1Start.QuadPart) / fre.QuadPart;
	std::cout <<"P1(Scan Database for First Time) Execution Time:"<< fixed << setprecision(5) << times << 's' << endl;
	cout << hui_count;
	system("pause");
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
			Element E;
			E.tid = tid;
			E.iu = it->first.second;
			E.ru = RU;
			vt->Add_Element(E);
		}
		TWU_D += tu;
		tid++;
	}
}

void Increasing_Min_Value(){
	cout << "" << endl;
}

void KHUI(UL P, vector<UL> P_ULs){
	for (vector<UL>::iterator it = P_ULs.begin(); it != P_ULs.end(); it++){
		if (it->Itemset.size() == 1){
			cout << *it->Itemset.begin();
		}
		if (it->Sum_IU >= min_value){
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
UL Combination(UL& P, UL& PX, UL& PY){
	UL PXY;
	PXY.Itemset = PX.Itemset;
	PXY.Itemset.push_back(*PY.Itemset.rbegin());
	for (vector<Element>::iterator x = PX.Elements.begin(); x != PX.Elements.end(); x++){
		vector<Element>::iterator Last_Y_Pos = PY.Elements.begin();
		auto y = find_if(Last_Y_Pos, PY.Elements.end(), Find_Element(x->tid));
		if (y != PY.Elements.end()){
			Last_Y_Pos = y;
			if (P.Itemset.size() == 0){
				Element E;
				E = *y;
				E.iu += x->iu;
				PXY.Add_Element(E);
			}
			else{
				vector<Element>::iterator Last_Z_Pos = P.Elements.begin();
				auto z = find_if(Last_Z_Pos, P.Elements.end(), Find_Element(x->tid));
				if (z != P.Elements.end()){
					Last_Z_Pos = z;
					Element E;
					E.tid = x->tid;
					E.iu = x->iu + y->iu -z->iu;
					E.ru = y->ru;
					PXY.Add_Element(E);
				}
			}
		}
	}
	return PXY;
}


//	
//     //取得CPU頻率
//    QueryPerformanceCounter(&P1Start); //取得開機到現在經過幾個CPU Cycle
//	std::cout <<"P1(Scan Database for First Time) Start:"<<endl;
//    fin.open(filename,ios::in);
//	while(getline(fin, line)){
//		istringstream iss(line);
//		int n=0,tu=0,count=0;
//		vector<int> Items, Utilities;
//		while (iss>>n)
//		{
//			if(count==0){
//				Items.push_back(n);
//			}
//			else if (count == 1){
//				tu = n;
//			}
//			else if(count==2){
//				Utilities.push_back(n);
//			}
//			if(iss.peek() ==':'){
//				iss.ignore();
//				count++;
//			}
//		}
//		for(vector<int>::iterator it=Items.begin();it!=Items.end();it++)
//		{
//			ItemsetToTWU::iterator mt = OneItem_TWU.find(*it);
//			if(mt==OneItem_TWU.end()){
//				OneItem_TWU.insert(make_pair(*it,tu));
//			}
//			else{
//				mt->second+=tu;
//			}
//		}
//    }
//	fin.close();
//	for (ItemsetToTWU::iterator mt = OneItem_TWU.begin(); mt != OneItem_TWU.end();mt++){
//		cout << mt->first <<":"<< mt->second <<endl;
//	}
//	QueryPerformanceCounter(&P1End);
//    times=((double)P1End.QuadPart-(double)P1Start.QuadPart)/fre.QuadPart;
//    std::cout <<"P1(Scan Database for First Time) Execution Time:"<< fixed << setprecision(5) << times << 's' << endl;
//
//	std::sort(OneItem_TWU.begin(), OneItem_TWU.end(), sort_pairs()); //sort 1-items with TWU order
//
//	for(vec_kvpair::iterator it=OneItem_TWU.begin();it!=OneItem_TWU.end();it++){
//		if(it->second>=min_value){
//			UL Temp;
//			Temp.Itemset.push_back(it->first);
//			A_Items.insert(it->first);
//			OneItem_ULs.push_back(Temp);
//		}
//	}
//
//	cout<<endl;
//	std::cout <<"P2(Scan Database for Second Time) Start:"<<endl;
//	QueryPerformanceCounter(&P2Start);
//    fin.open(filename,ios::in);
//	tid = 0;
//	while(getline(fin, line)){
//		istringstream iss(line);
//		int n=0,tu=0,count=0;
//		vector<int> Items,R_Items,Utilities,R_Utilities;
//		while (iss>>n)
//		{
//			if(count==0)
//				Items.push_back(n);
//			else if(count==1){
//				tu=n;
//			}
//			else{
//				Utilities.push_back(n);
//			}
//			if(iss.peek() ==':'){
//				iss.ignore();
//				count++;
//			}
//		}
//		for(vector<int>::iterator it=Items.begin();it!=Items.end();it++)
//		{
//		}
//		int TU=tu;
//		for (vector<int>::iterator it = R_Items.begin();it!=R_Items.end(); it++)
//		{
//			vector<UL>::iterator vt=find_if(OneItem_ULs.begin(),OneItem_ULs.end(),Find_Large(*it));
//			int iu = R_Utilities[it - R_Items.begin()];
//			int ru = TU-iu;
//			vt->Add_Element(make_pair(tid, make_pair(iu,ru)));
//		}
//		for(vector<int>::iterator it=R_Items.begin();it!=R_Items.end();it++){
//			for(vector<int>::iterator jt=it+1;jt!=R_Items.end();jt++){
//				TwoItem_TWU[make_pair(*it,*jt)]+=tu;
//			}
//		}
//		tid++;
//    }
//	fin.close();
//	
//	for(vector<UL>::iterator ut=OneItem_ULs.begin();ut!=OneItem_ULs.end();ut++){
//		if(ut->Sum_IU>=min_value){
//			vector<int> I;
//			I.push_back(*ut->Itemset.begin());
//			Update_TopK(I , ut->Sum_IU);
//		}
//	}
//	QueryPerformanceCounter(&P2End);
//	times=((double)P2End.QuadPart-(double)P2Start.QuadPart)/fre.QuadPart;
//    std::cout <<"P2(Scan Database for Second Time) Execution Time:"<<fixed << setprecision(5) << times << 's' << endl;
//	Construct
//	std::cout <<"P3(HUIU-Miner) Start:"<<endl;
//	QueryPerformanceCounter(&P3Start);
//	UL empty;
//	Fill_TopK(empty ,OneItem_ULs);
//	Find_TopK(empty ,OneItem_ULs);
//	QueryPerformanceCounter(&P3End);
//	times=((double)P3End.QuadPart-(double)P3Start.QuadPart)/fre.QuadPart;
//    std::cout<<"P3(HUIU-Miner) Execution Time:"<< fixed << setprecision(5) << times << 's' << endl;
//	std::cout<<"Mission Complete!"<<endl;
//
//	Output_Result();
//	system("pause");
//	return 0;
//}
//
//void Fill_TopK(UL P, vector<UL> P_ULs){
//	cout<<" ";
//}
//
//void Find_TopK(UL P, vector<UL> P_ULs){
//	for(vector<UL>::iterator it=P_ULs.begin();it!=P_ULs.end();it++){
//		if(it->Sum_IU>=min_value||TopK.size()<k){// If Utility Value of Current Itemset is not less than min_value
//			Update_TopK(it->Itemset, it->Sum_IU);
//		}
//		UL Px=*it; //Decalre UtilityList Px=*it
//		vector<UL> Px_Extend_ULs; //Declare UtilityList Px_Extend_ULs
//		if(it->Sum_IU+it->Sum_RU>=min_value){
//			for(vector<UL>::iterator jt=it+1;jt!=P_ULs.end();jt++){
//				UL Py=*jt;//Decalre UtilityList Px=*jt
//				if(it->Itemset==jt->Itemset)
//					continue;
//				int TWU_value=TwoItem_TWU[make_pair(*it->Itemset.rbegin(),*jt->Itemset.rbegin())]; //Get TwoItem_TWU value from Matrix
//				if(TWU_value<min_value){
//					continue;
//				}
//				else{
//					Px_Extend_ULs.push_back(Combination(P, Px, Py));
//				}
//			}
//			Find_TopK(Px,Px_Extend_ULs);
//		}
//	}
//}
//UL Combination(UL& P, UL& PX, UL& PY){
//	UL PXY;
//	PXY.Itemset = PX.Itemset;
//	PXY.Itemset.push_back(*PY.Itemset.rbegin());
//	for(map<int, pair<int,int>>::iterator a=PX.Elements.begin();a!=PX.Elements.end();a++){
//		pair<int,int> B=Find_Element(PY.Elements, a->first);
//		if(B == pair<int,int>())
//			continue;
//		if(P.Itemset.size()==0){
//			PXY.Add_Element(make_pair(a->first, make_pair(a->second.first+B.first, B.second)));
//			continue;
//		}
//		else{
//			pair<int, int> C=Find_Element(P.Elements, a->first);
//			if (C == pair<int, int>()){
//				continue;
//			}
//			else{
//				PXY.Add_Element(make_pair(a->first, make_pair(a->second.first + B.first-C.first, B.second)));
//				continue;
//			}
//		}
//	}
//	return PXY;
//}
//
//void Update_TopK(vector<int> Itemset, int IU){
//	ItemsetToValue Temp;
//	Temp.first=Itemset;
//	Temp.second=IU;
//	TopK.push_back(Temp);
//	sort(TopK.begin(), TopK.end(), TopK_Sort());
//	while(TopK.size()>k){
//		TopK.erase(TopK.begin());
//	}
//}
//
//void Output_Result(){
//	fstream file;
//	file.open("Result.txt", ios::app);     
//	if(!file){
//		exit(1);
//    }
//	file<<filename<<" " << min_value << " "<<k<<" "<< fixed << setprecision(5) << times<<endl;
//	for(vector<ItemsetToValue>::iterator iv=TopK.begin();iv!=TopK.end();iv++){
//		for(vector<int>::iterator pt=iv->first.begin();pt!=iv->first.end();pt++){
//			file<<*pt<<"\t";
//		}
//		file<<iv->second<<endl;
//	}
//}
//
//
