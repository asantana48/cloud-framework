#include <iostream>
#include <fstream>

#include "lib/AWSConnector.h"




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