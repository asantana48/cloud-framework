#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "stdlib.h"

#include "include/Redis_Scanner.hpp"
#include "include/Redis_Client.hpp"

using namespace std;
//using redox::Redox;
//using redox::Command;
//using redox::Subscriber;

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

	struct FileData File1 = {"/home/file1", "File1", 500, 120, static_cast<long int>(currentTime) - 865000, true};
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

	
	SizePolicy SP(0, 1000);
	vector<FileData> fileSizes1 = RS.getFilesInSizeRange(SP);
	cout << "Return files with size between 0 and 1000 bytes\n";
	for (int i=0; i<fileSizes1.size(); i++)
	{
		cout << fileSizes1[i].fileName << endl;
	}

	/*
	vector<FileData> fileSizes2 = RS.getFilesInSizeRange(300, 700);
	cout << "Return files with size between 300 and 700 bytes\n";
	for (int i=0; i<fileSizes2.size(); i++)
	{
		cout << fileSizes2[i].fileName << endl;
	}

	vector<FileData> fileSizes3 = RS.getFilesInSizeRange(260, 1024);
	cout << "Return files with size between 260 and 1024 bytes\n";
	for (int i=0; i<fileSizes3.size(); i++)
	{
		cout << fileSizes3[i].fileName << endl;
	}

	vector<FileData> fileSizes4 = RS.getFilesInSizeRange(1000, 0);
	cout << "Return files with size between 1000 and 0 bytes\n";
	for (int i=0; i<fileSizes4.size(); i++)
	{
		cout << fileSizes4[i].fileName << endl;
	}
	*/
	
	TimePolicy TP(0, 0, 10, 0, 0, 0);
	vector<FileData> temp1 = RS.getFilesWithinLastModifiedTime(TP);
	cout << "Return files older than 10 days\n";
	for (int i=0; i<temp1.size(); i++)
	{
		cout << temp1[i].fileName << endl;
	}

}