#include <iostream>
#include <string>
#include <vector>
#include "stdlib.h"

#include "include/redox.hpp"
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
	
			//DO NOT DELETE THIS CODE!!!
/*DO NOT TOUCH THIS KEY!!! */ rdx.set("Key", "1"); /*BACK AWAY FROM THIS LINE!!*/
			// DO NOT DELETE THIS KEY!!!!
	
	Redis_HMSET("File1", "500");
	Redis_HMSET("File2", "800");
	Redis_HMSET("File3", "575");
	
	vector<string> metaData = Redis_HGETALL("1");

	for (int i=0; i<metaData.size(); i++)
	{ 
	cout << metaData[i] << endl;
	}
}
