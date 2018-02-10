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

/*
 * processNode:
 * @reader: the xmlReader
 *
 * Dump information about the current node
 */
static void processNode(xmlTextReaderPtr reader) {
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
    name = BAD_CAST "--";

    value = xmlTextReaderConstValue(reader);

    printf("%d %d %s %d %d", 
        xmlTextReaderDepth(reader),
        xmlTextReaderNodeType(reader),
        name,
        xmlTextReaderIsEmptyElement(reader),
        xmlTextReaderHasValue(reader));
    if (value == NULL)
    printf("\n");
    else {
        if (xmlStrlen(value) > 40)
            printf(" %.40s...\n", value);
        else
        printf(" %s\n", value);
    }
}

/**
 * streamFile:
 * @filename: the file name to parse
 *
 * Parse and print information about an XML file.
 */
static void streamFile(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            processNode(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            fprintf(stderr, "%s : failed to parse\n", filename);
        }
    } else {
        fprintf(stderr, "Unable to open %s\n", filename);
    }
}

using namespace std;

int main(int argc, char* argv[]) {
    AWSConnector aws;
    aws.setBucket("devon-bucket");

    // Get file list and metadata
    char const* path = "/home/andres/Projects/cloud-framework/testfiles/";
    char full_name[255];
    vector<string> names = FileUtils::List(path);
    vector<FileData> files;
    struct stat statObj;
    for (auto it = names.begin(); it != names.end(); ++it) {
        strcpy(full_name, path);
        strcat(full_name, (*it).c_str());
        stat(full_name, &statObj);
        cout << full_name << endl;
        cout << statObj.st_size << endl;

        FileData fd;
        fd.location = full_name;
        fd.fileSize = statObj.st_size;
        files.push_back(fd);
    }
    
    // Set policies
    PolicyManager pm;
    int MB12 = 12*1024*1024;
    pm.setPolicy(new SizePolicy(MB12, false))

    return 0;
}
