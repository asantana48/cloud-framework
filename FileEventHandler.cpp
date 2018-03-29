#include "lib/FileEventHandler.hpp"
#include "Redis_Database/include/Redis_Client.hpp"
#include "Redis_Database/include/Redis_Scanner.hpp"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int MAX_EVENTS = 1024; // Max number of events to process in one go
int LEN_NAME = 400; // The length of a file name won't exceed this number of bytes
int EVENT_SIZE = sizeof(struct inotify_event); // The size of one event struct
int BUF_LEN = MAX_EVENTS * (EVENT_SIZE + LEN_NAME);

void FileEventHandler::initializeINotify()
{
	
	char buffer[BUF_LEN];
	int pid, length, i = 0, wd[2];
	int fd;
	const struct inotify_event *event;
	char *ptr;
	string directory = FILES_PATH;
	string trash = "/home/graves/.local/share/Trash/files";
	string oldFileName = "";

	Redis_Client RC;
	Redis_Scanner RS;

	cout << "Fork made!\n";
	pid = fork();
	cout << "pid: " << pid << endl;

	// If parent process
	if (pid > 0)
	{
		cout << "Parent fork created!\n";
		return;
	}

	fd = inotify_init();

	if (fd < 0)
	{
		cout << "Failed to initialize inotify!\n";
		return;
	}
	
	// This is where the different events are specified
	// If needed, more events can be added as the third parameter
	// wd = inotify(fd, file.localURI, IN_ACCESS | IN_DELETE | IN_MODIFY); and so on
	wd[0] = inotify_add_watch(fd, directory.c_str(), IN_ALL_EVENTS);

	wd[1] = inotify_add_watch(fd, trash.c_str(), IN_DELETE);

	if (wd[0] == -1)
	{
		cout << "Failed to add watch to " << directory << "\n";
		return;
	}

	else
	{
		cout << "Watching " << directory << "\n";
	}

	if (wd[1] == -1)
	{
		cout << "Failed to add watch to " << trash << "\n";
		return;
	}

	else
	{
		cout << "Watching " << trash << "\n";
	}
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

			if(event->len)
			{
				string key = directory + event->name;

				// If a file is opened, increment "Times_Accessed" in database
				if (event->mask & IN_OPEN)
				{
					// Add conditional to check for directory
					printf("File accessed: %s\n", event->name);
					RC.incrementTimesAccessed(key);
				}

				// If a file is created in the watched directory, add its local URI to the database
				if (event->mask & IN_CREATE)
				{
					printf("File created: %s\n", event->name);
					newFileDataObject(event->name);
				}

				// If a file is deleted, remove its local URI from the database
				if (event->mask & IN_DELETE)
				{
					printf("File deleted: %s\n", event->name);
					FileData fd = RC.Redis_HGETALL(key);
					RS.deleteFileFromAllSets(fd);
					RC.Redis_DEL(key);
				}

				// If a file is modified, update its size and last modified time
				if (event->mask & IN_MODIFY)
				{
					printf("File modified: %s\n", event->name);
					struct stat statObj;
					stat(key.c_str(), &statObj);
					RC.setFileSize(key, statObj.st_size);
				}

				// If a file is moved or renamed, this will grab the new name of the file
				if (event->mask & IN_MOVED_TO)
				{
					printf("File renamed to %s\n", event->name);
					printf("old file directory: %s\n", oldFileName);
					printf("New key: %s\n", key);
					if (strcmp(oldFileName.c_str(), "") != 0)
					{
						RC.Redis_RENAME(oldFileName, key);
						RC.setFileName(key, event->name);
					}

					oldFileName = "";
				}

				// If a file is moved or renamed, this will grab the old name of the file
				if (event->mask & IN_MOVED_FROM)
				{
					printf("File %s is being changed\n", event->name);
					if (strcmp(oldFileName.c_str(), "") == 0)
					{
						oldFileName = key;
					}
				}
			}
		}
	}
}

// Creates a new entry in the Redis database
void FileEventHandler::newFileDataObject(string fileName)
{
	Redis_Client RC;
	struct stat statObj;

	// Get file metadata
	string full_path = FILES_PATH + fileName;
    stat(full_path.c_str(), &statObj);

    // Put stat into metadata object
    FileData fd;
    fd.fileName = fileName;
    fd.localURI = full_path;
    fd.remoteURI = "";
    fd.fileSize = statObj.st_size;
    fd.lastModified = statObj.st_atime;
    fd.timesAccessed = 0;
    fd.isLocal = true;

    // Put info in db
    RC.Redis_HMSET(fd);
}