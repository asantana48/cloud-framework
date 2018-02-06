#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include <cstdint>
#include <cstdio>
#include <string>

#include <io.h>   // replace with unistd for linux
//#include <dirent.h>

class FileUtils {
public:
    int Read(int handle, char* data, int bytesToRead, int &bytesRead) {
        bytesRead = 0;
        while (bytesRead < bytesToRead) {
            bytesRead += read(handle, data + bytesRead, bytesToRead - bytesRead);
            if (bytesRead = -1) {
                return bytesRead;
            }
        }
        return 0;
    }

    int Write(int handle, char* data, int bytesToWrite, int &bytesWritten) {
        bytesWritten = 0;
        while (bytesWritten < bytesToWrite) {
            bytesWritten += write(handle, data + bytesWritten, bytesToWrite);
            if (bytesWritten = -1) {
                return bytesWritten;
            }
        }
        return 0;
    }

    /*int List(char* path, char* url, &pListDataOut,
        &listDataSize,
        attrFlag,
        partialMatchingFlag,
        addURLPrefix);*/
};
#endif