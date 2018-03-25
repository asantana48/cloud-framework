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

    for (string name: filenames) {
        FEH.newFileDataObject(name);
    }

    // Start the event handler on the testbed directory
    FEH.initializeINotify();
    cout << "PARENT PROCESS RETURNED\n";
    return 0;
}
