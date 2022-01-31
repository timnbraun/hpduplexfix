
#include <windows.h>
#include <winver.h>
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>

using std::string, std::vector, std::cout, std::endl;

#include "getversion.h"

#define TRACE printf
#define _T

bool GetProductAndVersion(string &strProductName, string &strProductVersion)
{
    // get the filename of the executable containing the version resource
    TCHAR szFilename[MAX_PATH + 1] = {0};
    if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0)
    {
        TRACE("GetModuleFileName failed with error %ld\n", GetLastError());
        return false;
    }

    // allocate a block of memory for the version info
    DWORD dummy;
    DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
    if (dwSize == 0)
    {
        TRACE("GetFileVersionInfoSize failed with error %ld\n", GetLastError());
        return false;
    }
    vector<BYTE> data(dwSize);

    // load the version info
    if (!GetFileVersionInfo(szFilename, 0, dwSize, &data[0]))
    {
        TRACE("GetFileVersionInfo failed with error %ld\n", GetLastError());
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
        TRACE("Can't obtain ProductName and ProductVersion from resources\n");
        return false;
    }

	// string length returned includes terminating 0
    strProductName = string((LPCSTR)pvProductName, iProductNameLen - 1);
    strProductVersion = string((LPCSTR)pvProductVersion, iProductVersionLen - 1);

    return true;
}
