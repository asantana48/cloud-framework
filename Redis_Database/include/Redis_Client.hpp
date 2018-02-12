#ifndef Redis_Client_H
#define Redis_Client_H

#include <iostream>
#include <string>
#include <vector>

#include "redox.hpp"
#include "FileData.hpp"


using namespace std;
using redox::Redox;
using redox::Command;
using redox::Subscriber;



class Redis_Client
{
	Redox rdx;

	public:
	
	void Redis_HMSET(FileData&);
	FileData Redis_HGETALL(string);
	vector<string> Redis_List_All_Keys();

	void setFileName(string, string);
	string getFileName(string);

	void setFileSize(string, string);
	int getFileSize(string);

	void incrementTimesAccessed(string);
	int getTimesAccessed(string);

	void setIsLocal(string, bool);
	bool getIsLocal(string);
	
	void updateLastTimeModified(string);
	time_t getLastTimeModified(string);
	
	string deleteFile(string);
	time_t stringToTime_t(string);
};


#endif
