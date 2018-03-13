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
#include <string>


using namespace std;
std::atomic_bool ready;

// Process & thread related tasks
void daemonize();
void manageFiles(PolicyManager& pm, AWSConnector& aws, int time);
void updatePolicies(PolicyManager& pm, int time);

// Migration management functions
vector<FileData> findIntersection(vector<vector<FileData>> &fileLists, vector<FileData> &intersection, int &i);
vector<FileData> getDemotionList(PolicyManager& pm);
vector<FileData> getPromotionList(PolicyManager& pm);

// Bucket that we'll be using
const std::string BUCKET = "devon-bucket";


bool orderFiles(FileData& file1, FileData& file2)
{
    return (file1.localURI < file2.localURI);
}

void sortVector(vector<FileData>& files)
{
    sort (files.begin(), files.end(), orderFiles);
}



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
    
    ready = false;
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
    Redis_Client RC;
    while (true) {
        if (ready) {
            // Demote files
            syslog(LOG_NOTICE, "Querying database for demotion candidates.");
            vector<FileData> demotionList = getDemotionList(pm);
            syslog(LOG_NOTICE, "Demoting the following files:");
            for (FileData fd: demotionList) {
                syslog(LOG_NOTICE, fd.getLocalURI().c_str());
                if (fd.remoteURI == "") {
                    aws.demoteObject(BUCKET, fd.localURI, fd.getLocalURI());
                    RC.setRemoteURI(fd.fileName, "AWS/" + BUCKET + "/" + fd.fileName);
                    RC.setIsLocal(fd.fileName, false);
                }
                else  {
                    aws.demoteObject(BUCKET, fd.getRemoteURI(), fd.localURI);
                }
            }
            sleep(time);
        }
    }
}


vector<FileData> getDemotionList(PolicyManager& pm)
{
    Redis_Scanner RS;
    vector<FileData> demotionList;
    vector<FileData> intersection;
    int i = 0;

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
    
    // Find the intersection of the demotion policies
    vector<vector<FileData>> fileLists = {isLocal, inSizeRange, inTimeRange, inHitsRange};
    demotionList = findIntersection(fileLists, intersection, i);

    cout << "Files to migrate:\n";
    for (int i=0; i<demotionList.size(); i++)
    {
        cout << demotionList[i].fileName << endl;
    }

    return demotionList;
}


vector<FileData> getPromotionList(PolicyManager& pm)
{
    Redis_Scanner RS;
    vector<FileData> promotionList;
    vector<FileData> intersection;
    int i = 0;

    vector<FileData> inSizeRange;
    vector<FileData> inTimeRange;
    vector<FileData> inHitsRange;
    vector<FileData> isNonLocal;
   
    // TODO append instead of set
    for (auto p : pm.getPolicyList())  {
        // Grab and sort all files within file size policy range
        if (p->type.compare("sizepolicy") == 0) {
            SizePolicy* sp = (SizePolicy*) p;
            inSizeRange = RS.getFilesOutOfSizeRange(*sp);
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

    // Find the intersection of the promotion policies
    vector<vector<FileData>> fileLists = {isNonLocal, inSizeRange, inTimeRange, inHitsRange};
    promotionList = findIntersection(fileLists, intersection, i);

    return promotionList;
}



/* Return a vector of files that match all of the migration policies
*
* @param vector<vector<FileData>> &fileLists
*   A vector of vectors that contain files that match specified migration policies
*
* @param vector<FileData> &intersection
*   The current intersection of FileData objects from the fileLists vector
*
* @param int &i
*   A counter. Used to help go through fileLists recursively
*
* @return vector<FileData> intersection
*   Contains the FileData objects that were in all of the fileLists vectors
*/
vector<FileData> findIntersection(vector<vector<FileData>> &fileLists, vector<FileData> &intersection, int &i)
{
    Redis_Scanner RS;
    vector<FileData> temp;
    if(i==0)
    {
        set_intersection(fileLists[i].begin(), fileLists[i].end(), fileLists[i+1].begin(), fileLists[i+1].end(), back_inserter(temp), RS.orderFiles);
        RS.sortVector(temp);
        i = 2;
        intersection = findIntersection(fileLists, temp, i);
    }

    else
    {
        while(i < fileLists.size())
        {
            set_intersection(fileLists[i].begin(), fileLists[i].end(), intersection.begin(), intersection.end(), back_inserter(temp), RS.orderFiles);
            RS.sortVector(temp);
            i++;
            intersection = findIntersection(fileLists, temp, i);
        }
    }
    return intersection;
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
