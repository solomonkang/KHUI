#include "stdafx.h"
#include "Builder.h"
#include "DataCollection.h"

void Builder::loadingDatabase(std::string file_path, DataCollection* utility_lists)
{
	utility_lists->addUtilityList(1);
	utility_lists->addUtilityList(2);
	utility_lists->addUtilityList(3);
	utility_lists->addUtilityList(4);
	utility_lists->addUtilityList(5);
	utility_lists->addUtilityList(6);
}
