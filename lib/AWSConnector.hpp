#ifndef AWSCONNECTOR_H
#define AWSCONNECTOR_H

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>

#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/Object.h>

#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

class AWSConnector {
private:
	std::unique_ptr<Aws::S3::S3Client> client;
	Aws::Client::ClientConfiguration clientConfig;
	Aws::SDKOptions options; // leave default for now
	Aws::String currentBucket;

public:
    AWSConnector();
    ~AWSConnector();

    void connect(std::string region);
    void setBucket(std::string bucket);

    void getObject(std::string source, std::string destination);
    void putObject(std::string source, std::string destination);

	void listBuckets();

	//List objects (keys) within an Amazon S3 bucket.
	//Parameters: list_objects(bucketName, regionName)
	void listObjects();
	void listObjects(std::string bucket);
};
#endif

