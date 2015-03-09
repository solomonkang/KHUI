#include "stdafx.h"
#include "utilitylist.h"
using namespace std;

typedef vector<UL> ULs;
typedef pair<vector<int>, int> ItemsetToValue;
typedef pair<int,int> ItemToValue;

struct Find_Item{
  Find_Item(int val) : val_(val) {}
  bool operator()(const std::pair<int, int>& elem) {
    return  elem.first == val_;
  }
  private:
    int val_;
};
struct Find_Itemset{
  Find_Itemset(vector<int> val) : val_(val) {}
  bool operator()(const UL& elem) {
	  return  elem.Itemset == val_;
  }
  private:
    vector<int> val_;
};

struct Find_From_Two{
  Find_From_Two(vector<int> val) : val_(val) {}
  bool operator()(const ItemsetToValue& elem) {
	  return  elem.first == val_;
  }
  private:
    vector<int> val_;
};


typedef struct Sort_Pairs {
    bool operator()(pair<int,int> &left, pair<int,int> &right) {
        return left.second < right.second;
    }
} Sort_Pairs;

typedef struct Sort_TopK {
    bool operator()(pair<vector<int>,int> &left, pair<vector<int>,int> &right) {
        return left.second < right.second;
    }
} Sort_TopK;


//claim sub functions
void Scan_First();
void Scan_Second();
void Fill_TopK(UL P, ULs Ps);
void Find_TopK(UL P, ULs Ps);
void Update_TopK(vector<int> Itemset, int IU);
UL Combination(UL& a, UL& ab, UL& ac);
Element Find_Element(UL P, int TID);

//Global Variables
UL empty; //Declare a empty UtilityList: for Fill_TopK && Find_TopK
ULs OneItem_ULs; //Declare and initialize UtilityLists for OneItem
vector<ItemsetToValue> OneItem_IU_TopK;
vector<ItemsetToValue> TopK;
vector<ItemToValue> OneItem_IU;
vector<ItemToValue> OneItem_TWU;
map<pair<int,int>,int> TwoItem_TWU;
map<pair<int,int>,int> TwoItem_IU;
int min_value=0; //Declare min_value = 0
unsigned int k; //Declare K for Top-K, this will limit the size of TopK container
char *filename="";
set<int> A_Items;

int main(int argc, char *argv[])
{
	if(argv[1]&&argv[2]&&argv[3]){
		min_value=atoi(argv[2]);
		filename =argv[1];
		k=atoi(argv[3]);
	}
	Scan_First();
	Scan_Second();
	Fill_TopK(empty, OneItem_ULs);
	Find_TopK(empty, OneItem_ULs);
	return 0;
}
//sub functions
void Scan_First(){

	string line;
	fstream fin;
	int tid=0; //initialize tid=0;
	fin.open(filename,ios::in);

	while(getline(fin, line)){
		istringstream iss(line);
		int n=0,tu=0,count=0;
		vector<int> T_Items,T_Utilities;
		while (iss>>n)
		{
			if(count==0)
				T_Items.push_back(n);
			else if(count==1){
				tu=n;
				if(tu>=min_value){
					min_value=tu; //increasing min_value with TU;
				}
			}
			else {
				T_Utilities.push_back(n);
			}
			if(iss.peek() ==':'){
				iss.ignore();
				count++;
			}
		}
		for(vector<int>::iterator it=T_Items.begin();it!=T_Items.end();it++)
		{
			vector<ItemToValue>::iterator vt = find_if(OneItem_TWU.begin(), OneItem_TWU.end(), Find_Item(*it));
			if(vt==OneItem_TWU.end()){
				OneItem_TWU.push_back(ItemToValue(*it,tu));
			}
			else{
				vt->second+=tu;
			}
		}
    }
	fin.close();
	sort(OneItem_TWU.begin(), OneItem_TWU.end(), Sort_Pairs());
	for(vector<ItemToValue>::iterator it=OneItem_TWU.begin();it!=OneItem_TWU.end();it++){
		if(it->second>=min_value){
			UL TempList;
			TempList.Add_Item(it->first);
			A_Items.insert(it->first);
			OneItem_ULs.push_back(TempList);
		}
	}
	cout<<min_value;
}

void Scan_Second(){
	string line;
	fstream fin;
	int tid=0; //initialize tid=0;
	fin.open(filename,ios::in);

	while(getline(fin, line)){
		istringstream iss(line);
		int n=0,tu=0,count=0;
		vector<int> T_Items,T_Utilities, R_Items, R_Utilities;
		while (iss>>n)
		{
			if(count==0)
				T_Items.push_back(n);
			else if(count==1){
				tu=n;
			}
			else {
				T_Utilities.push_back(n);
			}
			if(iss.peek() ==':'){
				iss.ignore();
				count++;
			}
		}
		int TU=tu; // Declare TU for keeping tu value in memory
		for(vector<int>::iterator it=T_Items.begin();it!=T_Items.end();it++){
			if(A_Items.find(*it)!=A_Items.end()){ // If found T_Items also Valaible in A_Items
				R_Items.push_back(*it);
				R_Utilities.push_back(T_Utilities[it-T_Items.begin()]);
			}
		}
		for(vector<int>::iterator it=R_Items.begin();it!=R_Items.end();it++){
			vector<int> I;
			I.push_back(*it);
			Element E;
			E.tid=tid;
			E.iu=R_Utilities[it-R_Items.begin()];
			TU-=E.iu;
			E.ru=TU;
			ULs::iterator ut=find_if(OneItem_ULs.begin(),OneItem_ULs.end(),Find_Itemset(I));
			ut->Add_Element(E);
		}
		for(vector<int>::iterator pt=R_Items.begin();pt!=R_Items.end();pt++){
			for(vector<int>::iterator kt=pt+1;kt!=R_Items.end();kt++){
				TwoItem_TWU[make_pair(*pt,*kt)]+=tu;
				TwoItem_IU[make_pair(*pt,*kt)]+=(R_Utilities[pt-R_Items.begin()]+R_Utilities[kt-R_Items.begin()]);
			}
		}
		tid++;
    }
	
	fin.close();
	for(map<pair<int,int>,int>::iterator mt=TwoItem_IU.begin();mt!=TwoItem_IU.end();mt++){
		if(mt->second>=min_value){
			min_value=mt->second;
		}
	}
	cout<<min_value;
}

void Fill_TopK(UL P, ULs Ps){
	cout<<"Fill_TopK"<<endl;
}

void Find_TopK(UL P, ULs Ps){
	for(ULs::iterator it=Ps.begin();it!=Ps.end();it++){
		if(it->Sum_IU>=min_value){
			Update_TopK(it->Itemset,it->Sum_IU);
			for(ULs::iterator jt=it+1;jt!=Ps.end();jt++){
				Combination(P, *it, *jt);
			}
		}
		if(it->Sum_IU+it->Sum_RU>=min_value){
			for(ULs::iterator jt=it+1;jt!=Ps.end();jt++){
					Combination(P, *it, *jt);
			}
		}
	}
}
void Update_TopK(vector<int> Itemset, int IU){
	TopK.push_back(ItemsetToValue(Itemset, IU));
	sort(TopK.begin(), TopK.end(), Sort_TopK());
	while(TopK.size()>k){
		TopK.erase(TopK.begin());
	}
	min_value=TopK.begin()->second;
}

UL Combination(UL& P, UL& PX, UL& PY){
	UL PXY;
	return PXY;
}