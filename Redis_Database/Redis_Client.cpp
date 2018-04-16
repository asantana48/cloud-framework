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


void Redis_Client::Redis_HMSET(FileData& file)
{	
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HMSET", file.localURI, "File_Name", file.fileName, "Remote_URI", file.remoteURI, "File_Size", to_string(file.fileSize), "Times_Accessed", to_string(file.timesAccessed), "Last_Modified", ctime (&file.lastModified), "Is_Local", to_string(file.isLocal), "Is_Metadata", to_string(file.isMetadata)});
	if(c.ok()) {
		cout << file.fileName << " successfully added to the database!\n";
		Redis_Scanner RS;
		RS.addFileToAllSets(file);
	}
	else {
		cerr << "Command has error code " << c.status() << endl;
	}
	c.free();
	rdx.disconnect();
}

void Redis_Client::Redis_DEL(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"DEL", key});
	c.free();	
	rdx.disconnect();
}

void Redis_Client::Redis_RENAME(string originalKey, string newKey)
{
	Redox rdx;
	Redis_Scanner RS;
	rdx.connect("localhost", 6379);
	FileData oldFile = Redis_HGETALL(originalKey);
	Command<string>& c = rdx.commandSync<string>({"RENAME", originalKey, newKey});
	FileData newFile = Redis_HGETALL(newKey);
	RS.deleteFileFromAllSets(oldFile);
	RS.addFileToAllSets(newFile);
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
	
	if (!metaData.empty()) {
		file.localURI = key;
		file.fileName = metaData[1];
		file.remoteURI = metaData[3];
		file.fileSize = stoi(metaData[5]);
		file.timesAccessed = stoi(metaData[7]);
		file.lastModified = stringToTime_t(metaData[9]);
		file.isLocal = stoi(metaData[11]);
		file.isMetadata = stoi(metaData[13]);
	}
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

void Redis_Client::setRemoteURI(string key, string remoteURI)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", key,"Remote_URI", remoteURI});
	c.free();
	rdx.disconnect();
	updateLastTimeModified(key);
}


string Redis_Client::getRemoteURI(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HGET", key, "Remote_URI"});
	if(!c.ok()) {
		cout << "Command has error code " << c.reply() << endl;
	}
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
	updateLastTimeModified(key);
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


void Redis_Client::setFileSize(string key, int fileSize)
{
	Redox rdx;
	Redis_Scanner RS;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", key,"File_Size", to_string(fileSize)});
	c.free();
	rdx.disconnect();
	FileData fd = Redis_HGETALL(key);
	RS.updateFileInFileSizeSet(fd);
	updateLastTimeModified(key);
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
	Redis_Scanner RS;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", key, "Times_Accessed", to_string(times_accessed)});
	c.free();
	rdx.disconnect();
	FileData fd = Redis_HGETALL(key);
	RS.updateFileInHitList(fd);
	updateLastTimeModified(key);
}


int Redis_Client::getTimesAccessed(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HGET", key, "Times_Accessed"});
	if(!c.ok()) {
		cout << "Command has error code " << c.reply() << endl;
		return -1;
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
	updateLastTimeModified(key);
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

void Redis_Client::setIsMetadata(string key, bool isMetadata)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", key, "Is_Metadata", to_string(isMetadata)});
	c.free();
	rdx.disconnect();
	updateLastTimeModified(key);
}


bool Redis_Client::getIsMetadata(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HGET", key, "Is_Metadata"});
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
	Redis_Scanner RS;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", key, "Last_Modified", ctime(&currentTime)});
	if(!c.ok()) {
		cout << "Command has error code " << c.reply() << endl;
		return;
	}
	FileData fd = Redis_HGETALL(key);
	RS.updateFileInLastModifiedSet(fd);
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


time_t Redis_Client::stringToTime_t(string stringTime)
{
	const char *time_details = stringTime.c_str();
	struct tm tm;
	strptime(time_details, "%A %B %d %H:%M:%S %Y", &tm);
	time_t t = mktime(&tm);
	return t;
}
