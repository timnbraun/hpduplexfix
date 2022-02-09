//
// gethpduplex
//
//   Show whether the duplexer is available on my hp printer
//
//   Option to call powershell to enable duplexing
//
//  Copyright (c) 2022 Tim Braun <tim.n.braun@gmail.com>
//
#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <cstdlib>

using std::string, std::endl, std::cout, std::cerr;
using std::filesystem::path;
using std::vector, std::pair;

#include "hpprinterkeys.h"
#include "printerutils.h"
#include "getversion.h"
#include "utils.h"

const string buildDate = BUILD_DATE;

bool verbose = false;

vector<name_value> dumpDriverFeatures( const string key );
bool enableDuplex();

int main(int argc, char *argv[])
{
	int rc = 0;
	string program = path(argv[0]).stem().string();

	if (argc == 2 && string(argv[1]) == "--version") {
		string product, version;

		getProductAndVersion( product, version );
		cout << product << " V" << version << " " << buildDate << endl;

		exit(0);
	}

	if (program == "sethpduplex" ||
		(argc == 2 && string(argv[1]) == "--set")) {

		if (!IsUserAnAdmin()) {
			cout << "This may not work - you're not in admin group" << endl;
		}
		enableDuplex( PrinterName, PropertyName );
	}
	else {

		vector<name_value> pairs;
		pairs = dumpDriverFeatures( DriverKey, FeatureKeyword );
		if (verbose) {
			cout << "From CurrentControlset:" << endl;
			for (const auto &p : pairs) {
				cout << ' ' << p.first << " = " << p.second << endl;
			}
		}
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

#if 0
		cout << endl << "From WOW6432Node:" << endl;
		pairs = dumpDriverFeatures( DriverKey64 );
		for (const auto &p : pairs) {
			cout << ' ' << p.first << " = " << p.second << endl;
		}
#endif
	}
	return rc;
}
