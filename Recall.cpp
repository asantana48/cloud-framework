#include <iostream>
#include <fstream>

#include "lib/AWSConnector.hpp"
#include "lib/Constants.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	std::string region = "us-east-2";
	std::string bucket = "devon-bucket";

    AWSConnector aws;
    aws.connect(region);
    for (std::string filename: aws.listBucketContents(bucket)) {
        cout << "Recalling " << filename << "\n";
        std::string localPath = FILES_PATH + filename;
        aws.promoteObject("devon-bucket", filename, localPath);
    }
	return 0;
}
