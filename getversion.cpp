//
// getversion.cpp
//
//   Get the product name and version from the VERSIONINFO structure
//
//  Copyright (c) 2022 Tim Braun <tim.n.braun@gmail.com>
//

#include <windows.h>
#include <winver.h>
#include <string>
#include <iostream>
#include <vector>

using std::string, std::vector, std::cerr, std::endl;

#include "getversion.h"

#define _T

bool getProductAndVersion(string &strProductName, string &strProductVersion)
{
    // get the filename of the executable containing the version resource
    TCHAR szFilename[MAX_PATH + 1] = {0};
    if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0)
    {
        cerr << "GetModuleFileName failed with error " << GetLastError() << endl;
        return false;
    }

    // allocate a block of memory for the version info
    DWORD dummy;
    DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
    if (dwSize == 0)
    {
        cerr << "GetFileVersionInfoSize failed with error " << GetLastError() << endl;
        return false;
    }
    vector<BYTE> data(dwSize);

    // load the version info
    if (!GetFileVersionInfo(szFilename, 0, dwSize, &data[0]))
    {
        cerr << "GetFileVersionInfo failed with error " << GetLastError() << endl;
        return false;
    }

    // get the name and version strings
    LPVOID pvProductName = NULL;
    UINT iProductNameLen = 0;
    LPVOID pvProductVersion = NULL;
    UINT iProductVersionLen = 0;

    // replace "040904e4" with the language ID of your resources
    if (!VerQueryValue(&data[0], _T("\\StringFileInfo\\040904e4\\ProductName"), &pvProductName, &iProductNameLen) ||
        !VerQueryValue(&data[0], _T("\\StringFileInfo\\040904e4\\ProductVersion"), &pvProductVersion, &iProductVersionLen))
    {
        cerr << "Can't obtain ProductName and ProductVersion from resources" << endl;
        return false;
    }

	// string length returned includes terminating 0
    strProductName = string((LPCSTR)pvProductName, iProductNameLen - 1);
    strProductVersion = string((LPCSTR)pvProductVersion, iProductVersionLen - 1);

    return true;
}
