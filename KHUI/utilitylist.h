#include <iostream>
#include <vector>
#include <set>
#include <map>
using namespace std;

class UtilityList
{
    public:
        vector<int> itemset;
        int SumIU;
        int SumRU;
		map<int, pair<int, int> > elements;
		UtilityList() : SumIU(0), SumRU(0)
        {
        }
        void addItem(int item)
		{
			this->itemset.push_back(item);
		}
        
        void addElement(pair<int ,pair<int, int>> element)
        {
            SumIU += element.second.first;
            SumRU += element.second.second;
            elements.insert(element);
        }
};