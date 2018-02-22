#include "lib/Constants.hpp"
#include "lib/AWSConnector.hpp"
#include "lib/PolicyManager.hpp"
#include "lib/FileData.hpp"
#include "lib/FileUtils.hpp"
#include "Redis_Database/include/Redis_Client.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

using namespace std;

void daemonize();

int main(int argc, char** argv)
{
    int policyInterval = 10;
    int migrationInterval = 60;

    daemonize();
    syslog (LOG_NOTICE, "Started the migration supervisor.");
    while (true)
    {
        //TODO: Insert daemon code here.
        sleep (20);
        break;
    }

    syslog (LOG_NOTICE, "Supervisor terminated.");
    closelog();
    return EXIT_SUCCESS;
}

void daemonize()
{
    pid_t pid;

    // Fork off the parent process 
    pid = fork();

    // Have parent end itself 
    if (pid != 0) 
    {
        if (pid < 0)
            exit(EXIT_FAILURE);
        else if (pid > 0)
            exit(EXIT_SUCCESS);
    }

    // Set child as session leader
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    // Set signals to ignore
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    // Fork off again to finalize the daemon process
    pid = fork();

    // Have parent end itself 
    if (pid != 0) 
    {
        if (pid < 0)
            exit(EXIT_FAILURE);
        if (pid > 0)
            exit(EXIT_SUCCESS);
    }

    // Set permission of newly created/migrated files to default
    umask(022);

    // Change directory to testbed directory 
    chdir(FILES_PATH.c_str());

    // Close all open file descriptors
    for (int x = sysconf(_SC_OPEN_MAX); x>=0; x--)
        close (x);

    // Open a log file
    openlog ("supervisor", LOG_PID, LOG_DAEMON);
}
/*
int main(int argc, char* argv[]) {
    // Get the policies from the file
    PolicyManager pm;
    pm.addPolicy(new SizePolicy(10*1024*1024, false));

    // Get file list and metadata
    list<string> filenames = FileUtils::List(FILES_PATH.c_str());
    list<FileData> files;
    Redis_Client RC;
    struct stat statObj;
    for (string name: filenames) {
        string full_path = FILES_PATH + name;
        stat(full_path.c_str(), &statObj);

        // Put information in metadata object
        FileData fd;
        fd.fileName = name;
        fd.location = full_path;
        cout << fd.getName() << endl;
        fd.fileSize = statObj.st_size;
        fd.lastModified = statObj.st_atime;
        files.push_back(fd);
        RC.Redis_HMSET(fd);
    }

    /*AWSConnector aws;
    aws.connect("us-east-2");
    aws.setBucket("devon-bucket");

    list<FileData> migrate = pm.getFileDemotionList(files);
    for (FileData f: migrate) {
        aws.putObject(f.location, f.getName());
    }
    return 0;
}*/
