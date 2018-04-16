#include <iostream>
#include <fstream>

#include "lib/AWSConnector.hpp"
#include "lib/Constants.hpp"


using namespace std;

int main(int argc, char* argv[])
{
	std::string region = "us-east-2";
	
	AWSConnector aws;
	aws.connect(region);

	
	if (argc == 1) {
		cout << "BUCKETS\n";
		for (auto const &b: aws.listBuckets()) {
			cout << b << endl;
		}
		cout << endl;
	}
	for (int i = 0; i < argc; ++i) 
	{
		std::string prefix = "testbed/";
		if(0 == strcmp(argv[i], "get") && i+1 < argc)	{
			cout << "Getting object\n";
			aws.getObject(BUCKET, argv[i+1], prefix + argv[i+1]);
		}		
		if(0 == strcmp(argv[i], "put") && i+1 < argc)	{
			cout << "Putting object\n";
			aws.putObject(BUCKET, prefix + argv[i+1], argv[i+1]);
		}		
		else if(0 == strcmp(argv[i], "delete") && i+1 < argc)	{
			cout << "Deleting object\n";
			aws.deleteObject(BUCKET, prefix + argv[i+1]);
		}
		else if (0 == strcmp(argv[i], "promote")  && i+1 < argc) {
			cout << "Promoting object\n";
			aws.promoteObject(BUCKET, argv[i+1], prefix + argv[i+1]);
		}
		else if (0 == strcmp(argv[i], "demote")  && i+1 < argc) {
			cout << "Demoting object\n";
			aws.demoteObject(BUCKET, prefix +argv[i+1], argv[i+1]);
		}
		else if (0 == strcmp(argv[i], "list")) {
			cout << "OBJECTS in chosen bucket\n";
			for (auto &b: aws.listBucketContents(BUCKET))
				cout << b << endl;
			cout << endl;
		}
	}
	cout << "Done.\n";

	return 0;
}
