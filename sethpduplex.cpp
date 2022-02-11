//
// sethpduplex
//
//   Call powershell to enable duplexing
//   Option to show whether the duplexer is available on my hp printer
//
//

#include <windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdio>

using std::string, std::cout, std::endl;
using std::filesystem::path;
using std::vector, std::pair;

#include "hpprinterkeys.h"
#include "printerutils.h"
#include "getversion.h"
#include "utils.h"

const string buildDate = BUILD_DATE;

bool verbose = false;

int main(int argc, char *argv[])
{
	string program = path(argv[0]).stem().string();

	if (argc == 2 && string(argv[1]) == "--version") {
		string product, version;

		getProductAndVersion( product, version );
		cout << program << " V" << version << " " << buildDate << endl;

		exit(0);
	}
	else if (argc == 2 && (string(argv[1]) == "--get" ||
		string(argv[1]) == "--view")) {
		LRESULT rc = 0;

		vector<name_value> pairs;
		pairs = dumpDriverFeatures( DriverKey, FeatureKeyword );
		for (const auto &p : pairs) {
			if (p.first == "DuplexUnit") {
				if (p.second == "TRUE") {
					cout << "Duplexer is enabled." << endl;
				}
				else {
					cout << "Duplexer is not available." << endl;
					rc = 1;
				}
				break;
			}
		}

		exit(rc);
	}

	//
	// Do the work to enable the feature
	//
	enableDuplex(PrinterName, PropertyName);
	cout << "Duplexer enabled on '" << PrinterName << "'" << endl;

	bool LaunchedByDoubleClick = (consoleListLength() == 1);
	cout << "This was " << (LaunchedByDoubleClick? "" : "not ") <<
		"launched by double click" << endl;

	// If the console is going to disappear, keep it on screen for a short time
	if (LaunchedByDoubleClick) {
		Sleep(4000);
	}

	return 0;
}
