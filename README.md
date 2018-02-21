# cloud-framework

## Requirements
AWS SDK for C++: core, s3 components

libxml2

## Compiling
g++ Migrator.cpp AWSConnector.cpp PolicyManager.cpp Redis_Database/Redis_Client.cpp Redis_Database/Redis_Scanner.cpp-laws-cpp-sdk-s3 -laws-cpp-sdk-core -std=c++14 -lredox -lev -lhiredis [-I/usr/include/libxml2 | -lxml2] -o cloud_framework


