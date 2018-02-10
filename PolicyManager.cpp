#include "lib/PolicyManager.hpp"
#include <stdio.h>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <string.h>

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

void PolicyManager::setMaxSaturation(int saturation)
{
    this->saturation = saturation;
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
	xmlChar *key;
	while (cur != NULL)
	{

		if(!xmlStrcmp(cur->name, (const xmlChar *)"name"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			printf("name: %s\n", key);
		}

		if(!xmlStrcmp(cur->name, (const xmlChar *)"lowerbound"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			printf("lower bound: %s\n", key);
		}

		if(!xmlStrcmp(cur->name, (const xmlChar *)"upperbound"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			printf("upper bound: %s\n", key);
		}
		cur = cur->next;
	}
}

void PolicyManager::parsePolicy (xmlDocPtr doc, xmlNodePtr cur)
{
	xmlChar *key;
	cur = cur->xmlChildrenNode;
	while(cur != NULL)
	{
		if(!xmlStrcmp(cur->name, (const xmlChar *)"type"))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			printf("policy found of type: %s\n", key);

			//there was a hit on finding the type of policy it is. Corresponding parsing function.
			if(!xmlStrcmp(key, (const xmlChar *)"sizepolicy"))
			{
				parseSizePolicy(doc, cur);
			}
			xmlFree(key);

			printf("\n");
		}
		cur = cur->next;
	}
}

static void PolicyManager::streamFile(const char *filename) {
	xmlDocPtr doc;
	xmlNodePtr cur;
	doc = xmlParseFile(filename);

	//if the document isn't parsed correctly	
	if (doc == NULL)
	{
		fprintf(stderr, "Document not successfully parsed.\n");
		return;
	}

	//if the document is empty
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL)
	{
		fprintf(stderr, "Empty document.\n");
		return;
	}
	
	//ensures xml file's root node is of type policylist
	if (xmlStrcmp(cur->name, (const xmlChar *) "policylist"))
	{
		fprintf(stderr, "Document not of type \"policylist\".");
		xmlFreeDoc(doc);
		return;
	}
	

	//parse through policylist to determine type of policy
	cur = cur->xmlChildrenNode; //gets child node of cur
	while(cur != NULL)
	{
		//if there is a match, calls funciton parsePolicy
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"policy")))
		{
			parsePolicy(doc, cur);
		}
	cur = cur->next;
	}

}

void parsePoliciesFromXMLFile() 
{
    char configFileName[] = "policies.xml";

  
    streamFile(configFileName);

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();

    return;
}


