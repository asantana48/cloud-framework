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
		}

		if(!xmlStrcmp(cur->name, (const xmlChar *)"lowerbound"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		}

		if(!xmlStrcmp(cur->name, (const xmlChar *)"upperbound"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
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
		}

		if(!xmlStrcmp(cur->name, (const xmlChar *)"minimumhits"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
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
	return policy;
}

std::string PolicyManager::streamFile(const char* filename) {
	xmlDocPtr doc;
	xmlNodePtr cur;
	doc = xmlParseFile(filename);

	//if the document isn't parsed correctly	
	if (doc == NULL)
	{
		return "Document not successfully parsed.\n";
		
	}

	//if the document is empty
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL)
	{
		return "Empty document.\n";
	}
	
	//ensures xml file's root node is of type policyfile
	if (xmlStrcmp(cur->name, (const xmlChar *) "policyfile"))
	{
		return "Document not of type \"policyfile\".";
		xmlFreeDoc(doc);
		
	}
	

	//parse through policylist to determine type of policy
	cur = cur->xmlChildrenNode; //gets child node of cur
	while(cur != NULL)
	{
		//if there is a match, calls function parsePolicyList
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"policylist")))
		{
			parsePolicyList(doc, cur);
		}
		cur = cur->next;
	}
	return "Successfully parsed the policies.";
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
			newList.push_back(newPolicy);
		}
		cur = cur->next;
	}

	policyList.push_back(newList);
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

std::list<std::list<Policy*>> PolicyManager::getPolicyList() {
	return policyList;
}

