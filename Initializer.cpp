//#include "lib/AWSConnector.hpp"
#include "lib/PolicyManager.hpp"
#include "lib/FileData.hpp"
#include "lib/FileUtils.hpp"
#include "lib/Constants.hpp"
#include "lib/FileEventHandler.hpp"
#include "Redis_Database/include/Redis_Client.hpp"


//#include <libxml/xmlreader.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

int main(int argc, char* argv[]) {
    // Initialize the database
    list<string> filenames = FileUtils::List(FILES_PATH.c_str());
    vector<int> fileDescriptors;
    Redis_Client RC;
    FileEventHandler FEH;
    struct stat statObj;

    for (string name: filenames) {
        // Get file metadata
        string full_path = FILES_PATH + name;
        stat(full_path.c_str(), &statObj);

        // Put stat into metadata object
        FileData fd;
        fd.fileName = name;
        fd.localURI = full_path;
        fd.remoteURI = "";
        fd.fileSize = statObj.st_size;
        fd.lastModified = statObj.st_atime;
        fd.timesAccessed = 0;
        fd.isLocal = true;

        // Put info in db
        RC.Redis_HMSET(fd);
        FEH.initializeINotify(fd);
    }

    FEH.test();
    return 0;
}
