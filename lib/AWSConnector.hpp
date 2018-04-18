#ifndef AWSCONNECTOR_H
#define AWSCONNECTOR_H

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>

#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/Object.h>

#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>

#include "FileData.hpp"

#include <fstream>
#include <string>
#include <cstdio>

class AWSConnector {
private:
	Aws::S3::S3Client* client;
	Aws::Client::ClientConfiguration clientConfig;
	Aws::SDKOptions options;  // leave default 

public:
    AWSConnector();
    ~AWSConnector();

    bool connect(std::string region);

	// Operations on buckets
    bool createBucket(std::string bucket); // tbi
	bool deleteBucket(std::string name);   // tbi

	// Operations on file objects
    bool getObject(std::string bucket, std::string remote, std::string local);
    bool putObject(std::string bucket, std::string local, std::string remote);
	bool deleteObject(std::string bucket, std::string object);

	// Full migration methods
	bool promoteObject(std::string bucket, std::string remote, std::string local);
	bool promoteObject(std::string bucket, std::string object);
	
	bool demoteObject(std::string bucket, std::string local, std::string remote);
	bool demoteObject(std::string bucket, std::string object);
	
	// Information retrieval functions
	bool existsObject(std::string bucket, std::string object);
	std::list<std::string> listBuckets();
	std::list<std::string> listBucketContents(std::string bucket);
	// std::list<string> listAllObjects();
};
#endif

