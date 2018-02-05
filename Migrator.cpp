#include "lib/AWSConnector.hpp"
#include "lib/PolicyManager.hpp"
#include "lib/FileData.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    AWSConnector aws;
    aws.setBucket("devon-bucket");
    return 0;
}