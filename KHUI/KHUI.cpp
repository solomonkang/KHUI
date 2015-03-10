// HUIM-UP.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "utilitylist.h"
using namespace std;

typedef struct sort_pairs {
    bool operator()(pair<int,int> &left, pair<int,int> &right) {
        return left.second < right.second;
    }
} sort_pairs;

typedef struct TopK_Sort {
    bool operator()(pair<vector<int>,int> &left, pair<vector<int>,int> &right) {
        return left.second < right.second;
    }
} TopK_Sort;


struct Find_Pairs{
  Find_Pairs(int val) : val_(val) {}
  bool operator()(const std::pair<int, int>& elem) {
    return  elem.first == val_;
  }
  private:
    int val_;
};


struct Find_Large{
  Find_Large(int val) : val_(val) {}
  bool operator()(const UL& elem) {
	  return  elem.Item == val_;
  }
  private:
    int val_;
};
void Fill_TopK(UL P, pair<vector<int>,vector<UL>> P_ULs);
void Find_TopK(UL P, pair<vector<int>,vector<UL>> P_ULs);
void Update_TopK(vector<int> Itemset, int IU);
void Output_Result();
UL Combination(UL& P,UL& PX, UL& PY);
Element Find_Element(vector<Element>, int TID, int LastPos);


map <pair<int,int>, int > TwoItem_TWU;

unsigned int TopK_count=0,k;
typedef pair<vector<int>,  int> ivpair;
typedef pair<int, int> kvpair;
typedef std::vector<kvpair> vec_kvpair;
typedef pair<vector<int>, int> ItemsetToValue;
typedef pair<int,int> Trans;
int min_value=0;;

LARGE_INTEGER P1Start,P1End,P2Start,P2End,P3Start,P3End,fre;
double times;
char *filename="";
pair<vector<int>, vector<UL> > OneItem_ULs;

vector<ItemsetToValue> TopK;
vec_kvpair OneItem_TWU;

int main(int argc, char *argv[])
{
	set <int> aitems; // claim container of avaialbe items
	string line;
	fstream fin;
	int tid=0;

	if(argv[1]&&argv[2]&&argv[3]){
		min_value=atoi(argv[2]);
		filename =argv[1];
		k=atoi(argv[3]);
	}
	
    QueryPerformanceFrequency(&fre); //取得CPU頻率
    QueryPerformanceCounter(&P1Start); //取得開機到現在經過幾個CPU Cycle
	std::cout <<"P1(Scan Database for First Time) Start:"<<endl;
    fin.open(filename,ios::in);
	while(getline(fin, line)){
		istringstream iss(line);
		int n=0,tu=0,count=0;
		vector<int> Items, Utilities;
		while (iss>>n)
		{
			if(count==0){
				Items.push_back(n);
			}
			else if(count==1)
				tu=n;
			else if(count==2){
				Utilities.push_back(n);
			}
			if(iss.peek() ==':'){
				iss.ignore();
				count++;
			}
		}
		for(vector<int>::iterator it=Items.begin();it!=Items.end();it++)
		{
			vector<pair<int,int>>::iterator vt=find_if(OneItem_TWU.begin(),OneItem_TWU.end(), Find_Pairs(*it));
			if(vt==OneItem_TWU.end()){
				OneItem_TWU.push_back(make_pair(*it, tu));
			}
			else{
				vt->second+=tu;
			}
		}
    }
	fin.close();
	QueryPerformanceCounter(&P1End);
    times=((double)P1End.QuadPart-(double)P1Start.QuadPart)/fre.QuadPart;
    std::cout <<"P1(Scan Database for First Time) Execution Time:"<< fixed << setprecision(5) << times << 's' << endl;

	std::sort(OneItem_TWU.begin(), OneItem_TWU.end(), sort_pairs()); //sort 1-items with TWU order

	for(vec_kvpair::iterator it=OneItem_TWU.begin();it!=OneItem_TWU.end();it++){
		if(it->second>=min_value){
			UL Temp;
			Temp.Item=it->first;
			OneItem_ULs.second.push_back(Temp);
		}
	}

	cout<<endl;
	std::cout <<"P2(Scan Database for Second Time) Start:"<<endl;
	QueryPerformanceCounter(&P2Start);
    fin.open(filename,ios::in);
	while(getline(fin, line)){
		istringstream iss(line);
		int n=0,tu=0,count=0;
		vector<int> Items,R_Items,Utilities,R_Utilities;
		while (iss>>n)
		{
			if(count==0)
				Items.push_back(n);
			else if(count==1){
				tu=n;
			}
			else{
				Utilities.push_back(n);
			}
			if(iss.peek() ==':'){
				iss.ignore();
				count++;
			}
		}
		vector<pair<int,int>> Trans;
		for(vector<int>::iterator it=Items.begin();it!=Items.end();it++)
		{
			vector<UL>::iterator ut=find_if(OneItem_ULs.second.begin(),OneItem_ULs.second.end(), Find_Large(*it));
			if(ut!=OneItem_ULs.second.end()){
				pair<int, int> R_T;
				R_T.first=*it;
				R_T.second=Utilities[it-Items.begin()];
				Trans.push_back(R_T);
			}
		}
		int TU=tu;
		for(vector<pair<int,int>>::iterator it=Trans.begin();it!=Trans.end();it++)
		{
			Element E;
			E.tid=tid;
			E.iu=it->second;
			TU-=E.iu;
			E.ru=TU;
			vector<UL>::iterator vt=find_if(OneItem_ULs.second.begin(),OneItem_ULs.second.end(),Find_Large(it->first));
			vt->Add_Element(E);
		}
		for(vector<pair<int,int>>::iterator pt=Trans.begin();pt!=Trans.end();pt++){
			for(vector<pair<int,int>>::iterator kt=pt+1;kt!=Trans.end();kt++){
				TwoItem_TWU[make_pair(pt->first,kt->first)]+=tu;
			}
		}
		tid++;
    }
	fin.close();
	
	for(vector<UL>::iterator ut=OneItem_ULs.second.begin();ut!=OneItem_ULs.second.end();ut++){
		if(ut->Sum_IU>=min_value){
			vector<int> I;
			I.push_back(ut->Item);
			Update_TopK(I , ut->Sum_IU);
		}
	}
	cout<<min_value;



	QueryPerformanceCounter(&P2End);
	times=((double)P2End.QuadPart-(double)P2Start.QuadPart)/fre.QuadPart;
    std::cout <<"P2(Scan Database for Second Time) Execution Time:"<<fixed << setprecision(5) << times << 's' << endl;
	//Construct
	cout<<min_value<<endl;
	system("pause");
	std::cout <<"P3(HUIU-Miner) Start:"<<endl;
	QueryPerformanceCounter(&P3Start);
	UL empty;
	Fill_TopK(empty ,OneItem_ULs);
	Find_TopK(empty ,OneItem_ULs);
	QueryPerformanceCounter(&P3End);
	times=((double)P3End.QuadPart-(double)P3Start.QuadPart)/fre.QuadPart;
    std::cout<<"P3(HUIU-Miner) Execution Time:"<< fixed << setprecision(5) << times << 's' << endl;
	std::cout<<"Mission Complete!"<<endl;

	Output_Result();
	system("pause");
	return 0;
}

void Fill_TopK(UL P, pair<vector<int>,vector<UL>> P_ULs){
	cout<<"Fill_TopK";
}

void Find_TopK(UL P,pair<vector<int>,vector<UL>> P_ULs){
	for(vector<UL>::iterator it=P_ULs.second.begin();it!=P_ULs.second.end();it++){
		if(it->Sum_IU>=min_value||TopK.size()<k){// If Utility Value of Current Itemset is not less than min_value
			vector<int> I;
			I=P_ULs.first;
			I.push_back(it->Item);
			Update_TopK(I, it->Sum_IU);
		}
		UL Px=*it; //Decalre UtilityList Px=*it
		pair<vector<int>,vector<UL>> Px_Extend_ULs; //Declare UtilityList Px_Extend_ULs
		if(it->Sum_IU+it->Sum_RU>=min_value){
			for(vector<UL>::iterator jt=it+1;jt!=P_ULs.second.end();jt++){
				UL Py=*jt;//Decalre UtilityList Px=*jt
				if(it->Item==jt->Item)
					continue;
				int TWU_value=TwoItem_TWU[make_pair(it->Item,jt->Item)]; //Get TwoItem_TWU value from Matrix
				if(TWU_value<min_value){
					continue;
				}
				else{
					Px_Extend_ULs.second.push_back(Combination(P, Px, Py));
				}
			}
			Px_Extend_ULs.first.push_back(Px.Item);
			Find_TopK(Px,Px_Extend_ULs);
		}
	}
}
UL Combination(UL& P, UL& PX, UL& PY){
	UL PXY;
	int Last_Y_Pos=0;
	for(vector<Element>::iterator a=PX.Elements.begin();a!=PX.Elements.end();a++){
		Element B=Find_Element(PY.Elements, a->tid, Last_Y_Pos);
		if(B.tid==-1)
			continue;
		if(P.Item==0){
			Element E;
			E.tid=a->tid;
			E.iu=a->iu+B.iu;
			E.ru=B.ru;
			PXY.Add_Element(E);
			continue;
		}
		else{
			int Last_C_Pos=0;
			Element C=Find_Element(P.Elements, a->tid, Last_C_Pos);
			if(C.tid!=-1){
				Element E;
				E.tid=a->tid;
				E.iu=a->iu+B.iu-C.iu;
				E.ru=B.ru;
				PXY.Add_Element(E);
			}
		}
	}
	return PXY;
}

void Update_TopK(vector<int> Itemset, int IU){
	ItemsetToValue Temp;
	Temp.first=Itemset;
	Temp.second=IU;
	TopK.push_back(Temp);
	sort(TopK.begin(), TopK.end(), TopK_Sort());
	while(TopK.size()>k){
		TopK.erase(TopK.begin());
	}
}

void Output_Result(){
	fstream file;
	file.open("Result.txt", ios::app);     
	if(!file){
		exit(1);
    }
	file<<filename<<" " << min_value << " "<<k<<" "<< fixed << setprecision(5) << times<<endl;
	for(vector<ItemsetToValue>::iterator iv=TopK.begin();iv!=TopK.end();iv++){
		for(vector<int>::iterator pt=iv->first.begin();pt!=iv->first.end();pt++){
			file<<*pt<<"\t";
		}
		file<<iv->second<<endl;
	}
}


Element Find_Element(vector<Element> B, int TID, int LastPos){
	Element E;
	E.tid=-1;
	for(vector<Element>::iterator b=B.begin()+LastPos;b!=B.end();b++){
		if(TID==b->tid){
			E.tid=TID;
			E.iu=b->iu;
			E.ru=b->ru;
		}
	}
	return E;
}