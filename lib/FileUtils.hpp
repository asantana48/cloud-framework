#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include <unistd.h>   // replace with unistd for linux
#include <dirent.h>

class FileUtils {
public:
    static int Read(int handle, char* data, int bytesToRead, int &bytesRead) {
        bytesRead = 0;
        while (bytesRead < bytesToRead) {
            bytesRead += read(handle, data + bytesRead, bytesToRead - bytesRead);
            if (bytesRead = -1) {
                return bytesRead;
            }
        }
        return 0;
    }

    static int Write(int handle, char* data, int bytesToWrite, int &bytesWritten) {
        bytesWritten = 0;
        while (bytesWritten < bytesToWrite) {
            bytesWritten += write(handle, data + bytesWritten, bytesToWrite);
            if (bytesWritten = -1) {
                return bytesWritten;
            }
        }
        return 0;
    }
    static std::vector<std::string> List(char const* path) {
        std::vector<std::string> files;
        
        DIR *dp;
        struct dirent *dirp;
        if ((dp = opendir(path)) != NULL) {
            while ((dirp =readdir(dp)) != NULL) {
                if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, ".."))
                    files.push_back(std::string(dirp->d_name));
            }
        }

        return files;
    }
};
#endif