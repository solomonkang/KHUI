#include <iostream>
#include <vector>
using namespace std;

class Element{
	public:
		int tid;
		int iu;
		int ru;
};


class UL {
    public:
		int Item; //Item name
        int Sum_IU; // Sum of Utility
        int Sum_RU; // Sum of Remaining Utility
		vector<Element> Elements; // vector of <Element>
		UL(int Item) : Sum_IU(0), Sum_RU(0){ //Initialize value of Sum_IU and Sum_RU
			this->Item = Item;
        }
        void Add_Element(Element E){ //Add_Element sub function
			Sum_IU += E.iu;
            Sum_RU += E.ru;
			Elements.push_back(E);
        }
};