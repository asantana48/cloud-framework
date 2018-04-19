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
#include <sstream>
#include <fstream>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;
std::atomic_bool ready;

// Process & thread related tasks
void daemonize();
void manageFiles(PolicyManager& pm, AWSConnector& aws, int time);
void updatePolicies(PolicyManager& pm, int time);


// Migration management functions
vector<FileData> findIntersection(vector<vector<FileData>> &fileLists, vector<FileData> &intersection, int &i);
vector<FileData> findUnion(vector<vector<FileData>> &fileLists);
vector<FileData> getDemotionList(list<Policy*> policyCriteria);
vector<FileData> getPromotionList(list<Policy*> policyCriteria);


int main(int argc, char** argv)
{
    
    // Intervals for various tasks
    int policyInterval = 10;
    int migrationInterval = 30;

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
    /*FileData f1, f2, f3, f4;
    f1.fileName = "A";
    f2.fileName = "B";
    f3.fileName = "B";
    f4.fileName = "C";

    vector<FileData> one;
    vector<FileData> two;
    vector<vector<FileData>> comb;
    one.push_back(f1);
    one.push_back(f2);
    two.push_back(f3);
    two.push_back(f4);
    comb.push_back(one);
    comb.push_back(two);

    vector<FileData> tres;
    tres = findUnion(comb);
    for (FileData fd: tres)
        cout << fd.fileName << "\n";*/
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

        for(std::list<Policy*> policyList: pm.getPolicyList()){
            for (Policy* p: policyList) {
                std::string name = p->name;
                syslog(LOG_NOTICE, name.c_str());
            }
        }
        ready = true; 
        sleep(time);
    }
}

void manageFiles(PolicyManager& pm, AWSConnector& aws, int migrateTime) {
    Redis_Client RC;
    Redis_Scanner RS;
    FileData tempFD;
    int i = 0;
    vector<FileData> intersection;
    vector<FileData> demotionList;
    vector<FileData> promotionList;
    vector<vector<FileData>> promotionLists;
    vector<vector<FileData>> demotionLists;
    while (true) {
        if (ready) {
            // Demote files
            syslog(LOG_NOTICE, "Querying database for demotion candidates.");
            for (auto p : pm.getPolicyList())
            {
                demotionLists.push_back(getDemotionList(p));
            }

            if (demotionLists.size() > 1)
                demotionList = findIntersection(demotionLists, intersection, i);
            else
                demotionList = demotionLists[0];

            syslog(LOG_NOTICE, "----------DEMOTION START----------");
            for (FileData fd: demotionList) {

                syslog(LOG_NOTICE, "Demoting file: ");
                syslog(LOG_NOTICE, fd.fileName.c_str());

                if(RC.getIsOpen(fd.localURI))
                {
                    syslog(LOG_NOTICE, "FILE IS CURRENTLY OPEN! SKIPPING DEMOTION");
                }

                // Do not bounce back if under 5 minutes old
                else if (time(NULL) - fd.lastModified > 30) {

                    if (fd.remoteURI == "") {
                        RC.setRemoteURI(fd.localURI, fd.fileName);   
                        fd.remoteURI = fd.fileName; 
                    }
                    
                    tempFD = fd;

                    // Demotion
                    aws.demoteObject(BUCKET, fd.localURI, fd.remoteURI);
                    syslog(LOG_NOTICE, "File demoted:");
                    syslog(LOG_NOTICE, fd.fileName.c_str());

                    this_thread::sleep_for (chrono::milliseconds(1000));
                    syslog(LOG_NOTICE, "File demoted now!");

                    RC.Redis_HMSET(tempFD);




                    syslog(LOG_NOTICE, "Database updated"); 
                    this_thread::sleep_for (chrono::milliseconds(1000)); 
                          
                    RC.setIsMetadata(tempFD.localURI, true);
                    syslog(LOG_NOTICE, "Metadata flag set to true");
                    this_thread::sleep_for (chrono::milliseconds(1000));

                    // Create a copy of the demoted file
                    ofstream newFile(tempFD.localURI);

                    // newFile << fd.fileName << " is being retrieved from long term storage. Please wait for updated thumbnail...";

                    newFile.close();

                    syslog(LOG_NOTICE, "new file closed!");
                    this_thread::sleep_for (chrono::milliseconds(2000));

                    RC.setIsLocal(tempFD.localURI, false); 


                    syslog(LOG_NOTICE, "local flag set to false");

                    // this_thread::sleep_for (chrono::seconds(5));
                    this_thread::sleep_for (chrono::milliseconds(500));
                }
                
                else
                {
                    syslog(LOG_NOTICE, "Bounceback prevented on %s.", fd.fileName.c_str());
                }
            }
            
            demotionList.clear();
            intersection.clear();
            syslog(LOG_NOTICE, "----------DEMOTION END----------");
            this_thread::sleep_for (chrono::seconds(5));

            syslog(LOG_NOTICE, "waiting...");
            this_thread::sleep_for (chrono::seconds(5));
            // Promote files
            syslog(LOG_NOTICE, "----------PROMOTION START----------");
            syslog(LOG_NOTICE, "Querying database for promotion candidates.");
            for (auto p : pm.getPolicyList())
            {
                vector<FileData> promotionList = getPromotionList(p);
                for (FileData fd: promotionList) {
                    syslog(LOG_NOTICE, "Promoting file: ");
                    syslog(LOG_NOTICE, fd.fileName.c_str());

                    // Promotion
                    aws.promoteObject(BUCKET, fd.remoteURI, fd.localURI);
                    syslog(LOG_NOTICE, "File promoted:");
                    syslog(LOG_NOTICE, fd.fileName.c_str());

                    // Update the database
                    RC.setRemoteURI(fd.localURI, "");
                    
                    RC.setIsLocal(fd.localURI, true);
                    RC.setIsMetadata(fd.localURI, false);
                }
                promotionList.clear();
            }

            syslog(LOG_NOTICE, "----------PROMOTION END----------");
            sleep(migrateTime);
        }
    }
}


vector<FileData> getDemotionList(list<Policy*> policyCriteria)
{
    Redis_Scanner RS;
    vector<FileData> demotionList;
    vector<FileData> intersection;
    int i = 0;

    vector<FileData> inSizeRange;
    vector<FileData> inTimeRange;
    vector<FileData> inHitsRange;
    vector<FileData> isLocal;
    vector<vector<FileData>> fileLists;

    

    // TODO append instead of set
    for (auto p : policyCriteria)  {
        // Grab and sort all files within file size policy range
        if (p->type.compare("sizepolicy") == 0) {
            SizePolicy* sp = (SizePolicy*) p;
            inSizeRange = RS.getFilesOutOfSizeRange(*sp);
            fileLists.push_back(inSizeRange);
        }
        // Grab and sort all files within last modified time range
        else if (p->type.compare("timepolicy") == 0) {
            TimePolicy* tp = (TimePolicy*) p;
            inTimeRange = RS.getFilesInLastModifiedTime(*tp);
            fileLists.push_back(inTimeRange);
        }
        // Grab and sort all files within times accessed range
        else if (p->type.compare("hitspolicy") == 0) {
            HitPolicy* hp = (HitPolicy*) p;
            inHitsRange = RS.getFilesInTimesAccessedRange(*hp);
            fileLists.push_back(inHitsRange);
        }
    } 
    // Grab all local files
    isLocal = RS.getLocalFiles();

    for (FileData fd: isLocal) {
        string local = "Local file: " + fd.fileName;
        syslog(LOG_NOTICE, local.c_str());
    }
    
    for (FileData fd: inSizeRange) {
        string local = "Size file: " + fd.fileName;
        syslog(LOG_NOTICE, local.c_str());
    }
    for (FileData fd: inTimeRange) {
        string local = "Time file: " + fd.fileName;
        syslog(LOG_NOTICE, local.c_str());
    }
    for (FileData fd: inHitsRange) {
        string local = "Hits file: " + fd.fileName;
        syslog(LOG_NOTICE, local.c_str());
    }

 
    demotionList = findUnion(fileLists);

    fileLists.clear();
    fileLists.push_back(demotionList);
    fileLists.push_back(isLocal);

    demotionList = findIntersection(fileLists, intersection, i);



    // Print files to demote
    syslog(LOG_NOTICE, "Demotion Intersection files: ");
    for (int i=0; i<demotionList.size(); i++)
    {
        syslog(LOG_NOTICE, demotionList[i].fileName.c_str());
    }
    

    return demotionList;
}


vector<FileData> getPromotionList(list<Policy*> policyCriteria)
{
    Redis_Scanner RS;
    vector<FileData> promotionList;
    vector<FileData> intersection;
    int i = 0;

    vector<FileData> inSizeRange;
    vector<FileData> inTimeRange;
    vector<FileData> inHitsRange;
    vector<FileData> isNonLocal;
    vector<vector<FileData>> fileLists;

    // TODO append instead of set
    for (auto p : policyCriteria)  {
        // Grab and sort all files within file size policy range
        if (p->type.compare("sizepolicy") == 0) {
            SizePolicy* sp = (SizePolicy*) p;
            inSizeRange = RS.getFilesInSizeRange(*sp);
            fileLists.push_back(inSizeRange);
        }
        // Grab and sort all files within last modified time range
        else if (p->type.compare("timepolicy") == 0) {
            TimePolicy* tp = (TimePolicy*) p;
            inTimeRange = RS.getFilesInLastModifiedTime(*tp);
            fileLists.push_back(inTimeRange);
        }
        // Grab and sort all files within times accessed range
        else if (p->type.compare("hitspolicy") == 0) {
            HitPolicy* hp = (HitPolicy*) p;
            inHitsRange = RS.getFilesOutOfTimesAccessedRange(*hp);
            fileLists.push_back(inHitsRange);
        }
    } 
    // Grab and sort all files that are not local
    isNonLocal = RS.getNonLocalFiles();

    fileLists.push_back(isNonLocal);

    
    for (FileData fd: inSizeRange) {
        string local = "Size file: " + fd.fileName;
        syslog(LOG_NOTICE, local.c_str());
    }
    for (FileData fd: inTimeRange) {
        string local = "Time file: " + fd.fileName;
        syslog(LOG_NOTICE, local.c_str());
    }
    for (FileData fd: inHitsRange) {
        string local = "Hits file: " + fd.fileName;
        syslog(LOG_NOTICE, local.c_str());
    }
    for (FileData fd: isNonLocal) {
        string local = "Remote file: " + fd.fileName;
        syslog(LOG_NOTICE, local.c_str());
    }

    // Find the intersection of the promotion policies
    promotionList = findIntersection(fileLists, intersection, i);

    syslog(LOG_NOTICE, "Promotion Intersection files: ");
    // Print files to promote
    for (int i=0; i<promotionList.size(); i++)
    {
        syslog(LOG_NOTICE, promotionList[i].fileName.c_str());
    }
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

vector<FileData> findUnion(vector<vector<FileData>> &fileList)
{
    vector<FileData> unionVec;
        
    for (vector<FileData> fl: fileList) 
    {
        unionVec.insert(unionVec.end(), fl.begin(), fl.end());
    }
    //set<int> s( unionVec.begin(), unionVec.end() );
    //unionVec.assign( s.begin(), s.end() );

    sort( unionVec.begin(), unionVec.end() );
    unionVec.erase( unique( unionVec.begin(), unionVec.end() ), unionVec.end() );
    return unionVec;
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
