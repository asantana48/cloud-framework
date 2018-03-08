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
    for (FileData fd : aws.listAllObjects()) {
        cout << "Recalling " << fd.getLocalURI() << "\n";
        std::string path = FILES_PATH + fd.getLocalURI();
        aws.promoteObject("devon-bucket", fd.getLocalURI(), path);
    }
	return 0;
}
