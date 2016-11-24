#pragma once
#include <string>
class DataCollection;
class Builder {
public:
	void loadingDatabase(std::string file_path, DataCollection* utility_lists);
};