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



/*
bool orderFiles(FileData& file1, FileData& file2)
{
	return (file1.location < file2.location);
}

void sortVector(vector<FileData>& files)
{
    sort(files.begin(), files.end(), orderFiles);
}
*/

vector<FileData> findIntersection(vector<vector<FileData>> &fileLists, vector<FileData> &intersection, int &i)
{
	Redis_Scanner RS;
	vector<FileData> temp;
	if(i==0)
	{
		set_intersection(fileLists[i].begin(), fileLists[i].end(), fileLists[i+1].begin(), fileLists[i+1].end(), back_inserter(temp), RS.orderFiles);
    	RS.sortVector(temp);
    	i = 2;
    	cout << "BREAK 1\n";
    	intersection = findIntersection(fileLists, temp, i);
   	}

   	else
   	{
	   	while(i < fileLists.size())
		{
	    	set_intersection(fileLists[i].begin(), fileLists[i].end(), intersection.begin(), intersection.end(), back_inserter(temp), RS.orderFiles);
	    	RS.sortVector(temp);
	    	i++;
	    	cout << "BREAK 2\n";
	    	intersection = findIntersection(fileLists, temp, i);
		}
	}
	cout << "BREAK 3\n";
	return intersection;
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
	struct FileData File3 = {"/home/file3", "File3", 1024, 790, static_cast<long int>(currentTime), true};
	struct FileData File4 = {"/home/file4", "File4", 780, 700, static_cast<long int>(currentTime) - 865000, true};

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
	vector<FileData> temp31 = RS.getFilesInSizeRange(SP);
	cout << "Return files with size between 0 and 1000 bytes\n";
	for (int i=0; i<temp31.size(); i++)
	{
		cout << temp31[i].fileName << endl;
	}

	vector<FileData> temp32 = RS.getFilesOutOfSizeRange(SP);
	cout << "Return files with size outside of 0 and 1000 bytes\n";
	for (int i=0; i<temp32.size(); i++)
	{
		cout << temp32[i].fileName << endl;
	}

	TimePolicy TP(0, 0, 10, 0, 0, 0);
	vector<FileData> temp11 = RS.getFilesInLastModifiedTime(TP);
	cout << "Return files older than 10 days\n";
	for (int i=0; i<temp11.size(); i++)
	{
		cout << temp11[i].fileName << endl;
	}

	vector<FileData> temp12 = RS.getFilesOutOfLastModifiedTime(TP);
	cout << "Return files younger than 10 days\n";
	for (int i=0; i<temp12.size(); i++)
	{
		cout << temp12[i].fileName << endl;
	}

	HitPolicy HP(800);
	vector<FileData> temp21 = RS.getFilesInTimesAccessedRange(HP);
	cout << "Return files with less than 800 hits\n";
	for (int i=0; i<temp21.size(); i++)
	{
		cout << temp21[i].fileName << endl;
	}
	
	vector<FileData> temp22 = RS.getFilesOutOfTimesAccessedRange(HP);
	cout << "Return files with more than 800 hits\n";
	for (int i=0; i<temp22.size(); i++)
	{
		cout << temp22[i].fileName << endl;
	}
	

	vector<FileData> demotionList;
	vector<FileData> intersection;
    int i = 0;

    // Grab and sort all files within file size policy range
    vector<FileData> temp1 = RS.getFilesInSizeRange(SP);
    //sortVector(temp1);

    // Grab and sort all files within last modified time range
    vector<FileData> temp2 = RS.getFilesInLastModifiedTime(TP);
    //sortVector(temp2);

    // Grab and sort all files within times accessed range
    vector<FileData> temp3 = RS.getFilesInTimesAccessedRange(HP);
    //sortVector(temp3);

    vector<FileData> temp4 = RS.getLocalFiles();
    //sortVector(temp4);

    vector<vector<FileData>> fileLists = {temp1, temp2, temp3};


    demotionList = findIntersection(fileLists, intersection, i);

	cout << "Files to migrate:\n";
	for (int i=0; i<demotionList.size(); i++)
	{
		cout << demotionList[i].fileName << endl;
	}

}

