#include "lib/PolicyManager.hpp"
#include <stdio.h>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>

PolicyManager::~PolicyManager()
{
    auto it = policyList.begin();
    while (it != policyList.end()) {
        delete *it;
        it = policyList.erase(it);
    }
}

void PolicyManager::addPolicy(Policy* p)
{
    policyList.push_back(p);
}

bool PolicyManager::isFilePromoted(FileData fd)
{
    for (auto it = policyList.begin(); it != policyList.end(); ++it) {

    }

	return false;
}

std::list<FileData> PolicyManager::getFileDemotionList(std::list<FileData> fileList)
{
	std::list<FileData> migrationList;

	auto policyIt = policyList.begin();
	auto fileIt = fileList.begin();
	
	// Run through all policies for every file
	for (auto fileIt = fileList.begin(); fileIt != fileList.end(); ++fileIt) {
		bool needsMigrated = true;
		for (auto policyIt = policyList.begin(); policyIt != policyList.end(); ++policyIt) {
			if ((*policyIt)->isFileKept(*fileIt)) {
				needsMigrated = false;
				break;
			}
		}
		if (needsMigrated)
			migrationList.push_back(*fileIt);
	}

	return migrationList;
}

void PolicyManager::parseSizePolicy (xmlDocPtr doc, xmlNodePtr cur)
{
	Policy* sizePolicy;
	int lower;
	int upper;
	std::string name;

	xmlChar *key;
	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{

		if(!xmlStrcmp(cur->name, (const xmlChar *)"name"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			name = (char*) key;
		}

		if(!xmlStrcmp(cur->name, (const xmlChar *)"lowerbound"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			lower = std::stoi((char*) key);
		}

		if(!xmlStrcmp(cur->name, (const xmlChar *)"upperbound"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			upper = std::stoi((char*) key);
		}
		cur = cur->next;
	}
	sizePolicy = new SizePolicy(lower, upper);
	sizePolicy->name = name;
	sizePolicy->type = "sizepolicy";
	addPolicy(sizePolicy);
}

void PolicyManager::parseHitsPolicy (xmlDocPtr doc, xmlNodePtr cur)
{
	Policy* hitPolicy;
	int min;
	std::string name;

	xmlChar *key;
	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{

		if(!xmlStrcmp(cur->name, (const xmlChar *)"name"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			name = (char*) key;
		}

		if(!xmlStrcmp(cur->name, (const xmlChar *)"minimumhits"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			min = std::stoi((char*) key);
		}
		cur = cur->next;
	}
	hitPolicy = new HitPolicy(min);
	hitPolicy->name = name;
	hitPolicy->type = "hitspolicy";
	addPolicy(hitPolicy);
}

void PolicyManager::parseTimePolicy (xmlDocPtr doc, xmlNodePtr cur)
{
	Policy* timePolicy;
	int totalTimeInSeconds = 0;
	std::string name;

	xmlChar *key;
	cur = cur->xmlChildrenNode;
	

	while (cur != NULL)
	{
		if(!xmlStrcmp(cur->name, (const xmlChar *)"name"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			name = (char*) key;
		}

		if(!xmlStrcmp(cur->name, (const xmlChar *)"years"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			int temp = std::stoi((char *)key);
			totalTimeInSeconds = totalTimeInSeconds + (temp * 365 * 24 * 60 * 60);
		}
		if(!xmlStrcmp(cur->name, (const xmlChar *)"months"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			int temp = std::stoi((char *)key);
			totalTimeInSeconds = totalTimeInSeconds + (temp * 30 * 24 * 60 * 60);
		}
		if(!xmlStrcmp(cur->name, (const xmlChar *)"days"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			int temp = std::stoi((char *)key);
			totalTimeInSeconds = totalTimeInSeconds + (temp * 24 * 60 * 60);
		}
		if(!xmlStrcmp(cur->name, (const xmlChar *)"hours"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			int temp = std::stoi((char *)key);
			totalTimeInSeconds = totalTimeInSeconds + (temp * 60 * 60);
		}
		if(!xmlStrcmp(cur->name, (const xmlChar *)"minutes"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			int temp = std::stoi((char *)key);
			totalTimeInSeconds = totalTimeInSeconds + (temp * 60);
		}
		if(!xmlStrcmp(cur->name, (const xmlChar *)"seconds"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			int temp = std::stoi((char *)key);
			totalTimeInSeconds = totalTimeInSeconds + temp;
		}

		cur = cur->next;
	}

	timePolicy = new TimePolicy(totalTimeInSeconds);
	timePolicy->name = name;
	timePolicy->type = "timepolicy";
	addPolicy(timePolicy);
}

void PolicyManager::parsePolicy (xmlDocPtr doc, xmlNodePtr cur)
{
	xmlChar *key;
	xmlChar *policyType;

	//determine what type of policy we are parsing.
	policyType = xmlGetProp(cur, (const xmlChar *)"type");
	if (!xmlStrcmp(policyType, (const xmlChar *)"sizepolicy"))
	{
		parseSizePolicy(doc, cur);
	}
	else if (!xmlStrcmp(policyType, (const xmlChar *)"hitspolicy"))
	{
		parseHitsPolicy(doc, cur);
	}
	else if (!xmlStrcmp(policyType, (const xmlChar *)"timepolicy"))
	{
		parseTimePolicy(doc, cur);
	}
}

std::string PolicyManager::streamFile(const char* filename) {
	xmlDocPtr doc;
	xmlNodePtr cur;
	doc = xmlParseFile(filename);

	// Make sure document actually read
	if (doc == NULL)   
		return "Document not successfully parsed.";

	
	cur = xmlDocGetRootElement(doc);

	// Make sure document isn't empty
	if (cur == NULL)
		return "Empty document.";
	
	//  Make sure document is of type policylist
	if (xmlStrcmp(cur->name, (const xmlChar *) "policylist"))
	{
		xmlFreeDoc(doc);
		return "Document not of type <policylist>.";
	}
	

	// parse through policylist to determine type of policy
	cur = cur->xmlChildrenNode; //gets child node of cur
	while(cur != NULL)
	{
		//if there is a match, parse the policy
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"policy")))
			parsePolicy(doc, cur);

		cur = cur->next;
	}
	return "Successfully parsed the policies.";
}

std::string PolicyManager::parsePoliciesFromXMLFile(std::string configFileName) 
{  
    std::string msg = streamFile(configFileName.c_str());

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();

    return msg;
}

std::list<Policy*> PolicyManager::getPolicyList() {
	return policyList;
}

