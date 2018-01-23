#include "lib/PolicyManager.h"

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
	auto it = policyList.begin();
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
