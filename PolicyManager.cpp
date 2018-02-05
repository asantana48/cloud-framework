#include "lib/PolicyManager.hpp"

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
