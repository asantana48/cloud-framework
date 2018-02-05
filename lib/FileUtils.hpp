#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include <cstdint>
#include <string>
#include <sys/stat.h>
class FileUtils {
public:
    int Open(std::string resolvedPath, unsigned int oflags, int mode, int &handle) {

    }
    int Read(int handle, char* data, int bytesToRead, int &bytesRead) {

    }
    int Write(int handle, char* data, int bytesToWrite, int &bytesWritten) {

    }
    int Stat(std::string path, struct stat& buf) {

    }
    int Close(int ccurHandle, bool flush) {

    }
};
#endif