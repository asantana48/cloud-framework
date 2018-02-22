#include <iostream>
#include <fstream>

#include "lib/AWSConnector.hpp"


using namespace std;

int main(int argc, char* argv[])
{
	std::string region = "us-east-2";
	std::string bucket = "devon-bucket";
	
	AWSConnector aws;
	aws.connect(region);

	cout << "BUCKETS\n";
	for (auto const &b: aws.listBuckets()) {
		cout << b << endl;
	}
	cout << endl;

	cout << "OBJECTS in devon-bucket\n";
	for (auto &b: aws.listBucketContents("devon-bucket"))
		cout << b.getName() << " (" << b.getFileSize() << ")" << endl;
	cout << endl;

	cout << "ALL OBJECTS\n";
	for (auto &b: aws.listAllObjects())
		cout << b.getName() << " (" << b.getFileSize() << ")" << endl;
	cout << endl;
	
	for (int i = 0; i < argc; ++i) 
	{
		if (0 == strcmp(argv[i], "delete"))	{
			cout << "Deleting object\n";
			aws.deleteObject("devon-bucket", "AWSTester.cpp");
		}
		else if (0 == strcmp(argv[i], "promote")) {
			cout << "Promoting object\n";
			aws.promoteObject("devon-bucket", "small_old", "testbed/small_old");
		}
		else if (0 == strcmp(argv[i], "demote")) {
			cout << "Demoting object\n";
			aws.demoteObject("devon-bucket", "testbed/small_old", "small_old");
		}
	}

	return 0;
}
