#include <iostream>
#include <vector>
#include <set>
#include <map>
using namespace std;

class Element{
	public:
		int tid;
		int iu;
		int ru;
};


class UL {
    public:
        int Item;
        int Sum_IU;
        int Sum_RU;
		vector<Element> Elements;
		UL() : Sum_IU(0), Sum_RU(0){
        }
        void Add_Element(Element E){
			Sum_IU += E.iu;
            Sum_RU += E.ru;
            Elements.push_back(E);
        }
};