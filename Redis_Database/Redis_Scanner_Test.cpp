#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "stdlib.h"

#include "include/Redis_Scanner.hpp"
#include "include/Redis_Client.hpp"

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
	Redis_Scanner RS;

	//struct tm * currentTime;
	time_t currentTime;
	time(&currentTime);
	//currentTime = localtime(&t);

	struct FileData File1 = {"/home/file1", "File1", 500, 120, currentTime, true};
	struct FileData File2 = {"/home/file2", "File2", 260, 250, currentTime, true};
	struct FileData File3 = {"/home/file3", "File3", 1024, 3000, currentTime, true};
	struct FileData File4 = {"/home/file4", "File4", 780, 1000, currentTime, true};

	// File sizes will be added to sorted set through this function
	RC.Redis_HMSET(File1);
	RC.Redis_HMSET(File2);
	RC.Redis_HMSET(File3);
	RC.Redis_HMSET(File4);

	cout << "File Sizes\n";
	cout << "/home/file1: " << RC.getFileSize("/home/file1") << endl;
	cout << "/home/file2: " << RC.getFileSize("/home/file2") << endl;
	cout << "/home/file3: " << RC.getFileSize("/home/file3") << endl;
	cout << "/home/file4: " << RC.getFileSize("/home/file4") << endl;

	

	vector<string> fileSizes1 = RS.getFilesInSizeRange(0, 10000);
	cout << "Return files with size between 0 and 10000 bytes\n";
	for (int i=0; i<fileSizes1.size(); i++)
	{
		cout << fileSizes1[i] << endl;
	}

	vector<string> fileSizes2 = RS.getFilesInSizeRange(300, 700);
	cout << "Return files with size between 300 and 700 bytes\n";
	for (int i=0; i<fileSizes2.size(); i++)
	{
		cout << fileSizes2[i] << endl;
	}

	vector<string> fileSizes3 = RS.getFilesInSizeRange(260, 1024);
	cout << "Return files with size between 260 and 1024 bytes\n";
	for (int i=0; i<fileSizes3.size(); i++)
	{
		cout << fileSizes3[i] << endl;
	}


}