#include "lib/FileUtils.hpp"
#include <iostream>

using namespace std;
int main(int argc, char* argv[]) {
	FileUtils fUtil;
	std::list<std::string> ls = fUtil.List("/ceph/content/", "");

	cout << "Files in Aquari:\n";
	for (std::string filename: ls){
		cout << filename << "\n";
	}
	return 0;
}