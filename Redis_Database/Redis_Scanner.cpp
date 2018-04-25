#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include "stdlib.h"

#include "include/redox.hpp"
#include "include/Redis_Client.hpp"
#include "include/Redis_Scanner.hpp"


using namespace std;
using redox::Redox;
using redox::Command;
using redox::Subscriber;

bool Redis_Scanner::orderFiles(FileData& file1, FileData& file2)
{
	return (file1.localURI < file2.localURI);
}

void Redis_Scanner::sortVector(vector<FileData>& files)
{
	Redis_Scanner RS;
    sort(files.begin(), files.end(), RS.orderFiles);
}


/* Add file name and file size to a sorted list
*
* @param FileData& file
*	FileData object containing all values related to a file 
*/
void Redis_Scanner::addToFileSizeSet(FileData& file)
{
	Redox rdx;
	rdx.connect("localhost", 6379);

	Command<int>& c = rdx.commandSync<int>({"ZADD", "File_Size", to_string(file.fileSize), file.localURI});
	if(c.reply() != 1)
	{
		cout << "Error while adding "<< file.fileName << " to File_Size set!\n";
	}
	c.free();
	rdx.disconnect();
}

void Redis_Scanner::updateFileInFileSizeSet(FileData& file)
{
	string key = file.localURI;
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"ZREM", "File_Size", key});
	addToFileSizeSet(file);
	c.free();
	rdx.disconnect();
}


/* Return a vector of file localURIs with a file size between the 
* lowLimit and the highLimit. The low and high limits are inclusive
* values when comparing file size values.
*
* @param SizePolicy policy
*	Contains the high and low size thresholds
*
* @return vector<string> files
*	Contains all of the file localURIs of files that fit the file size policy
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
	sortVector(files);
	return files;
}

vector<FileData> Redis_Scanner::getFilesOutOfSizeRange(SizePolicy policy)
{
	string lowLimit = "(" + to_string(policy.getLowThreshold());
	string highLimit = "(" + to_string(policy.getHighThreshold());
	Redox rdx;
	vector<FileData> files;
	rdx.connect("localhost", 6379);

	// Grab all files with a size less than the lowLimit
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "File_Size", "-inf", lowLimit});
	vector<string> temp(c.reply());
	Redis_Client RC;
	for (int i=0; i< temp.size(); i++)
	{
		files.push_back(RC.Redis_HGETALL(temp[i]));
	}

	// Grab all files with a size greater than the highLimit 
	Command<vector<string>>& c1 = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "File_Size", highLimit, "+inf"});
	vector<string> temp1(c1.reply());
	for (int i=0; i< temp1.size(); i++)
	{
		files.push_back(RC.Redis_HGETALL(temp1[i]));
	}

	c.free();
	c1.free();
	rdx.disconnect();
	sortVector(files);
	return files;
}


/* Add file name and last time modified to a sorted list
*
* @param FileData& file
*	FileData object containing all values related to a file 
*/
void Redis_Scanner::addToLastModifiedSet(FileData& file)
{
	Redox rdx;
	rdx.connect("localhost", 6379);

	Command<int>& c = rdx.commandSync<int>({"ZADD", "Last_Modified", to_string(static_cast<long int>(file.lastModified)), file.localURI});
	if(c.reply() != 1)
	{
		cout << "Error while adding "<< file.fileName << " to Last_Modified set!\n";
	}
	c.free();
	rdx.disconnect();
}

void Redis_Scanner::updateFileInLastModifiedSet(FileData& file)
{
	string key = file.localURI;
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"ZREM", "Last_Modified", key});
	
	addToLastModifiedSet(file);
	
	c.free();
	rdx.disconnect();
}

/* Return a vector of file localURIs with a last modified time
* that is greater than the threshold time set in the TimePolicy file.
*
* @param TimePolicy policy
*
* @return vector<string> files
*	Contains all of the file localURIs of files that fit the time policy
*/
vector<FileData> Redis_Scanner::getFilesInLastModifiedTime(TimePolicy policy)
{
	long int timeThreshold = static_cast<long int>(time(NULL)) - policy.getMinimumAge();

	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "Last_Modified", "-inf", to_string(timeThreshold)});
	vector<string> temp(c.reply());
	Redis_Client RC;
	vector<FileData> files;
	for (int i=0; i< temp.size(); i++)
	{
		files.push_back(RC.Redis_HGETALL(temp[i]));
	}
	c.free();
	rdx.disconnect();
	sortVector(files);
	return files;
}

vector<FileData> Redis_Scanner::getFilesOutOfLastModifiedTime(TimePolicy policy)
{
	long int timeThreshold = static_cast<long int>(time(NULL)) - policy.getMinimumAge();

	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "Last_Modified", to_string(timeThreshold), "+inf"});
	vector<string> temp(c.reply());
	Redis_Client RC;
	vector<FileData> files;
	for (int i=0; i< temp.size(); i++)
	{
		files.push_back(RC.Redis_HGETALL(temp[i]));
	}
	c.free();
	rdx.disconnect();
	sortVector(files);
	return files;
}

/* Add file name and total times accessed to a sorted set
*
* @param FileData& file
*	FileData object containing all values related to a file 
*/
void Redis_Scanner::addToHitList(FileData& file)
{
	Redox rdx;
	rdx.connect("localhost", 6379);

	Command<int>& c = rdx.commandSync<int>({"ZADD", "Times_Accessed", to_string(file.timesAccessed), file.localURI});
	if(c.reply() != 1)
	{
		cout << "Error while adding "<< file.fileName << " to Times_Accessed set!\n";
	}
	c.free();
	rdx.disconnect();
}

void Redis_Scanner::updateFileInHitList(FileData& file)
{
	string key = file.localURI;
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"ZREM", "Times_Accessed", key});
	addToHitList(file);
	c.free();
	rdx.disconnect();
}

// Find files that meet time accessed demotion policy
vector<FileData> Redis_Scanner::getFilesInTimesAccessedRange(HitPolicy policy)
{
	string hitsThreshold = "(" + to_string(policy.getHits());


	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "Times_Accessed", "-inf", hitsThreshold});
	vector<string> temp(c.reply());
	Redis_Client RC;
	vector<FileData> files;
	for (int i=0; i< temp.size(); i++)
	{
		files.push_back(RC.Redis_HGETALL(temp[i]));
	}
	c.free();
	rdx.disconnect();
	sortVector(files);
	return files;
}

// Find files that meet time accessed promotion policy
vector<FileData> Redis_Scanner::getFilesOutOfTimesAccessedRange(HitPolicy policy)
{
	int hitsThreshold = policy.getHits();

	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "Times_Accessed", to_string(hitsThreshold), "+inf"});
	vector<string> temp(c.reply());
	Redis_Client RC;
	vector<FileData> files;
	for (int i=0; i< temp.size(); i++)
	{
		files.push_back(RC.Redis_HGETALL(temp[i]));
	}
	c.free();
	rdx.disconnect();
	sortVector(files);
	return files;
}


void Redis_Scanner::addToIsLocalList(FileData& file)
{
	Redox rdx;
	rdx.connect("localhost", 6379);

	Command<int>& c = rdx.commandSync<int>({"ZADD", "Is_Local", to_string(file.isLocal), file.localURI});
	if(c.reply() != 1)
	{
		cout << "Error while adding "<< file.fileName << " to Is_Local set!\n";
	}
	c.free();
	rdx.disconnect();
}

void Redis_Scanner::updateFileInIsLocalList(FileData& file)
{
	string key = file.localURI;
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"ZREM", "Is_Local", key});
	addToIsLocalList(file);
	c.free();
	rdx.disconnect();
}


vector<FileData> Redis_Scanner::getLocalFiles()
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "Is_Local", "1", "1"});
	vector<string> temp(c.reply());
	Redis_Client RC;
	vector<FileData> files;
	for (int i=0; i< temp.size(); i++)
	{
		files.push_back(RC.Redis_HGETALL(temp[i]));
	}
	c.free();
	rdx.disconnect();
	sortVector(files);
	return files;
}

vector<FileData> Redis_Scanner::getNonLocalFiles()
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "Is_Local", "0", "0"});
	vector<string> temp(c.reply());
	Redis_Client RC;
	vector<FileData> files;
	for (int i=0; i< temp.size(); i++)
	{
		files.push_back(RC.Redis_HGETALL(temp[i]));
	}
	c.free();
	rdx.disconnect();
	sortVector(files);
	return files;
}

void Redis_Scanner::addFileToAllSets(FileData& file)
{
	addToFileSizeSet(file);
	addToHitList(file);
	addToIsLocalList(file);
	addToLastModifiedSet(file);
}

void Redis_Scanner::deleteFileFromAllSets(FileData& file)
{
	string key = file.localURI;
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& a = rdx.commandSync<int>({"ZREM", "File_Size", key});
	Command<int>& b = rdx.commandSync<int>({"ZREM", "Is_Local", key});
	Command<int>& c = rdx.commandSync<int>({"ZREM", "Times_Accessed", key});
	Command<int>& d = rdx.commandSync<int>({"ZREM", "Last_Modified", key});

	a.free();
	b.free();
	c.free();
	d.free();
	rdx.disconnect();
}