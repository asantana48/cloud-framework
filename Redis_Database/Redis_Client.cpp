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
	rdx.command<string>({"INCR", "Key"}, [](Command<string>& c){
		if(c.ok()) {
			cout << "Hello, async " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	});
	rdx.disconnect();
}


string get_Key()
{
	Redox rdx;
	rdx.connect("localhost", 6379);
	string key = rdx.get("Key");
	increment_Key();
	rdx.disconnect();
	return key;
}

void Redis_HMSET(string fileName, string fileSize)
{
	Redox rdx;
	
	rdx.connect("localhost", 6379);
	
		string redisKey = get_Key();
		rdx.command<string>({"HMSET", redisKey, "File_Name", fileName, "File_Size", fileSize}, [](Command<string>& c) {
			if(c.ok()) {
				cout << "Hello, async " << c.reply() << endl;
			}
			else {
				cerr << "Command has error code " << c.status() << endl;
			}
		});
	rdx.disconnect();
}


void Redis_HGETALL(string key)
{
	Redox rdx;
	
	rdx.connect("localhost", 6379);

		rdx.command<string>({"HGETALL", key}, [](Command<string>& c) {
			if(c.ok()) {
				cout << "Hello, async " << c.reply() << endl;
				//return c.reply();
			}
			else {
				cerr << "Command has error code " << c.status() << endl;
				//return c.status;
			}
			
		});
	
}




