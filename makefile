CXXFLAGS = -laws-cpp-sdk-s3 -laws-cpp-sdk-core -std=c++11 -lredox -lev -lhiredis -lxml2 -pthread

initialize : Initializer.cpp
	g++ -o initialize Initializer.cpp PolicyManager.cpp FileEventHandler.cpp Redis_Database/Redis_Client.cpp Redis_Database/Redis_Scanner.cpp $(CXXFLAGS)

supervise : Supervisor.cpp
	g++ -o supervise Supervisor.cpp PolicyManager.cpp AWSConnector.cpp Redis_Database/Redis_Client.cpp Redis_Database/Redis_Scanner.cpp $(CXXFLAGS)

recall : Recall.cpp
	g++ -o recall Recall.cpp AWSConnector.cpp Redis_Database/Redis_Client.cpp Redis_Database/Redis_Scanner.cpp $(CXXFLAGS)