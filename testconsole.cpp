
#include <windows.h>
#include <iostream>
#include <string>

using std::string, std::cout, std::endl;

#include "utils.h"

int main(int argc, char *argv[])
{
	bool LaunchedByDoubleClick = false;

	if (ownTheConsole()) {
		cout << "We own the console" << endl;
		LaunchedByDoubleClick = true;
	}
	else {
		cout << "We don't own the console" << endl;
	}

	DWORD consoleCount = consoleListLength();

	cout << "There are " << consoleCount << " consoles." << endl;
	if (consoleCount == 1) {
		LaunchedByDoubleClick = true;
	}

	cout << "This was " << (LaunchedByDoubleClick? "" : "not ") <<
		"launched by double click" << endl;

	Sleep(4000);
	if (LaunchedByDoubleClick) {
		cout << "More waiting..." << endl;
		Sleep(4000);
	}

	return 0;
}
