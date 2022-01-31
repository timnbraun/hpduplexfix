
#include <windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdio>

using std::string, std::cout, std::endl;
using std::filesystem::path;

int main(int argc, char *argv[])
{
	string program = path(argv[0]).stem().string();
	cout << "This is " << program << '.' << endl;
	return 0;
}
