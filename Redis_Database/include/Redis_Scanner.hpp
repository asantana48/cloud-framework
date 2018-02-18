#ifndef Redis_Scanner_H
#define Redis_Scanner_H


#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "stdlib.h"

#include "redox.hpp"
#include "Redis_Client.hpp"
#include "FileData.hpp"


using namespace std;
using redox::Redox;
using redox::Command;
using redox::Subscriber;


class Redis_Scanner
{

public:
	void addToFileSizeSet(FileData&);
	vector<string> getFilesInSizeRange(int, int);
};

#endif