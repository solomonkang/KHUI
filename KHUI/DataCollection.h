#include <iostream>
#include <vector>
#include <map>

class Transaction{
public:
	Transaction(int TID, float IU, float RU) : tid(TID), iu(IU), ru(RU) {};
private:
	int tid;
	float iu;
	float ru;
};

class UtilityList {
public:

	UtilityList(int name) : sum_iu(0), sum_ru(0), mau(0),name(name)
	{
		transactions = new std::vector<Transaction*>();
		itemset_name = new std::vector<UtilityList*>();
	};
	void addTransaction(int tid, float iu, float ru);

private:
	int name;
	float sum_iu;
	float sum_ru;
	float mau;
	std::vector<UtilityList*>* itemset_name;
	std::vector<Transaction*>* transactions; 
};

class DataCollection {
public:
	DataCollection()
	{
		utility_lists = new std::vector<UtilityList*>;
	};
	void addUtilityList(int name);
private:
	std::vector<UtilityList*>* utility_lists;
};