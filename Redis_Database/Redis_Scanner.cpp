#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "stdlib.h"

#include "include/redox.hpp"
#include "include/Redis_Client.hpp"
#include "include/Redis_Scanner.hpp"


using namespace std;
using redox::Redox;
using redox::Command;
using redox::Subscriber;

/* Add file name and file size to a sorted list
*
* @param FileData& file
*	FileData object containing all values related to a file 
*/
void Redis_Scanner::addToFileSizeSet(FileData& file)
{
	Redox rdx;
	rdx.connect("localhost", 6379);

	Command<int>& c = rdx.commandSync<int>({"ZADD", "File_Size", to_string(file.fileSize), file.location});
	if(c.reply() == 1)
	{
		cout << file.fileName << " successfully added to file size set!\n";
	}
	else
	{
		cout << "Error while adding file to file size set!\n";
	}
	c.free();
	rdx.disconnect();
}

/* Return a vector of file locations with a file size between the 
* lowLimit and the highLimit. The low and high limits are inclusive
* values when comparing file size values.
*
* @param int lowLimit
*	The low boundary for the fileSize policy
*
* @param int highLimit
*	The high boundary for the fileSize policy
*
* @return vector<string> c
*	Contains all of the file locations of files that fit the file size policy
*/
vector<FileData> Redis_Scanner::getFilesInSizeRange(SizePolicy policy)
{
	int lowLimit = policy.getLowThreshold();
	int highLimit = policy.getHighThreshold();
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "File_Size", to_string(lowLimit), to_string(highLimit)});
	vector<string> temp(c.reply());
	Redis_Client RC;
	vector<FileData> files;
	for (int i=0; i< temp.size(); i++)
	{
		files.push_back(RC.Redis_HGETALL(temp[i]));
	}
	c.free();
	rdx.disconnect();
	return files;
}

/*
vector<FileData> Redis_Scanner::getFilesInSizeRange(SizePolicy policy)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c;
	if(policy.greaterThan == true)
	{
		c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "File_Size", to_string(policy.getLowThreshold), "+inf"});
	}
	else if(policy.greaterThan == false)
	{
		c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "File_Size", "-inf", to_string(policy.getHighThreshold)});
	}
	vector<string> temp(c.reply());
	Redis_Client RC;
	vector<FileData> files;
	for (int i=0; i< temp.size(); i++)
	{
		files.push_back(RC.Redis_HGETALL(temp[i]));
	}
	c.free();
	rdx.disconnect();
	return files;
}
*/


void Redis_Scanner::addToLastModifiedSet(FileData& file)
{
	Redox rdx;
	rdx.connect("localhost", 6379);

	Command<int>& c = rdx.commandSync<int>({"ZADD", "Last_Modified", to_string(file.lastModified), file.location});
	if(c.reply() == 1)
	{
		cout << file.fileName << " successfully added to last modified set!\n";
	}
	else
	{
		cout << "Error while adding file to last modified set!\n";
	}
	c.free();
	rdx.disconnect();
}

/**
vector<FileData> getFilesInWithinLastModifiedTime()
{

}
*/
