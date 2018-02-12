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

	//struct tm * currentTime;
	time_t currentTime;
	time(&currentTime);
	//currentTime = localtime(&t);

	struct FileData File1 = {"/home/file1", "File1", 500, 120, currentTime, true};
	struct FileData File2 = {"/home/file2", "File2", 260, 250, currentTime, true};
	struct FileData File3 = {"/home/file3", "File3", 1024, 3000, currentTime, true};
	struct FileData File4 = {"/home/file4", "File4", 780, 1000, currentTime, true};

	RC.Redis_HMSET(File1);
	RC.Redis_HMSET(File2);
	RC.Redis_HMSET(File3);
	RC.Redis_HMSET(File4);
	
	
	FileData metaData = RC.Redis_HGETALL("/home/file1");

	cout << "File location: " << metaData.location << endl;
	cout << "File name: " << metaData.fileName << endl;
	cout << "File size: " << metaData.fileSize << endl;
	cout << "Times accessed: " << metaData.timesAccessed << endl;
	cout << "Last time modified: " << ctime(&metaData.lastModified) << endl;
	cout << "Is local: " << metaData.isLocal << endl;
	

	
	vector<string> keys = RC.Redis_List_All_Keys();
	

	
	for (int i=0; i<keys.size(); i++)
	{ 
		cout << keys[i] << endl;
	}
	

	RC.setFileName("/home/file3", "/home/file3A");

	cout << "Name for file 3 is: " << RC.getFileName("/home/file3") << endl;

	RC.setFileSize("/home/file3", "1000");

	cout << "File size for file 3 is: " << RC.getFileSize("/home/file3") << endl;

	for(int i = 0; i<100; i++)
	{
		RC.incrementTimesAccessed("/home/file3");
	}

	cout << "File 3 has been accessed: " << RC.getTimesAccessed("/home/file3") << " times" << endl;


	RC.setIsLocal("/home/file3", false);

	cout << "File 3 is local? " << RC.getIsLocal("/home/file3") << endl;

	RC.updateLastTimeModified("/home/file3");

	time_t time = RC.getLastTimeModified("/home/file3");

	cout << "File 3 was last modified at: " << ctime(&time) << endl;
	
/*	
	RC.deleteFile("/home/file3");
	
	cout << "File size for file 3 is: " << RC.getFileSize("/home/file3") << endl;
*/
	
	
}
