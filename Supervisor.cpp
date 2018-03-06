#include "lib/Constants.hpp"
#include "lib/AWSConnector.hpp"
#include "lib/PolicyManager.hpp"
#include "lib/FileData.hpp"
#include "lib/FileUtils.hpp"
#include "Redis_Database/include/Redis_Client.hpp"
#include "Redis_Database/include/Redis_Scanner.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <thread>
#include <atomic>


using namespace std;
std::atomic_bool ready;

// Process & thread related tasks
void daemonize();
void manageFiles(PolicyManager& pm, AWSConnector& aws, int time);
void updatePolicies(PolicyManager& pm, int time);

// Migration management functions
bool orderFiles(FileData&, FileData&);
void sortVector(vector<FileData>&);
vector<FileData> getDemotionList(PolicyManager& pm);
vector<FileData> getPromotionList(PolicyManager& pm);

int main(int argc, char** argv)
{
    // Intervals for various tasks
    int policyInterval = 8;
    int migrationInterval = 24;

    // Create necessary classes
    PolicyManager pm;    
    AWSConnector aws;

    // Initialize AWSConnector
    std::string region = "us-east-2";
    aws.connect(region);

    // Spawn daemon
    daemonize();

    syslog (LOG_NOTICE, "Started the migration supervisor.");
    
    std::thread policyT(updatePolicies, std::ref(pm), policyInterval);
    std::thread filesT(manageFiles, std::ref(pm), std::ref(aws), migrationInterval);
    
    while (true) {
        sleep(1);
    }

    syslog (LOG_NOTICE, "Supervisor terminated.");
    closelog();
    return EXIT_SUCCESS;
}

void updatePolicies(PolicyManager& pm, int time) {
    while (true) {
        ready = false;
        pm.clear();
        string resp = pm.parsePoliciesFromXMLFile(POLICIES_PATH);
        
        syslog(LOG_NOTICE, resp.c_str());
        ready = true; 
        sleep(time);
    }
}

void manageFiles(PolicyManager& pm, AWSConnector& aws, int time) {
    
    while (true) {
        if (ready) {
            // Demote files
            syslog(LOG_NOTICE, "Querying database for demotion candidates.");
            vector<FileData> demotionList = getDemotionList(pm);
            syslog(LOG_NOTICE, "Demoting the following files:");
            for (FileData fd: demotionList) {
                syslog(LOG_NOTICE, fd.getName().c_str());
                aws.demoteObject("devon-bucket", fd.location, fd.getName());
            }
 

            // Promote files
            syslog(LOG_NOTICE, "Querying database for promotion candidates.");
            vector<FileData> promotionList = getPromotionList(pm);
            syslog(LOG_NOTICE, "Promoting the following files:");
            for (FileData fd: promotionList) {
                syslog(LOG_NOTICE, fd.getName().c_str());
                aws.promoteObject("devon-bucket", fd.getName(), fd.location);
            }
            sleep(time);
        }
    }
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



vector<FileData> getDemotionList(PolicyManager& pm)
{
    Redis_Scanner RS;
    vector<FileData> demotionList;
    vector<FileData> temp1;
    vector<FileData> temp2;

    vector<FileData> inSizeRange;
    vector<FileData> inTimeRange;
    vector<FileData> inHitsRange;
    vector<FileData> isLocal;

    

    // TODO append instead of set
    for (auto p : pm.getPolicyList())  {
        // Grab and sort all files within file size policy range
        if (p->type.compare("sizepolicy") == 0) {
            SizePolicy* sp = (SizePolicy*) p;
            inSizeRange = RS.getFilesInSizeRange(*sp);
        }
        // Grab and sort all files within last modified time range
        else if (p->type.compare("timepolicy") == 0) {
            TimePolicy* tp = (TimePolicy*) p;
            inTimeRange = RS.getFilesInLastModifiedTime(*tp);
        }
        // Grab and sort all files within times accessed range
        else if (p->type.compare("hitspolicy") == 0) {
            HitPolicy* hp = (HitPolicy*) p;
            inHitsRange = RS.getFilesInTimesAccessedRange(*hp);
        }
    } 
    // Grab all local files
    isLocal = RS.getLocalFiles();

    sortVector(inSizeRange);
    sortVector(inTimeRange);
    sortVector(inHitsRange);
    sortVector(isLocal); 

    set_intersection(inSizeRange.begin(), inSizeRange.end(), inTimeRange.begin(), inTimeRange.end(), back_inserter(temp1), orderFiles);
    sortVector(temp1);
    
    set_intersection(inHitsRange.begin(), inHitsRange.end(), isLocal.begin(), isLocal.end(), back_inserter(temp2), orderFiles);
    sortVector(temp2);
    
    set_intersection(temp1.begin(), temp1.end(), temp2.begin(), temp2.end(), back_inserter(demotionList), orderFiles);
    sortVector(demotionList); 

    return demotionList;
}


vector<FileData> getPromotionList(PolicyManager& pm)
{
    Redis_Scanner RS;
    vector<FileData> promotionList;
    vector<FileData> temp1;
    vector<FileData> temp2;

    vector<FileData> inSizeRange;
    vector<FileData> inTimeRange;
    vector<FileData> inHitsRange;
    vector<FileData> isNonLocal;
   
    // TODO append instead of set
    for (auto p : pm.getPolicyList())  {
        // Grab and sort all files within file size policy range
        if (p->type.compare("sizepolicy") == 0) {
            SizePolicy* sp = (SizePolicy*) p;
            //inSizeRange = RS.getFilesOutOfSizeRange(*sp);
        }
        // Grab and sort all files within last modified time range
        else if (p->type.compare("timepolicy") == 0) {
            TimePolicy* tp = (TimePolicy*) p;
            inTimeRange = RS.getFilesOutOfLastModifiedTime(*tp);
        }
        // Grab and sort all files within times accessed range
        else if (p->type.compare("hitspolicy") == 0) {
            HitPolicy* hp = (HitPolicy*) p;
            inHitsRange = RS.getFilesOutOfTimesAccessedRange(*hp);
        }
    } 
    // Grab and sort all files that are not local
    isNonLocal = RS.getNonLocalFiles();

    sortVector(inSizeRange);
    sortVector(inHitsRange);
    sortVector(inTimeRange);
    sortVector(isNonLocal);
    
    set_intersection(inHitsRange.begin(), inHitsRange.end(), isNonLocal.begin(), isNonLocal.end(), back_inserter(temp1), orderFiles);
    sortVector(temp1);
    
    set_intersection(inSizeRange.begin(), inSizeRange.end(), inTimeRange.begin(), inTimeRange.end(), back_inserter(temp2), orderFiles);
    sortVector(temp2);  
    
    set_intersection(temp1.begin(), temp1.end(), temp2.begin(), temp2.end(), back_inserter(promotionList), orderFiles);
    sortVector(promotionList); 

    return promotionList;
}

void sortVector(vector<FileData>& files)
{
    sort (files.begin(), files.end(), orderFiles);
}

bool orderFiles(FileData& file1, FileData& file2)
{
    return (file1.location < file2.location);
}

