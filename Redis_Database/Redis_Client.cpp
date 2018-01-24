#include <iostream>
#include <string>
#include <vector>

#include "include/redox.hpp"
#include "include/Redis_Client.hpp"

using namespace std;
using redox::Redox;
using redox::Command;
using redox::Subscriber;

void increment_Key()
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	Command<int>& c = rdx.commandSync<int>({"INCR", "Key"});
		if(c.ok()) {
			cout << "The key value is at " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	c.free();
	rdx.disconnect();
}


string get_Key()
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	string key = rdx.get("Key");
	increment_Key();
	rdx.stop();
	rdx.disconnect();
	return key;
}

void Redis_HMSET(string fileName, string fileSize)
{
	Redox rdx;
	
	rdx.connect("localhost", 6379);
	
	string redisKey = get_Key();
	Command<string>& c = rdx.commandSync<string>({"HMSET", redisKey, "File_Name", fileName, "File_Size", fileSize});
			if(c.ok()) {
				cout << c.cmd() <<": " << c.reply() << endl;
			}
			else {
				cerr << "Command has error code " << c.status() << endl;
			}
	c.free();
	rdx.disconnect();
}


vector<string> Redis_HGETALL(string key)
{
	Redox rdx;
	
	rdx.connect("localhost", 6379);

	Command<vector<string>>& c = rdx.commandSync<vector<string>>({"HGETALL", key});

	cout << c.cmd() << ": " << c.status() << endl;

	c.free();
		
	rdx.disconnect();

	return c.reply();
}




