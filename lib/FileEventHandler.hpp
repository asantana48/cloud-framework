#ifndef FILEEVENTHANDLER_H
#define FILEEVENTHANDLER_H

#include "PolicyManager.hpp"
#include "FileData.hpp"
#include "FileUtils.hpp"
#include "Constants.hpp"
#include "FileEventHandler.hpp"
//#include "Aquari.h"

#include <sys/inotify.h>
#include <iostream>

using namespace std;

class FileEventHandler
{

public:
	void initializeINotify();

	void newFileDataObject(string);

};

#endif