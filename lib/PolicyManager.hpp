#ifndef POLICYMANAGER_H
#define POLICYMANAGER_H

#include "Policy.hpp"
#include "SizePolicy.hpp"
#include "TimePolicy.hpp"
#include "FileData.hpp"

#include <list>
#include <string>
#include <algorithm>

#include <stdio.h>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <string>

class PolicyManager {
private:
    std::list<Policy*> policyList;
	
    void parseSizePolicy (xmlDocPtr doc, xmlNodePtr cur)
    void parsePolicy (xmlDocPtr doc, xmlNodePtr cur)
    static void streamFile(const char *filename)
    
    
    // Out of 100
    int saturation;
public:
    PolicyManager() {}
    ~PolicyManager();

    void addPolicy(Policy* p);
    void setMaxSaturation(int saturation);
	bool isFilePromoted(FileData fd);
	std::list<FileData> getFileDemotionList(std::list<FileData> fileList); 
    void parsePoliciesFromXMLFile();

};
#endif
