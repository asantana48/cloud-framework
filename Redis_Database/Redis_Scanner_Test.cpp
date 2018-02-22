#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include "stdlib.h"

#include "include/Redis_Scanner.hpp"
#include "include/Redis_Client.hpp"

using namespace std;
//using redox::Redox;
//using redox::Command;
//using redox::Subscriber;



bool orderFiles(FileData& file1, FileData& file2)
{
	return (file1.location < file2.location);
}



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
	struct FileData File2 = {"/home/file2", "File2", 260, 250, static_cast<long int>(currentTime) - 865000, true};
	struct FileData File3 = {"/home/file3", "File3", 1024, 790, static_cast<long int>(currentTime) - 865000, true};
	struct FileData File4 = {"/home/file4", "File4", 780, 810, static_cast<long int>(currentTime) - 865000, true};

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
	vector<FileData> temp3 = RS.getFilesInSizeRange(SP);
	cout << "Return files with size between 0 and 1000 bytes\n";
	for (int i=0; i<temp3.size(); i++)
	{
		cout << temp3[i].fileName << endl;
	}

	TimePolicy TP(0, 0, 10, 0, 0, 0);
	vector<FileData> temp1 = RS.getFilesInLastModifiedTime(TP);
	cout << "Return files older than 10 days\n";
	for (int i=0; i<temp1.size(); i++)
	{
		cout << temp1[i].fileName << endl;
	}

	HitPolicy HP(800);
	vector<FileData> temp2 = RS.getFilesInTimesAccessedRange(HP);
	cout << "Return files with less than 800 hits\n";
	for (int i=0; i<temp2.size(); i++)
	{
		cout << temp2[i].fileName << endl;
	}

	vector<FileData> finalList;
	vector<FileData> temp4;
	//vector<int>::iterator it;

	sort (temp1.begin(), temp1.end(), orderFiles);
	sort (temp2.begin(), temp2.end(), orderFiles);
	sort (temp3.begin(), temp3.end(), orderFiles);


	set_intersection(temp1.begin(), temp1.end(), temp2.begin(), temp2.end(), back_inserter(temp4), orderFiles);
	set_intersection(temp3.begin(), temp3.end(), temp4.begin(), temp4.end(), back_inserter(finalList), orderFiles);

	cout << "Files to migrate:\n";
	for (int i=0; i<finalList.size(); i++)
	{
		cout << finalList[i].fileName << endl;
	}

}