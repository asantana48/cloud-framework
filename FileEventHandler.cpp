#include "lib/FileEventHandler.hpp"
#include "Redis_Database/include/Redis_Client.hpp"

#include <iostream>
#include <fstream>

using namespace std;

int MAX_EVENTS = 1024; // Max number of events to process in one go
int LEN_NAME = 128; // The length of a file name won't exceed this number of bytes
int EVENT_SIZE = sizeof(struct inotify_event); // The size of one event struct
int BUF_LEN = MAX_EVENTS * (EVENT_SIZE + LEN_NAME);

void FileEventHandler::initializeINotify(FileData& file)
{
	
	char buffer[BUF_LEN];
	int pid, length, i = 0, wd;
	int fd;
	char* ptr;
	const struct inotify_event *event;

	Redis_Client RC;


	fd = inotify_init();

	if (fd < 0)
	{
		cout << "Failed to initialize inotify!\n";
		return;
	}
	
	// This is where the different events are specified
	// If needed, more events can be added as the third parameter
	// wd = inotify(fd, file.localURI, IN_ACCESS | IN_DELETE | IN_MODIFY); and so on
	wd = inotify_add_watch(fd, file.localURI.c_str(), IN_ACCESS | IN_OPEN);

	if (wd == -1)
	{
		cout << "Failed to add watch to " << file.localURI << "\n";
		return;
	}
	else
	{
		cout << "Watching " << file.localURI << "\n";
	}

	cout << "Fork made!\n";
	pid = fork();
	cout << "pid: " << pid << endl;

	if (pid == 0)
	{
		cout << "Event Watcher created!\n";

		while(1 < 2)
		{
			length = read(fd, buffer, BUF_LEN);

			if (length < 0)
			{
				cout << "Read error! No events found!\n";
			}

			for (ptr = buffer; ptr < buffer + length;
				ptr += sizeof(struct inotify_event) + event->len)
			{
				event = (const struct inotify_event *) ptr;
				printf("BREAK!\n");

				if (event->mask & IN_OPEN)
				{
					printf("File opened: ");
					for(int j=0; j<strlen(event->name); j++)
					{
						printf("%c",event->name[j]);
					}
				}
			}
		}
	}

	else if (pid > 0)
	{
		cout << "Parent fork created!\n";
		return;
	}

	else
	{
		cout << "Start of Event Watcher failed!\n";
		return;
	}
}


void FileEventHandler::test()
{
	ofstream file;
	int fd;
	file.open("testbed/TestText.txt");
	file.close();
	file.open("testbed/TestText.txt");
	file.close();
	file.open("testbed/TestText.txt");
	file.close();	
	file.open("testbed/TestText.txt");
	file.close();
	file.open("testbed/TestText.txt");
	file.close();		
}