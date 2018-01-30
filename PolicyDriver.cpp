#include "lib/PolicyManager.h"
#include "lib/SizePolicy.h"
#include "lib/TimePolicy.h"

#include <iostream>

using namespace std;

int main()
{
	list<FileData> flist;
	FileData smallOld, mediumOld, mediumNew, largeNew;

    smallOld.fileSize = 500;
	mediumOld.fileSize = 1450;
    mediumNew.fileSize = 1500;
    largeNew.fileSize = 2000;

    smallOld.lastModified = time(NULL) - 5000;
    mediumOld.lastModified = time(NULL) - 5000;
    mediumNew.lastModified = time(NULL) - 20000;
    largeNew.lastModified = time(NULL) - 20000;

    flist.push_back(smallOld);
	flist.push_back(mediumOld);
    flist.push_back(mediumNew);
    flist.push_back(largeNew);

	PolicyManager pm;

    // Defined policies
	pm.addPolicy(new SizePolicy(1280, true));
    pm.addPolicy(new SizePolicy(1600, false));
    pm.addPolicy(new TimePolicy(8000));

	list<FileData> migrate = pm.getFileDemotionList(flist);
	 
	for (auto it = migrate.begin(); it != migrate.end(); ++it) {
		cout << it->fileSize << endl;
	}
	return 0;
}
