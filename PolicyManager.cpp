#include "lib/PolicyManager.hpp"
#include <stdio.h>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>

PolicyManager::~PolicyManager()
{
	this->clear();
}

void PolicyManager::clear() {
	auto outer = policyList.begin();
    while (outer != policyList.end()) {
        auto inner = outer->begin();
		while (inner != outer->end()) {
			delete *inner;
			inner = outer->erase(inner);
		}
		outer++;
    }
	policyList.clear();
}

Policy* PolicyManager::parseSizePolicy (xmlDocPtr doc, xmlNodePtr cur)
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
	return sizePolicy;
}

Policy* PolicyManager::parseHitsPolicy (xmlDocPtr doc, xmlNodePtr cur)
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
	return hitPolicy;
}

Policy* PolicyManager::parseTimePolicy (xmlDocPtr doc, xmlNodePtr cur)
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
	return timePolicy;
}

Policy* PolicyManager::parsePolicy (xmlDocPtr doc, xmlNodePtr cur)
{
	Policy* policy;
	xmlChar *key;
	xmlChar *policyType;

	//determine what type of policy we are parsing.
	policyType = xmlGetProp(cur, (const xmlChar *)"type");
	if (!xmlStrcmp(policyType, (const xmlChar *)"sizepolicy"))
	{
		policy = parseSizePolicy(doc, cur);
	}
	else if (!xmlStrcmp(policyType, (const xmlChar *)"hitspolicy"))
	{
		policy = parseHitsPolicy(doc, cur);
	}
	else if (!xmlStrcmp(policyType, (const xmlChar *)"timepolicy"))
	{
		policy = parseTimePolicy(doc, cur);
	}
	else 
		return NULL;

	return policy;
}

bool PolicyManager::streamFile(const char* filename) {
	xmlDocPtr doc;
	xmlNodePtr cur, validator;
	doc = xmlParseFile(filename);

	//if the document isn't parsed correctly	
	if (doc == NULL)
		return false;		

	//if the document is empty
	validator = xmlDocGetRootElement(doc);

	// Make sure the document is valid
	if (validator == NULL)
		return false;
	else if (xmlStrcmp(validator->name, (const xmlChar *) "policyfile"))
		return false;
	else {
		int children = xmlChildElementCount(validator);
		if (children == 0)
			return false;
	}
		
	
	
	// Set the current marker where it needs to be
	cur = xmlDocGetRootElement(doc);
	cur = cur->xmlChildrenNode;
	while(cur != NULL)
	{
		//if there is a match, calls function parsePolicyList
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"policylist")))
		{
			parsePolicyList(doc, cur);
		}

		cur = cur->next;
	}
	return true;
}

void PolicyManager::parsePolicyList (xmlDocPtr doc, xmlNodePtr cur)
{
	std::list<Policy*> newList;
	//parse through policylist to determine type of policy
	cur = cur->xmlChildrenNode; //gets child node of cur
	while(cur != NULL)
	{
		//if there is a match, calls function parsePolicy
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"policy")))
		{
			Policy* newPolicy = parsePolicy(doc, cur);
			if (newPolicy != NULL)
				newList.push_back(newPolicy);
		}
		cur = cur->next;
	}

	if (newList.size() > 0)
		policyList.push_back(newList);
}

bool PolicyManager::parsePoliciesFromXMLFile(std::string configFileName) 
{  
    bool success = streamFile(configFileName.c_str());

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();

    return success;
}

std::list<std::list<Policy*>> PolicyManager::getPolicyList() {
	return policyList;
}

