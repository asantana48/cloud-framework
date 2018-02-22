#ifndef POLICYMANAGER_H
#define POLICYMANAGER_H

#include "Policy.hpp"
#include "SizePolicy.hpp"
#include "TimePolicy.hpp"
#include "HitPolicy.hpp"
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
	
    void parseSizePolicy (xmlDocPtr doc, xmlNodePtr cur);
    void parsePolicy (xmlDocPtr doc, xmlNodePtr culr);
    void parseTimePolicy (xmlDocPtr doc, xmlNodePtr cur);
    void parseHitsPolicy (xmlDocPtr doc, xmlNodePtr cur);
    std::string streamFile(const char* filename);
    
public:
    PolicyManager() {}
    ~PolicyManager();

    void addPolicy(Policy* p);
	bool isFilePromoted(FileData fd);
	std::list<FileData> getFileDemotionList(std::list<FileData> fileList); 
    
    std::string parsePoliciesFromXMLFile(std::string filename);

    std::list<Policy*> getPolicyList();
};
#endif
