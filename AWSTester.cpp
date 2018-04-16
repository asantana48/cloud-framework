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
		cout << b << endl;
	cout << endl;

	
	for (int i = 0; i < argc; ++i) 
	{
		std::string prefix = "/testbed/";
		if (0 == strcmp(argv[i], "delete") && i+1 < argc)	{
			cout << "Deleting object\n";
			aws.deleteObject("devon-bucket", prefix + argv[i+1]);
		}
		else if (0 == strcmp(argv[i], "promote")  && i+1 < argc) {
			cout << "Promoting object\n";
			aws.promoteObject("devon-bucket", argv[i+1], prefix + argv[i+1]);
		}
		else if (0 == strcmp(argv[i], "demote")  && i+1 < argc) {
			cout << "Demoting object\n";
			aws.demoteObject("devon-bucket", prefix +argv[i+1], argv[i+1]);
		}
	}

	return 0;
}
