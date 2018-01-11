#include <iostream>
#include <redox.hpp>

using namespace std;
using redox::Redox;
using redox::Command;
using redox::Subscriber;

int initializeTestDatabase();

int main()
{
	int status = initializeTestDatabase();
	cout << status;
	
	return 0;
}

int initializeTestDatabase()
{
	Redox rdx;
	if(!rdx.connect("localhost", 6379)) return -1;
	
	rdx.command<string>({"FLUSHALL"}, [](Command<string>& c) {
		if(c.ok()) {
			cout << "Hello, async " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	});
	
	rdx.command<string>({"HMSET", "1", "File_Name", "File_1", "File_Size", "35"}, [](Command<string>& c) {
		if(c.ok()) {
			cout << "Hello, async " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	});
	
	rdx.command<string>({"HMSET", "2", "File_Name", "File_2", "File_Size", "10"}, [](Command<string>& c) {
		if(c.ok()) {
			cout << "Hello, async " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	});
	
	rdx.command<string>({"HMSET", "3", "File_Name", "File_3", "File_Size", "40"}, [](Command<string>& c) {
		if(c.ok()) {
			cout << "Hello, async " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	});

	rdx.command<string>({"HMSET", "4", "File_Name", "File_4", "File_Size", "15"}, [](Command<string>& c) {
		if(c.ok()) {
			cout << "Hello, async " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	});

	rdx.command<string>({"HMSET", "5", "File_Name", "File_5", "File_Size", "40"}, [](Command<string>& c) {
		if(c.ok()) {
			cout << "Hello, async " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	});

	rdx.command<string>({"HMSET", "6", "File_Name", "File_6", "File_Size", "1000"}, [](Command<string>& c) {
		if(c.ok()) {
			cout << "Hello, async " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	});

	rdx.command<string>({"HMSET", "7", "File_Name", "File_7", "File_Size", "10"}, [](Command<string>& c) {
		if(c.ok()) {
			cout << "Hello, async " << c.reply() << endl;
		}
		else {
			cerr << "Command has error code " << c.status() << endl;
		}
	});	
	
	return 1;
}	