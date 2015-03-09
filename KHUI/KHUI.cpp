#include "stdafx.h"
#include "utilitylist.h"
using namespace std;

typedef vector<UL> ULs;

//claim sub functions
void Scan_First();
void Scan_Second();
void Fill_TopK(UL P, ULs Ps);
void Find_TopK(UL P, ULs Ps);
UL Combination(UL& a, UL& ab, UL& ac);
Element Find_Element(UL P, int TID);

//Global Variables
UL empty; //not
ULs OneItem_ULs; //initialize UtilityLists for OneItem
int main(int argc, char *argv[])
{
	Scan_First();
	Scan_Second();
	Fill_TopK(empty, OneItem_ULs);
	Find_TopK(empty, OneItem_ULs);
	return 0;
}



//sub functions
void Scan_First(){
	cout<<"Scan_First"<<endl;
}

void Scan_Second(){
	cout<<"Scan_Second"<<endl;
}

void Fill_TopK(UL P, ULs Ps){
	cout<<"Fill_TopK"<<endl;
}

void Find_TopK(UL P, ULs Ps){
	cout<<"Find_TopK"<<endl;
}