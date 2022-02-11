//
// utils.h
//
//    verbose - global
//    consoleListLength()
//
//  Copyright (c) 2022 Tim Braun <tim.n.braun@gmail.com>
//


extern bool verbose;

inline DWORD consoleListLength()
{
	DWORD bogusList = 0;
	DWORD count = GetConsoleProcessList(&bogusList, 1);
	return count;
}

// This doesn't seem to be useful for us
inline bool ownTheConsole()
{
    HWND consoleWnd = GetConsoleWindow();
    DWORD dwProcessId = 0;

    GetWindowThreadProcessId(consoleWnd, &dwProcessId);

	return GetCurrentProcessId() == dwProcessId;
}
