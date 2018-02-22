CXXFLAGS = -laws-cpp-sdk-s3 -laws-cpp-sdk-core -std=c++11 -lredox -lev -lhiredis -lxml2
initialize : Initialization.cpp
	g++ -o initialize Initialization.cpp PolicyManager.cpp Redis_Database/Redis_Client.cpp Redis_Database/Redis_Scanner.cpp $(CXXFLAGS)