#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "stdlib.h"

#include "include/redox.hpp"
#include "include/Redis_Client.hpp"
#include "include/FileData.hpp"


using namespace std;
using redox::Redox;
using redox::Command;
using redox::Subscriber;

int main(int argc, char* argv[])
{
	Redox rdx;
	
	if(!rdx.connect("localhost", 6379))
	{
		cout << "No connection made!\n"; 
		return -1;
	}
	
	cout << "Connection made!\n";
	
	rdx.command<string>({"FLUSHALL"}, [](Command<string>& c) {
		if(c.ok()) {
			cout << "Hello, async " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	});

	
	Redis_Client RC;

	time_t currentTime;

	struct FileData File1 = {"/home/file1", "File1", 500, 12, currentTime, true};
	struct FileData File2 = {"/home/file2", "File2", 260, 25, currentTime, true};

	RC.Redis_HMSET(File1);
	RC.Redis_HMSET(File2);
	
	vector<string> metaData = RC.Redis_HGETALL("/home/file1");

	
	for (int i=0; i<metaData.size(); i++)
	{ 
		cout << metaData[i] << endl;
	}

	
	/*vector<string> keys = RC.Redis_List_All_Keys();
	cout << "BREAK\n";
	*/

	/*
	for (int i=0; i<keys.size(); i++)
	{ 
		cout << keys[i] << endl;
	}
	*/

	
}
