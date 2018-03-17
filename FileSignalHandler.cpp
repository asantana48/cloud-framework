#include "lib/PolicyManager.hpp"
#include "lib/FileData.hpp"
#include "lib/FileUtils.hpp"
#include "lib/Constants.hpp"
#include "Redis_Database/include/Redis_Client.hpp"


#include <sys/inotify.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

