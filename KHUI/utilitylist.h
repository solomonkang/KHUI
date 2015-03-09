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
        vector<int> Itemset;
        int Sum_IU;
        int Sum_RU;
		vector<Element> elements;
		UL() : Sum_IU(0), Sum_RU(0){
        }
        void Add_Item(int item)
		{
			this->Itemset.push_back(item);
		}
        
        void Add_Element(Element element)
        {
			Sum_IU += element.iu;
            Sum_RU += element.ru;
            elements.push_back(element);
        }
};