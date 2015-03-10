// HUIM-UP.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "utilitylist.h"
using namespace std;

typedef vector<UL> ULs;

typedef struct sort_pairs {
    bool operator()(pair<int,int> &left, pair<int,int> &right) {
        return left.second < right.second;
    }
} sort_pairs;

typedef struct TopK_sort {
    bool operator()(pair<vector<int>,int> &left, pair<vector<int>,int> &right) {
        return left.second < right.second;
    }
} TopK_sort;


struct find_pairs{
  find_pairs(int val) : val_(val) {}
  bool operator()(const std::pair<int, int>& elem) {
    return  elem.first == val_;
  }
  private:
    int val_;
};

struct find_itemsets
{
  find_itemsets(vector<int> val) : val_(val) {}
  bool operator()(const UL& elem) {
	  return  elem.Itemset == val_;
  }
  private:
    vector<int> val_;
};

void Fill_TopK(UL P, ULs OneItem_ULs);
void Find_TopK(UL P, ULs OneItem_ULs);
UL Combination(UL& P,UL& PX, UL& PY);

map <pair<int, int>, int> TWU_matrix;

unsigned int TopK_count=0,k;
typedef pair<vector<int>,  int> ivpair;
typedef pair<int, int> kvpair;
typedef std::vector<kvpair> vec_kvpair;
int min_value=0;;

LARGE_INTEGER P1Start,P1End,P2Start,P2End,P3Start,P3End,fre;
double times;

vector <ivpair> TopK;
vec_kvpair TWU_table;

int main(int argc, char *argv[])
{
	char *filename="";
	set <int> aitems; // claim container of avaialbe items
	string line;
	fstream fin;
	int tid=0;
	ULs OneItem_ULs;

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
		vector<int> items,Utilities;
		while (iss>>n)
		{
			if(count==0)
				items.push_back(n);
			else if(count==1)
				tu=n;
			if(iss.peek() ==':'){
				iss.ignore();
				count++;
			}
		}
		for(vector<int>::iterator it=items.begin();it!=items.end();it++)
		{
			vec_kvpair::iterator vt = std::find_if(TWU_table.begin(), TWU_table.end(), find_pairs(*it));
			if(vt==TWU_table.end()){
				TWU_table.push_back(kvpair(*it,tu));
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

	std::sort(TWU_table.begin(), TWU_table.end(), sort_pairs()); //sort 1-items with TWU order


	for(vec_kvpair::iterator it=TWU_table.begin();it!=TWU_table.end();it++){
		if(it->second>=min_value){
			UL tempUlist;
			tempUlist.Add_Item(it->first);
			OneItem_ULs.push_back(tempUlist);
			aitems.insert(it->first); 
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
		for(vector<int>::iterator it=Items.begin();it!=Items.end();it++)
		{
			if(aitems.find(*it)!=aitems.end()){
				R_Items.push_back(*it);
				R_Utilities.push_back(Utilities[it-Items.begin()]);
			}
		}
		int TU=tu;
		for(vector<int>::iterator it=R_Items.begin();it!=R_Items.end();it++)
		{
				UL tempUlist;
				vector<int> I;
				I.push_back(*it);
				Element E;
				E.tid=tid;
				E.iu=R_Utilities[it-R_Items.begin()];
				TU-=E.iu;
				E.ru=TU;
				ULs::iterator ut=std::find_if(OneItem_ULs.begin(),OneItem_ULs.end(),find_itemsets(I));
				OneItem_ULs[ut-OneItem_ULs.begin()].Add_Element(E);
		}
		for(vector<int>::iterator pt=R_Items.begin();pt!=R_Items.end();pt++){
			for(vector<int>::iterator kt=pt+1;kt!=R_Items.end();kt++){
				TWU_matrix[make_pair(*pt,*kt)]+=tu;;
			}
		}
		tid++;
    }
	fin.close();
	
	QueryPerformanceCounter(&P2End);
	times=((double)P2End.QuadPart-(double)P2Start.QuadPart)/fre.QuadPart;
    std::cout <<"P2(Scan Database for Second Time) Execution Time:"<<fixed << setprecision(5) << times << 's' << endl;
	//Construct
	std::cout <<"P3(HUIU-Miner) Start:"<<endl;
	QueryPerformanceCounter(&P3Start);
	UL empty;
	Fill_TopK(empty ,OneItem_ULs);
	Find_TopK(empty ,OneItem_ULs);
	QueryPerformanceCounter(&P3End);
	times=((double)P3End.QuadPart-(double)P3Start.QuadPart)/fre.QuadPart;
    std::cout<<"P3(HUIU-Miner) Execution Time:"<< fixed << setprecision(5) << times << 's' << endl;
	std::cout<<"Mission Complete!"<<endl;

	fstream file;
	file.open("Result.txt", ios::app);      //開啟檔案
	if(!file){
		exit(1);
    }
	file<<filename<<" " << min_value << " "<<k<<" "<< fixed << setprecision(5) << times<<endl;
	for(vector<pair<vector<int>,int>>::iterator iv=TopK.begin();iv!=TopK.end();iv++){
		for(vector<int>::iterator pt=iv->first.begin();pt!=iv->first.end();pt++){
			file<<*pt<<"\t";
		}
		file<<iv->second<<endl;
	}
  return 0;
}

void Fill_TopK(UL P, ULs P_ULs){
	for(ULs::iterator it=P_ULs.end()-1;it!=P_ULs.begin()-1;it--){
		if(TopK_count>=10*k)
			break;

		int item_size = it->Itemset.size();
		int IU = it->Sum_IU;
		int RU = it->Sum_RU;
		int SUM=IU+RU;

		if(IU>=min_value){
			ivpair temppair;
			temppair.first=it->Itemset;
			temppair.second=IU;
			TopK.push_back(temppair);
			sort(TopK.begin(), TopK.end(), TopK_sort());
			if(TopK.size()>k)
				TopK.erase(TopK.begin());
			min_value=TopK[0].second;
		}
		TopK_count++;

		UL Px=*it;
		ULs Extend_Ulist;

		if(SUM>=min_value){
			for(ULs::iterator jt=it+1;jt!=P_ULs.end();jt++){
				UL Py=*jt;
				UL Pxy;
				int TWU_value=TWU_matrix[make_pair(*it->Itemset.rbegin(),*jt->Itemset.rbegin())];
				if(TWU_value>=min_value){
					
					Extend_Ulist.push_back(Combination(P, Px, Py));
				}
				else
					break;
			}
		}
		if(Extend_Ulist.size()>0){
			Fill_TopK(Px,Extend_Ulist);
		}
	}
}

void Find_TopK(UL P, ULs P_ULs){
	for(ULs::iterator it=P_ULs.begin();it!=P_ULs.end();it++){

		int IU = it->Sum_IU;
		int RU = it->Sum_RU;
		int SUM=IU+RU;

		if(IU>=min_value){
			ivpair temppair;
			temppair.first=it->Itemset;
			temppair.second=IU;
			TopK.push_back(temppair);
			sort(TopK.begin(), TopK.end(), TopK_sort());
			if(TopK.size()>k)
				TopK.erase(TopK.begin());
			min_value=TopK[0].second;
		}
		
		UL Px=*it;
		ULs Extend_Ulist;

		if(SUM>=min_value){
			for(ULs::iterator jt=it+1;jt==P_ULs.begin();jt++){

				UL Py=*jt;
				UL Pxy;
				int TWU_value=TWU_matrix[make_pair(*Px.Itemset.rbegin(),*Py.Itemset.rbegin())];
				if(TWU_value>=min_value){
					Extend_Ulist.push_back(Combination(P, Px, Py));
				}
			}
		}
		Find_TopK(Px,Extend_Ulist);
	}
}
UL Combination(UL& P, UL& PX, UL& PY){
	UL PXY;
	PXY.Itemset=PX.Itemset;
	PXY.Add_Item(*PY.Itemset.rbegin());
	for(vector<Element>::iterator a=PX.elements.begin();a!=PX.elements.end();a++){
		for(vector<Element>::iterator b=PY.elements.begin();b!=PY.elements.end();b++){
			if(a->tid==b->tid){
				if(P.Itemset.size()==0){
					Element E;
					E.tid=a->tid;
					E.iu=a->iu+b->iu;
					E.ru=b->ru;
					PXY.Add_Element(E);
					break;
				}
				else{
					for(vector<Element>::iterator c=P.elements.begin();c!=P.elements.end();c++){
						if(a->tid==c->tid){
							Element E;
							E.tid=a->tid;
							E.iu=a->iu+b->iu-c->iu;
							E.ru=b->ru;
							PXY.Add_Element(E);
							break;
						}
					}
				}
			}
		}
	}
	return PXY;
}