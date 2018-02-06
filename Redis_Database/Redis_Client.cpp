#include <iostream>
#include <string>
#include <vector>


#include "include/redox.hpp"
#include "include/Redis_Client.hpp"
#include "include/FileData.hpp"

using namespace std;
using redox::Redox;
using redox::Command;
using redox::Subscriber;


void Redis_Client::Redis_HMSET(FileData& file)
{	
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HMSET", file.location, "File_Name", file.fileName, "File_Size", to_string(file.fileSize), "Times_Accessed", to_string(file.timesAccessed), "Last_Modified", to_string(file.lastModified), "Is_Local", to_string(file.isLocal)});
	if(c.ok()) {
		cout << c.cmd() <<": " << c.reply() << endl;
	}
	else {
		cerr << "Command has error code " << c.status() << endl;
	}
	c.free();
	rdx.disconnect();
}


// Change this function to return a struct of FileData
vector<string> Redis_Client::Redis_HGETALL(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"HGETALL", key});
	c.free();
		
	rdx.disconnect();
	return c.reply();
}

// THIS FUNCTION NEEDS TESTING! RETURNS A SEGMENTATION FAULT!!!
vector<string> Redis_Client::Redis_List_All_Keys()
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	cout << "BREAK\n";
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"KEYS", "*"});
	cout << "BREAK\n";
	c.free();	
	cout << "BREAK\n";
	rdx.disconnect();
	cout << "BREAK\n";
	return c.reply();
}


void Redis_Client::incrementTimesAccessed(string location)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"INCR", "Times_Accessed"});
	if(c.ok()) {
		cout << "The times accessed value incremented to " << c.reply() << endl;
	}
	else {
		cerr << "Command has error code " << c.status() << endl;
	}
	c.free();
	rdx.disconnect();
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


void Redis_Client::setFileSize(string location, string fileSize)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"HSET", location,"File_Size", fileSize});
	c.free();
	rdx.disconnect();
}


string Redis_Client::getFileSize(string key)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<string>& c = rdx.commandSync<string>({"HGET", key, "File_Size"});
	if(!c.ok()) {
		cout << "Command has error code " << c.reply() << endl;
	}
	c.free();
	rdx.disconnect();
	return c.reply();
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




