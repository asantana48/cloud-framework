To install Redis, you can type in the following commands in the terminal:
wget http://download.redis.io/redis-stable.tar.gz
tar xvzf redis-stable.tar.gz
cd redis-stable
make

You can run "make test" afterwards to check that all files are installed correctly.

When begining to use the database, be sure to link the dynamic library for Redox.

LD_LIBRARY_PATH=*path leading to Redox build directory*
export LD_LIBRARY_PATH

Compile the cpp files using this line:
g++ Redis_Client.cpp Redis_Scanner.cpp -std=c++11 -lredox -lev -lhiredis

This database is still a work in progress. Stay tuned for updates!
