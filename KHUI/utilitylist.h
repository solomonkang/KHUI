#include <iostream>
#include <vector>
#include <set>
#include <map>
using namespace std;

class UL {
    public:
        vector<int> Itemset; // Itemset
        int Sum_IU; // Sum of Utility
        int Sum_RU; // Sum of Remaining Utility
		map<int, pair<int,int>> Elements; // Map of <Key(TID), Value<iu,ru>>
		UL() : Sum_IU(0), Sum_RU(0){ //Initialize value of Sum_IU and Sum_RU
        }
        void Add_Element(pair<int, pair<int,int>> E){ //Add_Element sub function
			Sum_IU += E.second.first;
            Sum_RU += E.second.second;
            Elements.insert(E);
        }
};