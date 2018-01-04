#include <iostream>
#include <fstream>

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>

#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/Object.h>

#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>




//Size only migration. For demonstration purposes.
#include <stdio.h>
#include <string>
long double fileSizeThresholdInBytes = 10000;
using namespace std;

void setFileSizeThresholdInBytes(long double x)
{
	cout << "File Size Threshold set to " << x << " bytes.\n";
	fileSizeThresholdInBytes = x;
	return;
}

size_t getFileSize(const std::string& filename)
{
	struct stat st;
	if (stat(filename.c_str(), &st) != 0)
	{
		return 0;
	}
	return st.st_size;
}

void migrateFilesSizeOnly(Aws::String databaseName)
{
	cout << "\nSearching " << databaseName << " for files larger than " << fileSizeThresholdInBytes << " bytes.\n";
	ifstream inFile;
	inFile.open(databaseName);
	if (inFile)
		cout << "Opened the file." << endl;

	Aws::String filename;
	Aws::String databaseNameString = databaseName;
	
	string outputFile;

	while (inFile.eof() != true)
	{
		string s;
		inFile >> s;
		cout << s;
		outputFile.append(s);
		if (getFileSize(s) >= fileSizeThresholdInBytes)
		{
			cout << " <-- Migrate!";
			outputFile.append("*");
		}
		cout << endl;
		outputFile.append("\n");
	}
	inFile.close();

	ofstream newDatabaseTxt;
	newDatabaseTxt.open("database.txt");
	if (!newDatabaseTxt)
		cout << "Output file failed to open!";
	newDatabaseTxt << outputFile;
	newDatabaseTxt.close();
}
//end fileSizeOnly functions




/*void usage_instructions(int stage, Aws::String command)
{
switch (stage)
{
case 0:
std::cout << "migration-framework [list|upload|download] ";
case 1:
if (command.compare("list") == 0)
std::cout << "[all|bucket-name] ";
else if (command.compare("upload") == 0)
std::cout << "[local-filepath] ";
}
std::cout << "\n";
}*/

void list_buckets()
{
	Aws::S3::S3Client s3_client;
	auto outcome = s3_client.ListBuckets();

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



//List objects (keys) within an Amazon S3 bucket.
//Parameters: list_objects(bucketName, regionName)
void list_objects(Aws::String regionName, Aws::String bucketName)
{
	Aws::Client::ClientConfiguration clientConfig;
	clientConfig.region = regionName;


	Aws::S3::S3Client s3_client(clientConfig);

	std::cout << "Objects in S3 bucket: " << bucketName << std::endl;

	Aws::S3::Model::ListObjectsRequest objects_request;
	objects_request.WithBucket(bucketName);

	auto list_objects_outcome = s3_client.ListObjects(objects_request);

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



void get_object(Aws::String regionName, Aws::String bucket_name, Aws::String object_name)
{

	Aws::Client::ClientConfiguration clientConfig;
	clientConfig.region = regionName;

	std::cout << "Downloading " << object_name << " from S3 bucket: " <<
		bucket_name << std::endl;

	Aws::S3::S3Client s3_client(clientConfig);

	Aws::S3::Model::GetObjectRequest object_request;
	object_request.WithBucket(bucket_name).WithKey(object_name);

	auto get_object_outcome = s3_client.GetObject(object_request);

	if (get_object_outcome.IsSuccess())
	{
		Aws::OFStream local_file;
		local_file.open(object_name.c_str(), std::ios::out | std::ios::binary);
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



void put_object(Aws::String bucket, Aws::String source, Aws::String dest, Aws::String region)
{
	std::cout << "Uploading " << source << " to S3 bucket " <<
		bucket << " as " << dest << std::endl;

	Aws::Client::ClientConfiguration clientConfig;
	if (!region.empty())
		clientConfig.region = region;
	Aws::S3::S3Client s3_client(clientConfig);

	Aws::S3::Model::PutObjectRequest object_request;
	object_request.WithBucket(bucket).WithKey(dest);

	auto input_data = Aws::MakeShared<Aws::FStream>("PutObjectInputStream",
		source.c_str(), std::ios_base::in | std::ios_base::binary);

	object_request.SetBody(input_data);

	auto put_object_outcome = s3_client.PutObject(object_request);

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


int main(int argc, char* argv[])
{
	if (argc < 2)
		return -1;

	Aws::String region = "us-east-2";
	Aws::SDKOptions options;
	Aws::InitAPI(options);
	{
		Aws::String command;
		Aws::String bucket;
		Aws::String sourceFile;
		Aws::String destFile;


		command = argv[1];



		


		// 3 argument commands
		if (argc < 3)
			return -1;

		bucket = argv[2];

		//migrateFilesSizeOnly
		//bucket = database in this case
		if (command.compare("migrateFilesSizeOnly") == 0)
		{
			migrateFilesSizeOnly(bucket);
		}
		else if (command.compare("list") == 0)
		{
			if (bucket.compare("all") == 0)
				list_buckets();
			else
				list_objects(region, bucket);
		}




		

		// 4 argument commands
		if (argc < 4)
			return -1;

		sourceFile = argv[3];

		if (command.compare("download") == 0)
		{
			get_object(region, bucket, sourceFile);
		}


		//set policy
		if (command.compare("setPolicy") == 0)
		{
			if (bucket.compare("sizeOnly") == 0)
			{
				long double x = stold(sourceFile);

				setFileSizeThresholdInBytes(x);
			}
		}





		// 5 argument commands
		if (argc < 5)
			return -1;

		destFile = argv[4];

		if (command.compare("upload") == 0)
		{
			put_object(bucket, sourceFile, destFile, region);
		}
	}
	Aws::ShutdownAPI(options);
}