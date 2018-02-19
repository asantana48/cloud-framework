#ifndef FILEDATA_H
#define FILEDATA_H
#include <string>
#include <sstream>
#include <ctime>


struct FileData
{
    std::string location;
    std::string fileName;
    int fileSize;
    int timesAccessed; // TODO add accessors
    time_t lastModified;
    bool isLocal;

    void setFileSize(std::string s) {
        fileSize = stoi(s);
    }

    void setLastModified(std::string s) {
        struct time_tm;
    }

    void setIsLocal(std::string s) {
        if (s.compare("true") == 0)
            isLocal = true;
        else if (s.compare("false") == 0)
            isLocal = false;
    }

	std::string getFileSize()
	{
        std::stringstream s;
		s << fileSize;
		return s.str();
	}

    std::string getLastModified()
	{
        std::stringstream s;
		s << lastModified;
		return s.str();
	}

    std::string getIsLocal() {
        std::stringstream s;
        s << isLocal;
        return s.str();
    }

    std::string getName() {
        std::stringstream s;
        s << location.substr(location.find_last_of("/")+1);
        return s.str();
    }

};
#endif
