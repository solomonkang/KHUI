#include "stdafx.h"
#include "DataCollection.h"
#include "Builder.h"

using namespace std;

int main(int argc, char *argv[]){
	DataCollection* utilitylists = new DataCollection();
	Builder* builder = new Builder();
	builder->loadingDatabase("123", utilitylists);
	return 0;
}