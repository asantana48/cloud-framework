#include "lib/PolicyManager.h"
#include "lib/SizePolicy.h"
#include "lib/TimePolicy.h"

#include <iostream>

using namespace std;

int main()
{
	list<FileData> flist;
	FileData fd1, fd2;

	fd1.fileSize = 500;
	fd2.fileSize = 1500;

	flist.push_back(fd1);
	flist.push_back(fd2);

	PolicyManager pm;
	pm.addPolicy(new SizePolicy(1024, true));
	pm.addPolicy(new SizePolicy(2048, false));

	list<FileData> migrate = pm.getFileDemotionList(flist);
	 
	for (auto it = migrate.begin(); it != migrate.end(); ++it) {
		cout << it->fileSize << endl;
	}
	return 0;
}
