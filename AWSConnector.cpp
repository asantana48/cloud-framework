#include "lib/AWSConnector.hpp"

AWSConnector::AWSConnector() 
{
    client = NULL;
    Aws::InitAPI(options);
}

AWSConnector::~AWSConnector() 
{
    Aws::ShutdownAPI(options);
}

bool AWSConnector::connect(std::string region) 
{
    clientConfig.region = region.c_str();
    if (client)
        delete client;
    client = new Aws::S3::S3Client(clientConfig);
    
    return true;
}

bool AWSConnector::createBucket(std::string bucket)
{
    return false;
}

bool AWSConnector::deleteBucket(std::string bucket) 
{
    return false;
}

bool AWSConnector::getObject(std::string bucket, std::string source, std::string destination)
{
    Aws::S3::Model::GetObjectRequest req;
    req.WithBucket(bucket.c_str()).WithKey(source.c_str());

    auto outcome = client->GetObject(req);

    if (outcome.IsSuccess())
    {
        Aws::OFStream local_file;
        local_file.open(destination.c_str(), std::ios::out | std::ios::binary);
        local_file << outcome.GetResult().GetBody().rdbuf();
        return true;
    }
    else
    {
        std::cout << "GetObject error: " <<
            outcome.GetError().GetExceptionName() << " " <<
            outcome.GetError().GetMessage() << std::endl;
        return false;
    }
}

bool AWSConnector::putObject(std::string bucket, std::string source, std::string destination)
{
    Aws::S3::Model::PutObjectRequest req;
    req.WithBucket(bucket.c_str()).WithKey(destination.c_str());

    auto fileData = Aws::MakeShared<Aws::FStream>("PutObjectInputStream",
        source.c_str(), std::ios_base::in | std::ios_base::binary);

    req.SetBody(fileData);

    auto outcome = client->PutObject(req);

    if (outcome.IsSuccess())
    {
        return true;
    }
    else
    {
        std::cout << "PutObject error: " <<
            outcome.GetError().GetExceptionName() << " " <<
            outcome.GetError().GetMessage() << std::endl;
        return false;
    }
}

bool AWSConnector::deleteObject(std::string bucket, std::string object)  
{ 
    Aws::S3::Model::DeleteObjectRequest req;
    req.WithBucket(bucket.c_str()).WithKey(object.c_str());

    auto outcome = client->DeleteObject(req);

    if (outcome.IsSuccess()) 
    {
        return true;
    }
    else
    {
        std::cout << "DeleteObject error: " <<
            outcome.GetError().GetExceptionName() << " " <<
            outcome.GetError().GetMessage() << std::endl;
        return false;
    }
}

bool AWSConnector::promoteObject(std::string bucket, std::string remote, std::string local) 
{
    return getObject(bucket, remote, local) && deleteObject(bucket, remote);
}

bool AWSConnector::promoteObject(std::string bucket, std::string object) 
{
    return promoteObject(bucket, object, object);
}

bool AWSConnector::demoteObject(std::string bucket, std::string local, std::string remote)
{
    return putObject(bucket, local, remote) && std::remove(local.c_str());
}

bool AWSConnector::demoteObject(std::string bucket, std::string object)
{
    return demoteObject(bucket, object, object);
}

std::list<std::string> AWSConnector::listBuckets()
{
    std::list<std::string> contents;
    auto outcome = client->ListBuckets();

    if (outcome.IsSuccess())
    {
        Aws::Vector<Aws::S3::Model::Bucket> bucket_list =
            outcome.GetResult().GetBuckets();

        for (auto const &bucket : bucket_list)
        {
            contents.push_back(bucket.GetName().c_str());
        }
    }
    else
    {
        std::cout << "ListBuckets error: "
            << outcome.GetError().GetExceptionName() << " - "
            << outcome.GetError().GetMessage() << std::endl;
    }
    return contents;
}

std::list<FileData> AWSConnector::listBucketContents(std::string bucket)
{
    std::list<FileData> contents;
    
    Aws::S3::Model::ListObjectsRequest req;
    req.WithBucket(bucket.c_str());

    auto outcome = client->ListObjects(req);

    if (outcome.IsSuccess())
    {
        Aws::Vector<Aws::S3::Model::Object> object_list =
            outcome.GetResult().GetContents();
        
        for (auto const &object : object_list)
        {
            FileData fd;
            fd.localURI = "/" + bucket +  "/" + object.GetKey().c_str();
            fd.fileSize = object.GetSize();
            fd.isLocal = false;
            contents.push_back(fd);
        }
    }
    else
    {
        std::cout << "ListObjects error: " <<
            outcome.GetError().GetExceptionName() << " " <<
            outcome.GetError().GetMessage() << std::endl;
    }
    return contents;
}

std::list<FileData> AWSConnector::listAllObjects() {
    std::list<FileData> contents;

    for (auto bucket: listBuckets()) 
        contents.splice(contents.end(), listBucketContents(bucket));

    return contents;
}
