//
// driverfeatures.cpp
//
//   dumpDriverFeatures()
//
//   setPrinterProperty()
//
#include <windows.h>
#include <string>
#include <iostream>
#include <vector>

using std::string, std::endl, std::cout, std::cerr;
using std::vector, std::pair;

#include "utils.h"
#include "printerutils.h"

vector<name_value> dumpDriverFeatures( const string &driverKey, const string &feature )
{
	LSTATUS rc;
	vector<string> words;
	vector<name_value> pairs;

	HKEY key = NULL;
	if (ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, driverKey.c_str(), 
			0, KEY_READ, &key )) {

		if (verbose) cout << "Got the key..." << endl;

		DWORD data_size = 0, type = REG_BINARY;
		rc = RegQueryValueEx( key, feature.c_str(), NULL, &type, 
			NULL, &data_size );
		if (ERROR_SUCCESS == rc) {
			if (verbose) cout << "Got the value size = " << data_size << endl;

			BYTE value[data_size + 1];
			rc = RegQueryValueEx( key, feature.c_str(), NULL, &type, 
				value, &data_size );

			string valstring;
			vector<int> starting_points;
			for (ULONG i = 0, start = 0; i < sizeof(value); i++) {
				if (value[i] == '\n') {
					// skip newlines
				}
				else if (value[i] == 0) {
					if (valstring != "") {
						// cout << setw(2) << start << ": " << valstring << endl;
						starting_points.push_back( start );
						words.push_back( valstring );
					}
					start = i;
					valstring = "";
				}
				else {
					valstring += value[i];
				}
			}
			if (verbose) cout << "There were " << words.size() << " words" << endl;
			if (starting_points.size() != words.size()) {
				cerr << "Warning: starting points and word counts don't match" << endl;
			}
			if ((words.size() & 1) != 0) {
				words.pop_back();
			}
			for (auto s = words.begin(); s != words.end(); s++) {
				string this_name = *s++;
				pairs.push_back(name_value(this_name, *s));
			}

		}

		RegCloseKey( key );

	}
	else {
		cerr << "Can't open key, err = " << GetLastError() << endl;
	}

	return pairs;
}

// Set-PrinterProperty -PrinterName 'hplaser (HP LaserJet Professional P1606dn)' 
//            -PropertyName Config:DuplexUnit -Value TRUE

LSTATUS setPrinterProperty( const string &printer, const string &name, 
			const string &value )
{
	LSTATUS rc = ERROR_SUCCESS;

	string params = "Set-PrinterProperty -PrinterName '" + printer +
		"' -PropertyName " + name + " -Value " + value;

	SHELLEXECUTEINFO sei = {
		.cbSize = sizeof(SHELLEXECUTEINFO),
		.fMask = SEE_MASK_NOCLOSEPROCESS,
		.lpVerb = "open",
		.lpFile = "powershell.exe",
		.lpParameters = params.c_str(),
		.nShow = SW_HIDE,
	};


	if (verbose) {
		cout << name << " = " << value << endl;
		cout << sei.lpParameters << endl;
	}

	rc = ShellExecuteEx(&sei);

	WaitForSingleObject( sei.hProcess, INFINITE );
	DWORD exit_code = 0;
	GetExitCodeProcess( sei.hProcess, &exit_code );
	if (exit_code != ERROR_SUCCESS) {
		LSTATUS last_error = GetLastError();
		cerr << "A little problem with powershell : " << rc << 
			", GetLastError() = " << last_error << endl;
	}

	return rc;
}
