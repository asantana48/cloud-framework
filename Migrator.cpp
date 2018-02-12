//#include "lib/AWSConnector.hpp"
#include "lib/PolicyManager.hpp"
#include "lib/FileData.hpp"
#include "lib/FileUtils.hpp"

//#include <libxml/xmlreader.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

// Constants
static const string FILES_PATH = "/home/graves/cloud-framework/testbed/";        // directory
static const string POLICY_PATH = "/home/graves/cloud-framework/res/policies.xml"; // file

int main(int argc, char* argv[]) {
    // Get the policies from the file
    PolicyManager pm;
    pm.addPolicy(new SizePolicy(10*1024*1024, false));

    // Get file list and metadata
    list<string> filenames = FileUtils::List(FILES_PATH.c_str());
    list<FileData> files;
    struct stat statObj;
    for (string name: filenames) {
        string full_path = FILES_PATH + name;
        stat(full_path.c_str(), &statObj);

        // Put information in metadata object
        FileData fd;
        fd.location = full_path;
        cout << fd.getName() << endl;
        fd.fileSize = statObj.st_size;
        fd.lastModified = statObj.st_atime;
        files.push_back(fd);
    }

    /*AWSConnector aws;
    aws.connect("us-east-2");
    aws.setBucket("devon-bucket");

    list<FileData> migrate = pm.getFileDemotionList(files);
    for (FileData f: migrate) {
        aws.putObject(f.location, f.getName());
    }*/
    return 0;
}
