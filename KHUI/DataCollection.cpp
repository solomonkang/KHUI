#include "stdafx.h"
#include "DataCollection.h"

using namespace std;

void UtilityList::addTransaction(int tid, float iu, float ru)
{
	sum_iu += iu;
	sum_ru += ru;
	Transaction* transaction = new Transaction(tid, iu, ru);
	transactions->push_back(transaction);
}

void DataCollection::addUtilityList(int name)
{
	UtilityList* utility_list = new UtilityList(name);
	utility_list->addTransaction(4, 5, 6);
	utility_lists->push_back(utility_list);
}
