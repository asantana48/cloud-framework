#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "stdlib.h"


#include "include/redox.hpp"
#include "include/Redis_Client.hpp"
#include "include/FileData.hpp"
#include "include/Redis_Scanner.hpp"

using namespace std;
using redox::Redox;
using redox::Command;
using redox::Subscriber;


void Redis_Client::Redis_HMSET(FileData& file)
{	
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HMSET", file.location, "File_Name", file.fileName, "File_Size", to_string(file.fileSize), "Times_Accessed", to_string(file.timesAccessed), "Last_Modified", ctime (&file.lastModified), "Is_Local", to_string(file.isLocal)});
	if(c.ok()) {
		cout << c.cmd() <<": " << c.reply() << endl;
		Redis_Scanner RS;
		RS.addToFileSizeSet(file);
	}
	else {
		cerr << "Command has error code " << c.status() << endl;
	}
	c.free();
	rdx.disconnect();
}



FileData Redis_Client::Redis_HGETALL(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"HGETALL", key});
	
	vector<string> metaData = c.reply();

	FileData file;
	file.location = key;
	file.fileName = metaData[1];
	file.fileSize = stoi(metaData[3]);
	file.timesAccessed = stoi(metaData[5]);
	file.lastModified = stringToTime_t(metaData[7]);
	file.isLocal = stoi(metaData[9]);

	c.free();
	rdx.disconnect();

	return file;
}


vector<string> Redis_Client::Redis_List_All_Keys()
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"KEYS", "*"});
	c.free();	
	rdx.disconnect();
	return c.reply();
}


void Redis_Client::setFileName(string key, string fileName)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", key,"File_Name", fileName});
	c.free();
	rdx.disconnect();
}


string Redis_Client::getFileName(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HGET", key, "File_Name"});
	if(!c.ok()) {
		cout << "Command has error code " << c.reply() << endl;
	}
	c.free();
	rdx.disconnect();
	return c.reply();
}


void Redis_Client::setFileSize(string key, string fileSize)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", key,"File_Size", fileSize});
	c.free();
	rdx.disconnect();
}


int Redis_Client::getFileSize(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HGET", key, "File_Size"});
	if(!c.ok()) {
		cout << "Command has error code " << c.reply() << endl;
	}
	c.free();
	rdx.disconnect();
	return stoi(c.reply());
}


void Redis_Client::incrementTimesAccessed(string key)
{
	int times_accessed = getTimesAccessed(key);
	times_accessed += 1;
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", key, "Times_Accessed", to_string(times_accessed)});
	c.free();
	rdx.disconnect();
}


int Redis_Client::getTimesAccessed(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HGET", key, "Times_Accessed"});
	if(!c.ok()) {
		cout << "Command has error code " << c.reply() << endl;
	}
	c.free();
	rdx.disconnect();
	return stoi(c.reply());
}


void Redis_Client::setIsLocal(string key, bool isLocal)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", key, "Is_Local", to_string(isLocal)});
	c.free();
	rdx.disconnect();
}


bool Redis_Client::getIsLocal(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HGET", key, "Is_Local"});
	if(!c.ok()) {
		cout << "Command has error code " << c.reply() << endl;
	}
	int answer = stoi(c.reply());
	c.free();
	rdx.disconnect();
	if (answer == 1){ return true;}
	else {return false;}
}


void Redis_Client::updateLastTimeModified(string key)
{
	time_t currentTime;
	time(&currentTime); 
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", key, "Last_Modified", ctime(&currentTime)});
	if(!c.ok()) {
		cout << "Command has error code " << c.reply() << endl;
	}
	c.free();
	rdx.disconnect();
}

time_t Redis_Client::getLastTimeModified(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HGET", key, "Last_Modified"});
	if(!c.ok()) {
		cout << "Command has error code " << c.reply() << endl;
	}
	time_t time = stringToTime_t(c.reply());
	c.free();
	rdx.disconnect();
	return time;
}


string Redis_Client::deleteFile(string key)
{
	string reply;
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"DEL", key});
	if(c.reply() == 0) {
		reply = "Error while deleting file\n";
	}
	reply = "File was successfully deleted!\n"; 
	c.free();
	rdx.disconnect();
	return reply;
}


time_t Redis_Client::stringToTime_t(string stringTime)
{
	const char *time_details = stringTime.c_str();
	struct tm tm;
	strptime(time_details, "%A %B %d %H:%M:%S %Y", &tm);
	time_t t = mktime(&tm);
	return t;
}
