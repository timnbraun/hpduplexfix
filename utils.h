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
	DWORD count = GetConsoleProcessList(nullptr, 0);
	return count;
}

