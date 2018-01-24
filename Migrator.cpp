#include "lib/AWSConnector.h"
#include "lib/PolicyManager.h"
#include "lib/FileData.h"

#include <iostream>

int main(int argc, char* argv[]) {
    AWSConnector aws;
    aws.setBucket("devon-bucket");
    return 0;
}