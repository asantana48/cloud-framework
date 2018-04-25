#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP


/* CCURrados_test.cpp */
/*
 *
 * Copyright, 2014 - 2016, Concurrent Computer Corporation,
 *    as an unpublished work. This document contains confidential and
 *    proprietary information, including trade secrets, of Concurrent
 *    Computer Corporation. Any use, reproduction or transmission of any part
 *    or the whole of this document is expressly prohibited without the
 *    prior written permission of Concurrent Computer Corporation.
 *
 */

/** @file
 *
 *    CCURRADOS Client Library API tester.  This file contains functions
 *    for testing and exercising various CCURRADOS and BusinessLogicInterface
 *    functions.
 *
 */

#include <cstdint>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <list>

// Uncomment to output debug trace messages to a log file instead of syslog
#define OUTPUT_LOG_FILE  1

#define GetTimeCountUS   PII_GetTimeCountUS

#include <File.h>
#include <DataBuffer.h>

#include <CCURrados.h>
//#include <CCURrados/CCURrados_Configuration.h>
#include <DebugTraceInterface.h>
#include <PlatformIndependentInterface.h>


/*----------------------------------------------------------------------*/
/* Test methods */
/*----------------------------------------------------------------------*/

#define M_UNKNOWN                    0
#define M_GET_OBJECT                 1
#define M_PUT_OBJECT                 2
#define M_REMOVE_OBJECT              3
#define M_STAT_OBJECT                4
#define M_LIST_OBJECTS               5
#define M_ECT            6
#define M_TEST_PURGE_TRUNCATE        7
#define M_TEST_MULTI_READ_THREAD     8
#define M_TEST_MULTI_WRITE_THREAD    9
#define M_GET_CLUSTER_CAPACITY_REMAINING    10
#define M_TEST_MULTI_READ_WRITE_THREAD    11
#define M_STAT_POOL                  12
#define M_GET_CLUSTER_USAGE_STATUS   13
#define M_FIND_DELETED_POOL          14
#define M_QUEUE_FOR_ASYNC_DELETE     15
#define M_FSTAT_OBJECT               16
#define M_GET_CONTINUOUS             17
#define M_PUT_CONTINUOUS             18
#define M_IOSTATS                    19
#define M_IOSTATS_RESET              20

/*----------------------------------------------------------------------*/
/* constants */
/*----------------------------------------------------------------------*/

#define CCUR_MAXIMUM_ITERATION_COUNT   10000
#define CCUR_MAXIMUM_THREAD_COUNT      10000
#define CCUR_MAXIMUM_FILE_COUNT        10000

/*----------------------------------------------------------------------*/
/* Global variables */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global utility functions */
/*----------------------------------------------------------------------*/


class FileUtils {
private:
    // Singleton pattern
    /*FileUtils() {};
    FileUtils(FileUtils const&) {};
    FileUtils& operator=(FileUtils const&){};
    static FileUtils* fu_instance;*/

    // Resolve AQR path
    TMX_SDK__STATUS_CODE_e
    resolveUrl(
        const std::string & url,
        const std::string & rootPath,
        unsigned int verbose,
        std::string & resolvedPath,
        STORAGE_INTERFACE_TYPE_e * pStorageInterfaceType = NULL
        )
    {
        TMX_SDK__STATUS_CODE_e    status = TMX_SDK__STATUS_CODE__SUCCESS;
        STORAGE_INTERFACE_TYPE_e  storageInterfaceType = STORAGE_INTERFACE_TYPE__RADOS;

        resolvedPath = "ccurrados://";
        resolvedPath += url;

        if (pStorageInterfaceType)
        {
            *pStorageInterfaceType = storageInterfaceType;
        }

        return status;
    }

public:
    // Get instance
    /*static FileUtils* GetInstance(){
        if (!fu_instance)
            fu_instance = new FileUtils();
        return fu_instance;
    }*/
    /*
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
    */

    /*
    int Write(int handle, char* data, int bytesToWrite, int &bytesWritten) {
        bytesWritten = 0;
        while (bytesWritten < bytesToWrite) {
            bytesWritten += write(handle, data + bytesWritten, bytesToWrite);
            if (bytesWritten = -1) {
                return bytesWritten;
            }
        }
        return 0;
    }*/

    std::list<std::string> List(const std::string url, const std::string rootPath) {
        std::list<std::string> files;
        TMX_SDK__STATUS_CODE_e    status;
        std::string               resolvedPath;
        unsigned int              attrFlag = FALSE;
        unsigned int              partialMatchingFlag = FALSE;
        unsigned int              addURLPrefix = FALSE;
        CCURRADOS_File_t *        pListDataOut = NULL;
        int                       listDataSize = 0;
        int                       i;

        AQR_Initialize(TMX_SDK__DBTRACE_VERBOSITY__ERROR, 5);
        status = resolveUrl(url,rootPath,0,resolvedPath);

        if (status)
        {
            return files;
        }

        status = AQR_List(
            resolvedPath.c_str(),
            url.c_str(),
            &pListDataOut,
            &listDataSize,
            attrFlag,
            partialMatchingFlag,
            addURLPrefix);

        if (status)
        {
            return files;
        }
        if (pListDataOut)
        {
            // free list
            for (i = 0; i < listDataSize; ++i)
            {
                files.push_back(std::string(pListDataOut[i].fileName));
                free(pListDataOut[i].fileName);
            }
            free(pListDataOut);
        }
        
        AQR_Terminate();

        return files;
    }
};
#endif
