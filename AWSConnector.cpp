#include "lib/AWSConnector.h"

AWSConnector::AWSConnector() {
	Aws::InitAPI(options);
}

AWSConnector::~AWSConnector() {
	Aws::ShutdownAPI(options);
}

void AWSConnector::connect(std::string region) {
    clientConfig.region = region.c_str();
	if (client)
		client.reset();
	client = std::make_unique<Aws::S3::S3Client>(clientConfig);
}

void AWSConnector::listBuckets()
{
    auto outcome = client->ListBuckets();

    if (outcome.IsSuccess())
    {
        std::cout << "Your Amazon S3 buckets:" << std::endl;

        Aws::Vector<Aws::S3::Model::Bucket> bucket_list =
            outcome.GetResult().GetBuckets();

        for (auto const &bucket : bucket_list)
        {
            std::cout << "  * " << bucket.GetName() << std::endl;
        }
    }
    else
    {
        std::cout << "ListBuckets error: "
            << outcome.GetError().GetExceptionName() << " - "
            << outcome.GetError().GetMessage() << std::endl;
    }
}

void AWSConnector::setBucket(std::string bucket)
{
    currentBucket = bucket.c_str();
}

void AWSConnector::listObjects()
{
    if (currentBucket.empty())
        return;

    std::cout << "Objects in S3 bucket: " << currentBucket << std::endl;

    Aws::S3::Model::ListObjectsRequest objects_request;
    objects_request.WithBucket(currentBucket);

    auto list_objects_outcome = client->ListObjects(objects_request);

    if (list_objects_outcome.IsSuccess())
    {
        Aws::Vector<Aws::S3::Model::Object> object_list =
            list_objects_outcome.GetResult().GetContents();

        for (auto const &s3_object : object_list)
        {
            std::cout << "* " << s3_object.GetKey() << std::endl;
        }
    }
    else
    {
        std::cout << "ListObjects error: " <<
            list_objects_outcome.GetError().GetExceptionName() << " " <<
            list_objects_outcome.GetError().GetMessage() << std::endl;
    }
}

void AWSConnector::listObjects(std::string bucket) {
	setBucket(bucket);
	listObjects();
}

void AWSConnector::getObject(std::string source, std::string destination)
{

    std::cout << "Downloading " << source << " from S3 bucket: " <<
        currentBucket << " as " << destination << std::endl;

    Aws::S3::Model::GetObjectRequest object_request;
    object_request.WithBucket(currentBucket).WithKey(source.c_str());

    auto get_object_outcome = client->GetObject(object_request);

    if (get_object_outcome.IsSuccess())
    {
        Aws::OFStream local_file;
        local_file.open(destination.c_str(), std::ios::out | std::ios::binary);
        local_file << get_object_outcome.GetResult().GetBody().rdbuf();
        std::cout << "Done!" << std::endl;
    }
    else
    {
        std::cout << "GetObject error: " <<
            get_object_outcome.GetError().GetExceptionName() << " " <<
            get_object_outcome.GetError().GetMessage() << std::endl;
    }
}



void AWSConnector::putObject(std::string source, std::string destination)
{
    std::cout << "Uploading " << source << " to S3 bucket " <<
        currentBucket << " as " << destination << std::endl;

    Aws::S3::Model::PutObjectRequest object_request;
    object_request.WithBucket(currentBucket).WithKey(destination.c_str());

    auto input_data = Aws::MakeShared<Aws::FStream>("PutObjectInputStream",
        source.c_str(), std::ios_base::in | std::ios_base::binary);

    object_request.SetBody(input_data);

    auto put_object_outcome = client->PutObject(object_request);

    if (put_object_outcome.IsSuccess())
    {
        std::cout << "Done!" << std::endl;
    }
    else
    {
        std::cout << "PutObject error: " <<
            put_object_outcome.GetError().GetExceptionName() << " " <<
            put_object_outcome.GetError().GetMessage() << std::endl;
    }
}
