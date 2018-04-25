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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string>
#include <pthread.h>
#include <signal.h>

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

TMX_SDK__DBTRACE_VERBOSITY_e g_DebugTraceVerbosity = TMX_SDK__DBTRACE_VERBOSITY__ERROR;
int g_run = TRUE;

/*----------------------------------------------------------------------*/
/* Global utility functions */
/*----------------------------------------------------------------------*/

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


TMX_SDK__STATUS_CODE_e
put_object(
    const std::string & url,
    const std::string & rootPath,
    const std::string & filename,
    unsigned int bufferSize,
    unsigned int verbose,
    CCURRADOS_Attributes_s * attributes,
    uint16_t file_flags,
    uint64_t * pBytesTransferred
    )
{
    TMX_SDK__STATUS_CODE_e    status;
    TMX_SDK__STATUS_CODE_e    tempStatus;
    uint64_t                  fileSize = 0;
    uint64_t                  totalBytesWritten = 0;
    unsigned int              oflags;
    ccur::util::DataBuffer    buffer;
    size_t                    bytesToRead;
    size_t                    bytesRead;
    size_t                    bytesWritten;
    int                       ccurHandle = CCURRADOS_INVALID_HANDLE_VALUE;
    ccur::util::File          fileObject;
    std::string               resolvedPath;

    if (pBytesTransferred)
    {
        *pBytesTransferred = 0;
    }

    if (!buffer.create(bufferSize,16))
    {
        fprintf(stderr,"Failed to allocate memory\n");
        status = TMX_SDK__STATUS_CODE__OUT_OF_MEMORY;
        return status;
    }
    else
    {
        buffer.resize(bufferSize);
    }

    status = fileObject.open(
        filename.c_str(),
        O_RDONLY | O_BINARY,
        0 // mode
        );
    if (status)
    {
        fprintf(stderr,"Failed to open file, filename '%s', status(%u)\n",
            filename.c_str(), status);
        return status;
    }
    else
    {
        if (verbose)
        {
            fprintf(stdout,"Opened file, filename '%s'\n", filename.c_str());
        }
    }

    status = fileObject.size(&fileSize);
    if (status)
    {
        fprintf(stderr,"Failed to get file size, filename '%s', status(%u)\n",
            filename.c_str(), status);
        return status;
    }
    else
    {
        if (verbose)
        {
            fprintf(stdout,"Writing object of %" PRIu64 " bytes\n", fileSize);
        }
    }

    status = resolveUrl(url,rootPath,verbose,resolvedPath);
    if (status)
    {
        return status;
    }

    oflags = CCUR_O_CREAT | CCUR_O_TRUNC | CCUR_O_WRONLY;

    status = AQR_OpenEx(
        resolvedPath.c_str(),
        oflags,
        CCUR_OPEN_MODE_CREATE_DATAFILE,
        attributes,
        file_flags,
        &ccurHandle);
    if (status)
    {
        fprintf(stderr,"Failed to open CCUR object, url '%s', status(%u)\n",
            url.c_str(), status);
        return status;
    }
    else
    {
        if (verbose)
        {
            fprintf(stdout,"Opened CCUR object, url '%s'\n", url.c_str());
        }
    }

    while ((totalBytesWritten < fileSize) && !status && g_run)
    {
        bytesToRead = buffer.size();
        if ((fileSize - totalBytesWritten) < bytesToRead)
        {
            bytesToRead = (size_t)(fileSize - totalBytesWritten);
        }

        if (verbose > 1)
        {
            fprintf(stdout,"Reading %zu bytes from file, filename '%s'\n",
                bytesToRead, filename.c_str());
        }
        status = fileObject.read(buffer.data(), bytesToRead, &bytesRead);
        if (status)
        {
            fprintf(stderr,"Failed to read from file, filename '%s', status(%u)\n",
                filename.c_str(), status);
        }
        else if (bytesRead)
        {
            if (verbose > 1)
            {
                fprintf(stdout,"Writing %zu bytes to CCUR object, url '%s'\n",
                    bytesRead, url.c_str());
            }
            status = AQR_Write(ccurHandle, buffer.data(), bytesRead, &bytesWritten);
            if (status)
            {
                fprintf(stderr,"Failed to write to CCUR object, url '%s', status(%u)\n",
                    url.c_str(), status);
            }
            totalBytesWritten += bytesWritten;
        }
        else
        {
            status = TMX_SDK__STATUS_CODE__INVALID_DATA_LENGTH;
            fprintf(stderr,"Zero bytes read from file, filename '%s', status(%u)\n",
                filename.c_str(), status);
        }
    }

    if (!status && (totalBytesWritten < fileSize))
    {
        status = TMX_SDK__STATUS_CODE__NOT_ENOUGH_DATA_WRITTEN;
        fprintf(stderr,"Not enough data written for url '%s',"
            " totalBytesWritten %" PRIu64 ", fileSize %" PRIu64 ","
            " status(%u)\n",
            url.c_str(), totalBytesWritten, fileSize, status);
    }

    tempStatus = AQR_Close(ccurHandle,/*flush*/TRUE);
    if (tempStatus)
    {
        fprintf(stderr,"Failed to close CCUR object, url '%s', status(%u)\n",
            url.c_str(), tempStatus);
        if (!status)
        {
            status = tempStatus;
        }
    }

    tempStatus = fileObject.close();
    if (tempStatus)
    {
        fprintf(stderr,"Failed to close file, filename '%s', status(%u)\n",
            filename.c_str(), tempStatus);
        if (!status)
        {
            status = tempStatus;
        }
    }

    if (pBytesTransferred)
    {
        *pBytesTransferred = totalBytesWritten;
    }

    if (verbose)
    {
        fprintf(stdout,"Wrote %" PRIu64 " bytes to file, filename '%s', fileSize %" PRIu64 "\n",
            totalBytesWritten, url.c_str(), fileSize);
    }
    return status;
}



TMX_SDK__STATUS_CODE_e
get_object(
    const std::string & url,
    const std::string & rootPath,
    const std::string & filename,
    unsigned int bufferSize,
    unsigned int verbose,
    unsigned int persistContent,
    unsigned int testReadingPastEndOfFile,
    uint64_t * pBytesTransferred
    )
{
    TMX_SDK__STATUS_CODE_e    status;
    TMX_SDK__STATUS_CODE_e    tempStatus;
    unsigned int              error;
    uint64_t                  fileSize = 0;
    uint64_t                  totalBytesRead = 0;
    unsigned int              oflags;
    ccur::util::DataBuffer    buffer;
    size_t                    bytesToRead;
    size_t                    bytesRead;
    size_t                    bytesWritten;
    int                       ccurHandle = CCURRADOS_INVALID_HANDLE_VALUE;
    ccur::util::File          fileObject;
    std::string               resolvedPath;

    if (pBytesTransferred)
    {
        *pBytesTransferred = 0;
    }

    if (!buffer.create(bufferSize,16))
    {
        fprintf(stderr,"Failed to allocate memory\n");
        status = TMX_SDK__STATUS_CODE__OUT_OF_MEMORY;
        return status;
    }
    else
    {
        buffer.resize(bufferSize);
    }

    // resolve the given URL into a Ceph Storage Cluster URL
    status = resolveUrl(url,rootPath,verbose,resolvedPath);
    if (status)
    {
        return status;
    }

    if (persistContent)
    {
        // open the destination POSIX file for writing
        status = fileObject.open(
            filename.c_str(),
            O_CREAT | O_TRUNC | O_WRONLY | O_BINARY,
            CCUR_OPEN_MODE_CREATE_DATAFILE
            );
        if (status)
        {
            fprintf(stderr,"Failed to open file, filename '%s', status(%u)\n",
                filename.c_str(), status);
            return status;
        }
        else
        {
            if (verbose)
            {
                fprintf(stdout,"Opened file, filename '%s'\n", filename.c_str());
            }
        }
    }

    // open the source CCURRADOS logical file for reading
    oflags = CCUR_O_RDONLY;

    status = AQR_Open(
        resolvedPath.c_str(),
        oflags, // open flags
        0, // mode
        &ccurHandle);
    error = errno;
    if (status)
    {
        fprintf(stderr,"Failed to open CCUR object, url '%s', status(%u), errno(%u)\n",
            url.c_str(), status, error);
        return status;
    }
    else
    {
        if (verbose)
        {
            fprintf(stdout,"Opened CCUR object, url '%s', status(%u), errno(%u)\n",
                url.c_str(), status, error);
        }
    }

    // get the source CCURRADOS logical file size
    status = AQR_FileSize(ccurHandle,&fileSize);
    if (status)
    {
        fprintf(stderr,"Failed to get CCUR file size, url '%s', status(%u)\n",
            url.c_str(), status);
        AQR_Close(ccurHandle,FALSE);
        return status;
    }
    else
    {
        if (verbose)
        {
            fprintf(stdout,"Reading object of %" PRIu64 " bytes\n", fileSize);
        }
    }

    // loop getting data from the source CCURRADOS logical file and writing to the POSIX file
    while ((totalBytesRead < fileSize) && !status && g_run)
    {
        bytesToRead = buffer.size();
        if ((fileSize - totalBytesRead) < bytesToRead)
        {
            bytesToRead = (size_t)(fileSize - totalBytesRead);
        }

        if (verbose > 1)
        {
            fprintf(stdout,"Reading %zu bytes from CCUR object, url '%s'\n",
                bytesToRead, url.c_str());
        }
        status = AQR_Read(ccurHandle,buffer.data(), bytesToRead, &bytesRead);
        if (status)
        {
            fprintf(stderr,"Failed to read from CCUR object, filename '%s', status(%u)\n",
                url.c_str(), status);
        }
        else if (bytesRead)
        {
            if (persistContent)
            {
                if (verbose > 1)
                {
                    fprintf(stdout,"Writing %zu bytes to file, filename '%s'\n",
                        bytesRead, filename.c_str());
                }
                status = fileObject.write(buffer.data(), bytesRead, &bytesWritten);
                if (status)
                {
                    fprintf(stderr,"Failed to write to file, filename '%s', status(%u)\n",
                        filename.c_str(), status);
                }
                totalBytesRead += bytesWritten;
            }
            else
            {
                totalBytesRead += bytesRead;
            }
        }
        else
        {
            status = TMX_SDK__STATUS_CODE__INVALID_DATA_LENGTH;
            fprintf(stderr,"Zero bytes read from CCUR object, filename '%s', totalBytesRead %" PRIu64 ", fileSize %" PRIu64 ", bytesRead %zu, status(%u)\n",
                url.c_str(), totalBytesRead, fileSize, bytesRead, status);
        }
    }

    if (!status && (totalBytesRead < fileSize))
    {
        status = TMX_SDK__STATUS_CODE__NOT_ENOUGH_DATA_READ;
        fprintf(stderr,"Not enough data read for url '%s',"
            " totalBytesRead %" PRIu64 ", fileSize %" PRIu64 ","
            " status(%u)\n",
            url.c_str(), totalBytesRead, fileSize, status);
    }

    if (!status && testReadingPastEndOfFile)
    {
        offset64_t                offset;

        // test to see the results of reading past the end-of-file;
        // the read is expected to succeed and return zero bytes read
        status = AQR_Read(ccurHandle,buffer.data(),buffer.size(),&bytesRead);
        if (status || (bytesRead != 0))
        {
            if (!status)
            {
                status = TMX_SDK__STATUS_CODE__READ_FILE_FAILED;
            }
            fprintf(stderr,"Failed test of read past end-of-file, filename '%s', totalBytesRead %" PRIu64 ", fileSize %" PRIu64 ", bytesRead %zu, status(%u)\n",
                url.c_str(), totalBytesRead, fileSize, bytesRead, status);
        }

        // test reading more than the Ceph object size past the end of the logical file;
        // the read is expected to succeed and return zero bytes read
        offset = fileSize + 2 * (((int64_t)CCUR_DEFAULT_BYTES_PER_STRIPE) * CCUR_DEFAULT_OBJECTS_PER_OBJECT_SET * CCUR_DEFAULT_STRIPES_PER_OBJECT);
        status = AQR_ReadAtOffset(ccurHandle,buffer.data(),buffer.size(),offset,&bytesRead);
        if (status || (bytesRead != 0))
        {
            if (!status)
            {
                status = TMX_SDK__STATUS_CODE__READ_FILE_FAILED;
            }
            fprintf(stderr,"Failed test of read far past end-of-file, filename '%s', totalBytesRead %" PRIu64 ", fileSize %" PRIu64 ", bytesRead %zu, status(%u)\n",
                url.c_str(), totalBytesRead, fileSize, bytesRead, status);
        }
    }

    if (persistContent)
    {
        // close the POSIX file
        tempStatus = fileObject.close();
        if (tempStatus)
        {
            fprintf(stderr,"Failed to close file object, filename '%s', status(%u)\n",
                filename.c_str(), tempStatus);
            if (!status)
            {
                status = tempStatus;
            }
        }
    }

    // close the CCURRADOS logical file
    tempStatus = AQR_Close(ccurHandle,/*flush*/TRUE);
    if (tempStatus)
    {
        fprintf(stderr,"Failed to close ccur object, url '%s', status(%u)\n",
            url.c_str(), tempStatus);
        if (!status)
        {
            status = tempStatus;
        }
    }

    if (pBytesTransferred)
    {
        *pBytesTransferred = totalBytesRead;
    }

    if (verbose)
    {
        fprintf(stdout,"Read %" PRIu64 " bytes from file, filename '%s', fileSize %" PRIu64 "\n",
            totalBytesRead, url.c_str(), fileSize);
    }
    return status;
}

TMX_SDK__STATUS_CODE_e
removeObject(
    const std::string & url,
    const std::string & rootPath,
    unsigned int verbose
    )
{
    TMX_SDK__STATUS_CODE_e    status;
    std::string               resolvedPath;

    status = resolveUrl(url,rootPath,verbose,resolvedPath);
    if (status)
    {
        return status;
    }

    status = AQR_Remove(resolvedPath.c_str());
    if (status)
    {
        fprintf(stderr,"Failed to removed object '%s', status %u\n",
            url.c_str(), status);
    }
    else
    {
        if (verbose)
        {
            fprintf(stdout,"Removed object '%s'\n",url.c_str());
        }
    }

    return status;
}

TMX_SDK__STATUS_CODE_e
statObject(
    const std::string & url,
    const std::string & rootPath,
    unsigned int verbose
    )
{
    TMX_SDK__STATUS_CODE_e    status;
    std::string               resolvedPath;
    CCUR_file_stat64_t        ccurStat;
    time_t                    tmp_atime;
    time_t                    tmp_mtime;
    time_t                    tmp_ctime;

    status = resolveUrl(url,rootPath,verbose,resolvedPath);
    if (status)
    {
        return status;
    }

    status = AQR_Stat(resolvedPath.c_str(),&ccurStat);
    if (status)
    {
        fprintf(stderr,"Failed to stat CCUR object, url '%s', status(%u)\n",
            url.c_str(), status);
    }
    else
    {
        tmp_atime = (time_t)(ccurStat.mst_atime);
        tmp_mtime = (time_t)(ccurStat.mst_mtime);
        tmp_ctime = (time_t)(ccurStat.mst_ctime);
        fprintf(stdout,"File:    '%s'\n",url.c_str());
        fprintf(stdout,"Device:  0x%" PRIx64 "\n", ccurStat.mst_dev);
        fprintf(stdout,"Inode:   0x%" PRIx64 "\n", static_cast<uint64_t>(ccurStat.mst_ino));
        fprintf(stdout,"Mode:    0%o\n", ccurStat.mst_mode);
        fprintf(stdout,"Links:   %" PRIu64 "\n", static_cast<uint64_t>(ccurStat.mst_nlink));
        fprintf(stdout,"Uid:     %u\n", ccurStat.mst_uid);
        fprintf(stdout,"Gid:     %u\n", ccurStat.mst_gid);
        fprintf(stdout,"RDevice: 0x%" PRIx64 "\n", ccurStat.mst_rdev);
        fprintf(stdout,"Size:    %" PRIu64 " bytes\n", ccurStat.mst_size);
        fprintf(stdout,"Access:  %s", asctime(localtime(&tmp_atime)));
        fprintf(stdout,"Modify:  %s", asctime(localtime(&tmp_mtime)));
        fprintf(stdout,"Change:  %s", asctime(localtime(&tmp_ctime)));
        fprintf(stdout,"BlkSize: %" PRIu64 " bytes\n", static_cast<uint64_t>(ccurStat.mst_blksize));
        fprintf(stdout,"BlkCnt:  %" PRIu64 " blocks\n", static_cast<uint64_t>(ccurStat.mst_blocks));
    }

    return status;
}

TMX_SDK__STATUS_CODE_e
listObjects(
    const std::string & url,
    const std::string & rootPath,
    unsigned int verbose
    )
{
    TMX_SDK__STATUS_CODE_e    status;
    std::string               resolvedPath;
    unsigned int              attrFlag = FALSE;
    unsigned int              partialMatchingFlag = FALSE;
    unsigned int              addURLPrefix = FALSE;
    CCURRADOS_File_t *        pListDataOut = NULL;
    int                       listDataSize = 0;
    int                       i;

    status = resolveUrl(url,rootPath,verbose,resolvedPath);
    if (status)
    {
        return status;
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
        fprintf(stderr,"Failed to list CCUR objects, url '%s', status(%u)\n",
            url.c_str(), status);
        return status;
    }
    else
    {
        if (listDataSize > 0)
        {
            for (i = 0; (i < listDataSize) && g_run; ++i)
            {
                fprintf(stdout,"%s\n",pListDataOut[i].fileName);
            }
        }
        else
        {
            fprintf(stdout,"No CCUR objects found\n");
        }
    }

    if (pListDataOut)
    {
        // free list
        for (i = 0; (i < listDataSize) && g_run; ++i)
        {
            free(pListDataOut[i].fileName);
        }
        free(pListDataOut);
    }

    return status;
}

const char *
getMethodName(unsigned int method)
{
    switch(method)
    {
    case M_GET_OBJECT:
        return "get";
    case M_PUT_OBJECT:
        return "put";
    case M_STAT_OBJECT:
        return "stat";
    case M_REMOVE_OBJECT:
        return "rm";
    case M_LIST_OBJECTS:
        return "ls";
    default:
        return "unknown";
    }
}

TMX_SDK__STATUS_CODE_e
processRequest(
    unsigned int method,
    const std::string & url,
    const std::string & rootPath,
    const std::string & filename,
    unsigned int bufferSize,
    unsigned int verbose,
    unsigned int extraTests,
    unsigned int persistContent,
    unsigned int threadCount,
    unsigned int iterationCount,
    unsigned int fileCount,
    CCURRADOS_Attributes_s * attributes,
    uint16_t file_flags,
    const std::string & clusterName
    )
{
    TMX_SDK__STATUS_CODE_e status = TMX_SDK__STATUS_CODE__SUCCESS;

    if (!status)
    {
        switch (method)
        {
        case M_PUT_OBJECT:
            status = put_object(url,rootPath,filename,bufferSize,verbose,attributes,file_flags,NULL);
            break;
        case M_GET_OBJECT:
            status = get_object(url,rootPath,filename,bufferSize,verbose,TRUE,extraTests,NULL);
            break;
        case M_REMOVE_OBJECT:
            status = removeObject(url, rootPath, verbose);
        case M_STAT_OBJECT:
            status = statObject(url,rootPath,verbose);
            break;
        case M_LIST_OBJECTS:
            status = listObjects(url,rootPath,verbose);
            break;
        default:
            break;
        }

        if (status)
        {
            printf("Method '%s' failed, status(%u): '%s'\n",
                getMethodName(method), status, TMXErrorString(status));
        }
        else
        {
            printf("Method '%s' completed successfully\n",
                getMethodName(method));
        }
    }

    return status;
}



int main(int argc, char * argv[])
{
    TMX_SDK__STATUS_CODE_e status = TMX_SDK__STATUS_CODE__SUCCESS;
    uint32_t writeBufferCacheDelay = 5;
    std::string rootPath;
    std::string url;
    std::string clusterName;
    std::string filename;
    std::string logPath;
    int i;
    int method = M_UNKNOWN;
    int value;
    unsigned int bufferSize = CCUR_DEFAULT_BYTES_PER_STRIPE;
    unsigned int verbose = 0;
    unsigned int extraTests = FALSE;
    unsigned int persistContent = FALSE;
    unsigned int iterationCount = 1;
    unsigned int threadCount = 1;
    unsigned int fileCount = 1;
    bool restart_mode = true;
    uint16_t file_flags = 0;
    CCURRADOS_Attributes_s attributes;


    // Set CCURrados default attributes
    attributes.bytesPerStripe = CCUR_DEFAULT_BYTES_PER_STRIPE;
    attributes.objectsPerObjectSet = CCUR_DEFAULT_OBJECTS_PER_OBJECT_SET;
    attributes.stripesPerObject = CCUR_DEFAULT_STRIPES_PER_OBJECT;
    
    for (i = 1; i < argc; ++i)
    {
        if (0 == strcmp("-u",argv[i]))
        {
            if (++i < argc)
            {
                url = argv[i];
            }
        }
        else if (0 == strcmp("-f",argv[i]))
        {
            if (++i < argc)
            {
                filename = argv[i];
            }
        }
        else if (0 == strcmp("-p",argv[i]))
        {
            if (++i < argc)
            {
                rootPath = argv[i];
            }
        }
        else if (0 == strcmp("-g",argv[i]))
        {
            if (++i < argc)
            {
                logPath = argv[i];
            }
        }
        else if (0 == strcmp("-b",argv[i]))
        {
            if (++i < argc)
            {
                bufferSize = atoi(argv[i]);
                if (bufferSize < CCURRADOS_MINIMUM_BYTES_PER_STRIPE)
                {
                    bufferSize = CCURRADOS_MINIMUM_BYTES_PER_STRIPE;
                }
                else if (bufferSize > CCURRADOS_MAXIMUM_BYTES_PER_STRIPE)
                {
                    bufferSize = CCURRADOS_MAXIMUM_BYTES_PER_STRIPE;
                }
            }
        }
        else if (0 == strcmp("-t",argv[i]))
        {
            if (++i < argc)
            {
                threadCount = atoi(argv[i]);
                if (threadCount < 1)
                {
                    threadCount = 1;
                }
                else if (threadCount > CCUR_MAXIMUM_THREAD_COUNT)
                {
                    threadCount = CCUR_MAXIMUM_THREAD_COUNT;
                }
            }
        }
        else if (0 == strcmp("-i",argv[i]))
        {
            if (++i < argc)
            {
                iterationCount = atoi(argv[i]);
                if (iterationCount < 1)
                {
                    iterationCount = 1;
                }
                else if (iterationCount > CCUR_MAXIMUM_ITERATION_COUNT)
                {
                    iterationCount = CCUR_MAXIMUM_ITERATION_COUNT;
                }
            }
        }
        else if (0 == strcmp("-l",argv[i]))
        {
            if (++i < argc)
            {
                fileCount = atoi(argv[i]);
                if (fileCount < 1)
                {
                    fileCount = 1;
                }
                else if (fileCount > CCUR_MAXIMUM_FILE_COUNT)
                {
                    fileCount = CCUR_MAXIMUM_FILE_COUNT;
                }
            }
        }
        else if (0 == strcmp("-persist",argv[i]))
        {
            persistContent = TRUE;
        }
        else if (0 == strcmp("-abps",argv[i]))
        {
            if (++i < argc)
            {
                value = atoi(argv[i]);
                if ((value < CCURRADOS_MINIMUM_BYTES_PER_STRIPE) ||
                    (value > CCURRADOS_MAXIMUM_BYTES_PER_STRIPE))
                {
                    fprintf(stderr,"Specified bytes per stripe value: %d is out of range [%u..%u]\n",
                        value, CCURRADOS_MINIMUM_BYTES_PER_STRIPE, CCURRADOS_MAXIMUM_BYTES_PER_STRIPE);
                }
                else
                {
                    attributes.bytesPerStripe = value;
                }
            }
        }
        else if (0 == strcmp("-aopos",argv[i]))
        {
            if (++i < argc)
            {
                value = atoi(argv[i]);
                if ((value < CCURRADOS_MINIMUM_OBJECTS_PER_OBJECT_SET) ||
                    (value > CCURRADOS_MAXIMUM_OBJECTS_PER_OBJECT_SET))
                {
                    fprintf(stderr,"Specified objects per object set value: %d is out of range [%u..%u]\n",
                        value, CCURRADOS_MINIMUM_OBJECTS_PER_OBJECT_SET, CCURRADOS_MAXIMUM_OBJECTS_PER_OBJECT_SET);
                }
                else
                {
                    attributes.objectsPerObjectSet = value;
                }
            }
        }
        else if (0 == strcmp("-aspo",argv[i]))
        {
            if (++i < argc)
            {
                value = atoi(argv[i]);
                if ((value < CCURRADOS_MINIMUM_STRIPES_PER_OBJECT) ||
                    (value > CCURRADOS_MAXIMUM_STRIPES_PER_OBJECT))
                {
                    fprintf(stderr,"Specified stripes per object value: %d is out of range [%u..%u]\n",
                        value, CCURRADOS_MINIMUM_STRIPES_PER_OBJECT, CCURRADOS_MAXIMUM_STRIPES_PER_OBJECT);
                }
                else
                {
                    attributes.stripesPerObject = value;
                }
            }
        }
        else if (0 == strcmp("-cluster",argv[i]))
        {
            if (++i < argc)
            {
                clusterName = argv[i];
            }
        }
        else if (0 == strcmp("-no_restart",argv[i]))
        {
            restart_mode = false;
        }
        else if (0 == strcmp("-m",argv[i]))
        {
            if (++i < argc)
            {
                if (0 == strcmp("get",argv[i]))
                {
                    method = M_GET_OBJECT;
                }
                else if (0 == strcmp("put",argv[i]))
                {
                    method = M_PUT_OBJECT;
                }
                else if (0 == strcmp("rm",argv[i]))
                {
                    method = M_REMOVE_OBJECT;
                }
                else if (0 == strcmp("stat",argv[i]))
                {
                    method = M_STAT_OBJECT;
                }
                else if (0 == strcmp("ls",argv[i]))
                {
                    method = M_LIST_OBJECTS;
                }
                /*else if (0 == strcmp("puttest",argv[i]))
                {
                    method = M_TEST_PUT_OBJECT;
                }
                else if (0 == strcmp("pttest",argv[i]))
                {
                    method = M_TEST_PURGE_TRUNCATE;
                }
                else if (0 == strcmp("mrtest",argv[i]))
                {
                    method = M_TEST_MULTI_READ_THREAD;
                }
                else if (0 == strcmp("mwtest",argv[i]))
                {
                    method = M_TEST_MULTI_WRITE_THREAD;
                }
                else if (0 == strcmp("statpool",argv[i]))
                {
                    method = M_STAT_POOL;
                }
                else if (0 == strcmp("captest", argv[i]))
                {
                    method = M_GET_CLUSTER_CAPACITY_REMAINING;
                }
                else if (0 == strcmp("mrwtest",argv[i]))
                {
                    method = M_TEST_MULTI_READ_WRITE_THREAD;
                }
                else if (0 == strcmp("clustusagetest",argv[i]))
                {
                    method = M_GET_CLUSTER_USAGE_STATUS;
                }
                else if(0 == strcmp("fdpool",argv[i]))
                {
                    method = M_FIND_DELETED_POOL;
                }
                else if(0 == strcmp("qfadel",argv[i]))
                {
                    method = M_QUEUE_FOR_ASYNC_DELETE;
                }
                else if(0 == strcmp("fstat",argv[i]))
                {
                    method = M_FSTAT_OBJECT;
                }
                else if(0 == strcmp("getcont",argv[i]))
                {
                    method = M_GET_CONTINUOUS;
                }
                else if(0 == strcmp("putcont",argv[i]))
                {
                    method = M_PUT_CONTINUOUS;
                }
                else if(0 == strcmp("iostats", argv[i]))
                {
                    method = M_IOSTATS;
                }
                else if(0 == strcmp("iostatsreset", argv[i]))
                {
                    method = M_IOSTATS_RESET;
                }*/
            }
        }

        // else if (0 == strcmp("-v",argv[i]))
        // {
        //     verbose += 1;
        // }
        // else if (0 == strcmp("-e",argv[i]))
        // {
        //     extraTests = TRUE;
        // }
        // else
        // {
        //     usage();
        //     return 1;
        // }
    }


    // if the filename was not specified, then
    // set it to the filename at the end of the URL
    if (!url.empty() && filename.empty())
    {
        const char * p = strrchr(url.c_str(),'/');
        if (p)
        {
            filename.assign(p+1);
        }
        else
        {
            filename.assign(url);
        }
    }

    // Initialize the DebugTraceInterface to send debug trace messages to
    // a log file or to syslog.
    {
        DEBUG_TRACE_DESTINATION_s logDestination;
        if (logPath.empty())
        {
            logDestination.logType = DEBUG_TRACE_LOG_TYPE__OUTPUT_SYSLOG;
            logDestination.param = LOG_DAEMON; // syslog facility
            logDestination.name = "CCURrados_test";
            logDestination.path = NULL;
        }
        else
        {
            logDestination.logType = DEBUG_TRACE_LOG_TYPE__OUTPUT_TRACE_FILE;
            logDestination.param = 0; // maximum log lines
            logDestination.name = "CCURrados_test";
            logDestination.path = logPath.c_str();
        }
        status = DebugTraceInterface_Initialize(1, &logDestination);
        if (status)
        {
            fprintf(stderr,"Failed to initialize the DebugTraceInterface, status(%u) '%s'\n",
                status, TMXErrorString(status));
            return status;
        }
    }

    // Initialize the CCURrados shared library.
    status = AQR_Initialize(g_DebugTraceVerbosity,writeBufferCacheDelay);
    if (status)
    {
        fprintf(stderr,"Failed to initialize the CCURrados shared library, status(%u) '%s'\n",
            status, TMXErrorString(status));
        return status;
    }
    else
    {
        fprintf(stdout,"Initialized CCURrados\n");
    }

    if (!restart_mode)
        file_flags |= CCURRADOS_NO_RESTART_CONTENT_FLAG;

    status = processRequest(method,url,rootPath,filename,bufferSize,
        verbose,extraTests,persistContent,
        threadCount,iterationCount,fileCount,&attributes,file_flags,clusterName);

    // Cleanup resources
    AQR_Terminate();

    DebugTraceInterface_Terminate();

    return status;
}

