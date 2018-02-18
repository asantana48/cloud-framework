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

/* Add file name and file size to a sorted list
*
* @param FileData& file
*	FileData object containing all values related to a file 
*/
void Redis_Scanner::addToFileSizeSet(FileData& file)
{
	Redox rdx;
	rdx.connect("localhost", 6379);

	Command<int>& c = rdx.commandSync<int>({"ZADD", "File_Size", to_string(file.fileSize), file.location});
	if(c.reply() == 1)
	{
		cout << file.fileName << " successfully added to set!\n";
	}
	else
	{
		cout << "Error while adding file to set!\n";
	}
	c.free();
	rdx.disconnect();
}

/* Return a vector of file locations with a file size between the 
* lowLimit and the highLimit. The low and high limits are inclusive
* values when comparing file size values.
*
* @param int lowLimit
*	The low boundary for the fileSize policy
*
* @param int highLimit
*	The high boundary for the fileSize policy
*
* @return vector<string> c
*	Contains all of the file locations of files that fit the file size policy
*/
vector<string> Redis_Scanner::getFilesInSizeRange(int lowLimit, int highLimit)
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	cout << "BREAK!\n";
	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"ZRANGEBYSCORE", "File_Size", to_string(lowLimit), to_string(highLimit)});
	vector<string> temp(c.reply());
	c.free();
	cout << "BREAK!\n";
	rdx.disconnect();
	cout << "BREAK!\n";
	return temp;
}

