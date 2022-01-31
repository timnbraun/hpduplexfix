//
// gethpduplex
//
//   Show whether the duplexer is available on my hp printer
//
//   Option to call powershell to enable duplexing
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
#include "getversion.h"

const string buildDate = BUILD_DATE;

typedef pair<string, string> name_value;

bool verbose = false;

vector<name_value> dumpDriverFeatures( const string key );
LSTATUS SetPrinterProperty( string printer, string name, string value );
void enableDuplex();

LSTATUS setDriverFeatures( const string driverKey, const vector<name_value> );
vector<UCHAR> buildBlob( vector<name_value> pairs );
void printHexBlob( const vector<BYTE> &blob );

int main(int argc, char *argv[])
{
	int rc = 0;
	string program = path(argv[0]).stem().string();

	if (program == "sethpduplex" ||
		(argc == 2 && string(argv[1]) == "--set")) {

		if (!IsUserAnAdmin()) {
			cout << "This may not work - you're not in admin group" << endl;
		}
		enableDuplex();
	}
	else {
		if (argc == 2 && string(argv[1]) == "--version") {
			string product, version;

			GetProductAndVersion( product, version );
			cout << product << " V" << version << " " << buildDate << endl;

			exit(0);
		}

		vector<name_value> pairs;
		pairs = dumpDriverFeatures( DriverKey );
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

vector<name_value> dumpDriverFeatures( const string driverKey )
{
	LSTATUS rc;
	vector<string> words;
	vector<name_value> pairs;

	HKEY key = NULL;
	if (ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, driverKey.c_str(), 
			0, KEY_READ, &key )) {

		if (verbose) cout << "Got the key..." << endl;

		DWORD data_size = 0, type = REG_BINARY;
		rc = RegQueryValueEx( key, FeatureKeyword.c_str(), NULL, &type, 
			NULL, &data_size );
		if (ERROR_SUCCESS == rc) {
			if (verbose) cout << "Got the value size = " << data_size << endl;

			BYTE value[data_size + 1];
			rc = RegQueryValueEx( key, FeatureKeyword.c_str(), NULL, &type, 
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


void enableDuplex()
{
#if 0
	vector<name_value> pairs = { { "DuplexUnit", "TRUE" }, { "Memory", "32768KB" } };
	for (const auto &p : pairs) {
		if (verbose) cout << ' ' << p.first << " = " << p.second << endl;
	}

	cout << "CurrentControlset:" << endl;
	if (setDriverFeatures( DriverKey, pairs ) != ERROR_SUCCESS) {
		return;
	}

	cout << endl << "WOW6432Node:" << endl;
	setDriverFeatures( DriverKey64, pairs );
#endif
	SetPrinterProperty( PrinterName, PropertyName, "TRUE" );
}


//
//
// build the blob for the desired features,
// Get the current features,
// if the blob is the same, you're done
// else set the new blob
// 
//
LSTATUS setDriverFeatures( const string driverKey, const vector<name_value> pairs)
{
	LSTATUS rc;
	vector<string> words;

	// verbose = true;

	vector<UCHAR> blob;
	blob = buildBlob( pairs );

	HKEY key = NULL;
	rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE, driverKey.c_str(), 
			0, KEY_QUERY_VALUE, &key );
	if (ERROR_SUCCESS == rc) {

		if (verbose) cout << "Got the key..." << endl;

		DWORD data_size = 0, type = REG_BINARY;
		rc = RegQueryValueEx( key, FeatureKeyword.c_str(), NULL, &type, 
			NULL, &data_size );
		if (ERROR_SUCCESS == rc) {
			if (verbose) cout << "Got the value size = " << data_size << endl;

			vector<BYTE> old_value(data_size);
			rc = RegQueryValueEx( key, FeatureKeyword.c_str(), 0, &type, 
				old_value.data(), &data_size );

			if (verbose) { cout << "Current "; printHexBlob( old_value ); }
			// vector<UCHAR> old_blob;
			// old_blob.assign( value.begin(), value.end() );

			if (old_value != blob) {
				// printHexBlob( old_value );

				string valstring;
				vector<int> starting_points;
				for (ULONG i = 0, start = 0; i < old_value.size(); i++) {
					if (old_value[i] == '\n') {
						// skip newlines
					}
					else if (old_value[i] == 0) {
						if (valstring != "") {
							// cout << setw(2) << start << ": " << valstring << endl;
							starting_points.push_back( start );
							words.push_back( valstring );
						}
						start = i;
						valstring = "";
					}
					else {
						valstring += old_value[i];
					}
				}
				if (verbose) cout << "There were " << words.size() << " words" << endl;
				if (starting_points.size() != words.size()) {
					cerr << "Warning: starting points and word counts don't match" << endl;
				}
				if ((words.size() & 1) != 0) {
					words.pop_back();
				}

				vector<name_value> old_pairs;
				for (auto s = words.begin(); s != words.end(); s++) {
					string this_name = *s++;
					old_pairs.push_back(name_value(this_name, *s));
				}

				cout << "Old pairs:" << endl;
				for (const auto &p : old_pairs) {
					cout << ' ' << p.first << " = " << p.second << endl;
				}

				// Now to set the new value
				rc = RegSetValueEx( key, FeatureKeyword.c_str(), 0, REG_BINARY, 
					blob.data(), blob.size() );
				if (rc != ERROR_SUCCESS) {
					cerr << "Can't set key value, rc = " << rc << ", err = " 
						<< GetLastError() << endl;
				}
			}
		}

		RegCloseKey( key );
	}
	else {
		cerr << "Can't open key, rc = " << rc << ", err = " << GetLastError() << endl;
	}

	return rc;
}

void printHexBlob( const vector<BYTE> &blob )
{
	vector<BYTE> hexBlob = {};

	// Convert to hex encoding
	for (const auto &b : blob) {
		char hex[3];
		sprintf( hex, "%02X", b);
		hexBlob.push_back(hex[0]);
		hexBlob.push_back(hex[1]);
	}
	hexBlob.push_back( 0 );

	cout << "Blob: " << hexBlob.data() << endl;
}

// Build the binary blob - 
//   zero terminated strings, 
//   \n following pairs, 
//   two zeros at the end
vector<UCHAR> buildBlob( vector<name_value> pairs )
{
	vector<UCHAR> blob;
	for (const auto &p : pairs) {
		for (const auto &c : p.first) {
			blob.push_back( c );
		}
		blob.push_back( 0 );
		for (const auto &c : p.second) {
			blob.push_back( c );
		}
		blob.push_back( 0 );
		blob.push_back('\n');
	}
	blob.push_back( 0 );
	blob.push_back( 0 );

	if (verbose) {
		printHexBlob( blob );
	}
	return blob;
}


// Set-PrinterProperty -PrinterName 'hplaser (HP LaserJet Professional P1606dn)' 
//            -PropertyName Config:DuplexUnit -Value TRUE

LSTATUS SetPrinterProperty( string printer, string name, string value )
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

	cout << name << " = " << value << endl;

	if (verbose) cout << sei.lpParameters << endl;

	rc = ShellExecuteEx(&sei);

	WaitForSingleObject( sei.hProcess, INFINITE );
	DWORD exit_code = 0;
	GetExitCodeProcess( sei.hProcess, &exit_code );
	if (exit_code != ERROR_SUCCESS) {
		LSTATUS last_error = GetLastError();
		cerr << "A little problem with powershell : " << rc << ", GetLastError() = " <<
			last_error << endl;
	}

	return rc;
}
