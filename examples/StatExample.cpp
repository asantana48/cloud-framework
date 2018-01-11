#include <sys/stat.h>
#include <ctime>
#include <iostream>

using namespace std;

// Example of how to use POSIX stat 
int main() {
  struct stat st;
  
  // Output date last modified of a file
  if (stat("Source.cpp", &st) == 0)
    cout << ctime(&st.st_mtime) << endl;

  return 0;
}