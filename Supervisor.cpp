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
bool orderFiles(FileData&, FileData&);
void sortVector(vector<FileData>&);

int main(int argc, char** argv)
{
    // Intervals for various tasks
    int policyInterval = 10;
    int migrationInterval = 60;

    // Create necessary classes
    PolicyManager pm;

    // Spawn daemon
    daemonize();

    syslog (LOG_NOTICE, "Started the migration supervisor.");
    while (true)
    {        
        syslog(LOG_NOTICE, "test");
        string resp = pm.parsePoliciesFromXMLFile(POLICIES_PATH);
        syslog(LOG_NOTICE, resp.c_str());
        list<Policy*> policyList = pm.getPolicyList();
        for (auto policyIt = policyList.begin(); policyIt != policyList.end(); ++policyIt) {
            syslog(LOG_NOTICE, (*policyIt)->type.c_str());
        }
        sleep (2);
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
vector<FileData> getDemotionList(PolicyList)
{
    Redis_Scanner RS;
    vector<FileData> demotionList;
    vector<FileData> temp1;
    vector<FileData> temp2;

     // Grab and sort all files within file size policy range
    vector<FileData> inSizeRange = RS.getFilesInSizeRange(Size Policy);
    sortVector(inSizeRane);

     // Grab and sort all files within last modified time range
    vector<FileData> inTimeRange = RS.getFilesInLastModifiedTime(TimePolicy);
    sortVector(inTimeRange);

     // Grab and sort all files within times accessed range
    vector<FileData> inHitsRange = RS.getFilesInTimesAccessedRange(HitPolicy);
    sortVector(inHitsRange);

     // Grab and sort all files that are local
    vector<FileData> isLocal = RS.getLocalFiles();
    sortVector(isLocal);

    set_intersection(inSizeRange.begin(), inSizeRange.end(), inTimeRange.begin(), inTimeRange.end(), back_inserter(temp1), orderFiles);
    sortVector(temp1);
    
    set_intersection(inHitsRange.begin(), inHitsRange.end(), isLocal.begin(), isLocal.end(), back_inserter(temp2), orderFiles);
    sortVector(temp2);
    
    set_intersection(temp1.begin(), temp1.end(), temp2.begin(), temp2.end(), back_inserter(demotionList), orderFiles);
    sortVector(demotionList);    
    
    cout << "Files to demote:\n";
    for (int i=0; i<demotionList.size(); i++)
    {
        cout << demotionList[i].fileName << endl;
    }
     return demotionList;
}


vector<FileData> getPromotionList(PolicyList)
{
    Redis_Scanner RS;
    vector<FileData> promotionList;
    vector<FileData> temp1;
    vector<FileData> temp2;

    // Grab and sort all files outside of last modified time range
    vector<FileData> inTimeRange = RS.getFilesInLastModifiedTime(TimePolicy);
    sortVector(inTimeRange);
    
    // Grab and sort all files outside of times accessed range
    vector<FileData> inHitsRange = RS.getFilesInTimesAccessedRange(HitPolicy);
    sortVector(inHitsRange);
    
    // Grab and sort all files that are not local
    vector<FileData> isLocal = RS.getLocalFiles();
    sortVector(isLocal);
    
    set_intersection(inHitsRange.begin(), inHitsRange.end(), isLocal.begin(), isLocal.end(), back_inserter(temp1), orderFiles);
    sortVector(temp1);
    
    set_intersection(temp1.begin(), temp1.end(), inTimeRange.begin(), inTimeRange.end(), back_inserter(promotionList), orderFiles);
    sortVector(promotionList);  
    
    cout << "Files to promote:\n";
    for (int i=0; i<promotionList.size(); i++)
    {
        cout << promotionList[i].fileName << endl;
    }
     return promotionList;
}
*/


void sortVector(vector<FileData>& files)
{
    sort (files.begin(), files.end(), orderFiles);
}

bool orderFiles(FileData& file1, FileData& file2)
{
    return (file1.location < file2.location);
}

