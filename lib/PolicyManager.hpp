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

    bool parseError;
    std::list<std::list<Policy*>> policyList;
	
    Policy* parseSizePolicy (xmlDocPtr doc, xmlNodePtr cur);
    Policy* parsePolicy (xmlDocPtr doc, xmlNodePtr culr);
    Policy* parseTimePolicy (xmlDocPtr doc, xmlNodePtr cur);
    Policy* parseHitsPolicy (xmlDocPtr doc, xmlNodePtr cur);
    void parsePolicyList(xmlDocPtr doc, xmlNodePtr cur);
    bool streamFile(const char* filename);
    
public:
    PolicyManager() {}
    ~PolicyManager();
    
    void clear();
    
    bool parsePoliciesFromXMLFile(std::string filename);

    std::list<std::list<Policy*>> getPolicyList();
};
#endif