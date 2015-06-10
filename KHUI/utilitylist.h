#include <iostream>
#include <vector>
#include <map>
using namespace std;

class Element{
public:
	int tid;
	float iu;
	float ru;
};


class UL {
public:
	vector<int> Itemset; //Item name
	float Sum_IU; // Sum of Utility
	float Sum_RU; // Sum of Remaining Utility
	float Mau;
	vector<Element> Elements; // vector of <Element>
	vector<float> CTWU;
	UL() : Sum_IU(0), Sum_RU(0), Mau(0){ //Initialize value of Sum_IU and Sum_RU
	}
	void Add_Element(const Element& E){ //Add_Element sub function
		Sum_IU += E.iu;
		Sum_RU += E.ru;
		Elements.push_back(E);
		if (E.iu > Mau){
			Mau = E.iu;
		}
	}
};