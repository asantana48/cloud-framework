#include "lib/AWSConnector.hpp"
#include "lib/PolicyManager.hpp"
#include "lib/FileData.hpp"
#include "lib/FileUtils.hpp"

#include <libxml/xmlreader.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

// Constants
static const string FILES_PATH = "/home/andres/Projects/cloud-framework/testfiles/";        // directory
static const string POLICY_PATH = "/home/andres/Projects/cloud-framework/res/policies.xml"; // file

int main(int argc, char* argv[]) {
    AWSConnector aws;
    aws.setBucket("devon-bucket");

    // Get file list and metadata
    vector<string> names = FileUtils::List(FILES_PATH.c_str());
    vector<FileData> files;
    struct stat statObj;
    for (auto it = names.begin(); it != names.end(); ++it) {
        string full_path = FILES_PATH + (*it);
        stat(full_path.c_str(), &statObj);

        // Put information in metadata object
        FileData fd;
        fd.location = full_path;
        fd.fileSize = statObj.st_size;
        fd.lastModified = statObj.st_atime;
        files.push_back(fd);
    }
    
    for (FileData i: files) {
        cout << i.location << " (" << i.fileSize << ")" << i.lastModified << "\n";
    }
    // Set policies
    PolicyManager pm;
    int MB12 = 12*1024*1024;
    int atime = time(NULL) - 40000;
    pm.addPolicy(new SizePolicy(MB12, false));
    pm.addPolicy(new TimePolicy(atime));

    return 0;
}
