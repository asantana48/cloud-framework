#include <iostream>
#include <fstream>

#include "lib/AWSConnector.hpp"



int main(int argc, char* argv[])
{
	if (argc < 2)
		return -1;

	std::string region = "us-east-2";
	std::string bucket = "devon-bucket";

	std::string command;
	
	std::string sourceFile;
	std::string destFile;
	
	command = argv[1];

	AWSConnector aws;
	aws.connect(region);


	// 3 argument commands
	if (argc < 3)
		return -1;

	bucket = argv[2];

	//migrateFilesSizeOnly
	//bucket = database in this case
	//if (command.compare("migrateFilesSizeOnly") == 0)
	//{
	//	migrateFilesSizeOnly(bucket);
	//}
	
	if (command.compare("list") == 0)
	{
		if (bucket.compare("all") == 0)
			aws.listBuckets();
		else {
			aws.listObjects(bucket);
		}
	}

	// 4 argument commands
	if (argc < 4)
		return -1;
	
	aws.setBucket(bucket);

	sourceFile = argv[3];

	// 5 argument commands
	if (argc < 5)
		destFile = sourceFile;
	else
		destFile = argv[4];

	if (command.compare("download") == 0)
	{
		aws.getObject(sourceFile, destFile);
	}
	else if (command.compare("upload") == 0)
	{
		aws.putObject(sourceFile, destFile);
	}

	//set policy
	/*if (command.compare("setPolicy") == 0)
	{
		if (bucket.compare("sizeOnly") == 0)
		{
			long double x = stold(sourceFile);

			setFileSizeThresholdInBytes(x);
		}
	}
	
	

	*/
	return 0;
}
