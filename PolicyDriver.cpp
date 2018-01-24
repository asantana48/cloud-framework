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
	mediumOld.fileSize = 1500;
    mediumNew.fileSize = 1500;
    largeNew.fileSize = 2000;

    flist.push_back(smallOld);
	flist.push_back(mediumOld);
    flist.push_back(mediumNew);
    flist.push_back(largeNew);

	PolicyManager pm;

    // Defined policies
	pm.addPolicy(new SizePolicy(1280, true));
    pm.addPolicy(new SizePolicy(2048, false));

	list<FileData> migrate = pm.getFileDemotionList(flist);
	 
	for (auto it = migrate.begin(); it != migrate.end(); ++it) {
		cout << it->fileSize << endl;
	}
	return 0;
}
