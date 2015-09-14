#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500 // For defining version of the windows header(<windows.h>) files to use. Must be declared before including it.
#endif
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <tlhelp32.h> // for PROCESSENTRY32

// Thing to check out: why use DWORD? why would unsign long would have different range in different compiler?

using namespace std;

// http://www.cplusplus.com/forum/lounge/17053/
UINT generateLeftClick()
{
	INPUT newInput = {0};

	// Generate left click down
	newInput.type = INPUT_MOUSE;
	newInput.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &newInput, sizeof(INPUT)); // SendInput(<Number of structures in LPINPUT array>, <Array of INPUT structures>, <Size of INPUT Structure>)

	// Using ZeroMemory macro to clear up newInput to be used in MOUSEEVENTF_LEFTUP
	ZeroMemory(&newInput, sizeof(INPUT));

	// Generate left click up
	newInput.type = INPUT_MOUSE;
	newInput.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &newInput, sizeof(INPUT));
}

POINT getMousePointerLocation()
{
	POINT mousePoint;
	GetCursorPos(&mousePoint);

	return mousePoint;
}

UINT setMousePointerLocation(POINT& mousePoint)
{
	long displayWidth = GetSystemMetrics(SM_CXSCREEN); // The width of the screen of the PRIMARY monitor in pixel
	long displayHeight = GetSystemMetrics(SM_CYSCREEN);
	INPUT newInput = {0};

	// When dwFlags is set to MOUSEEVENTF_ABSOLUTE, value stored in dx and dy are normalized to 0 and 65535
	// 0,0 being the top-left corner and 65535,65535 being the bottom-right corner

	// Need to normalize the raw values (by multiplying targetMax / sourceMax)
	float fdx = mousePoint.x * (65535.0f/displayWidth); //.0f required for more precision
	float fdy = mousePoint.y * (65535.0f/displayHeight);

	// Preparing to set the mouse point
	newInput.type = INPUT_MOUSE;
	newInput.mi.dwFlags = MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE;
	newInput.mi.dx = (long)fdx;
	newInput.mi.dy = (long)fdy;

	SendInput(1, &newInput, sizeof(INPUT));

}

// LPCSTAR = const char*:http://programmers.stackexchange.com/questions/194764/what-is-lpctstr
// sz appended to beginning of variable indicate that the variable is string of characters, terminated by a null character. (z standing for zero)
DWORD FindProcessID(LPCTSTR szProcName)
{
	// http://stackoverflow.com/questions/2995251/why-in-c-do-we-use-dword-rather-than-unsigned-int
	// DWORD: SDK devs prefer to define their own types using typedef, allowing them to change underlying types only in one place.
	// DWORD is type defined in windows.h which is currently uses unsigned int
	DWORD dwPID = 0xFFFFFFFF;

	// What is HANDLE : an abstraction that hides real memory address from the API user, so the system can reorganize physical memory at will.
	// Basically an index into a table of pointers.
	// http://stackoverflow.com/questions/902967/what-is-a-windows-handle
	HANDLE hProcSnapShot = INVALID_HANDLE_VALUE;

	// PROCESSENTRY32 == Describes an entry from a list of the process residing in the system address sace when a snapshot was taken
	PROCESSENTRY32 pe;

	/* Getting the snapshot of the system */
	// Size of the structure. Must be set to sizeof(PROCESSENTRY32) before calling Process32First. If it's not initialized, Process32First fails
	pe.dwSize = sizeof(PROCESSENTRY32); 
	// CreateToolhelp32Snapshot 
	//   first arg: dwFlags, used to indicate portions of the system to be included in the snapshot.
	//   second arg: th32ProcessID, process identifier of the process to be included in the snapshot. Zero means to indicate the current process
	//   TH32CS_SNAPALL == Includes all processes and threads in the system, plus the heaps and modules of the process specified in th32ProcessID(second arg).
	hProcSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0); // DEBUG changed from NULL

	/* start finding process */
	// Process32First - should be called after CreateToolhelp32Snapshot()
	//   Retrieves informatino about the first process encountered in a system snapshot
	//   RETURNS TRUE if the first entry of the  process list has been copied to the buffer.
	if (Process32First(hProcSnapShot, &pe) == FALSE)
	{
		perror("Could not copy first entry of the process list!\n");
	}

	do
	{
		// _stricmp == compares str1 with str2 after converting the strings to lowercase characters. Retruns zero if strings are equal.
		//		Has fallback when used with uppercase + _. https://msdn.microsoft.com/en-us/library/k59z8dwe.aspx
		// szExeFile == The name of the executable file for the process. (Can also retrieve full path by using Module32First)
		if (!_stricmp(szProcName, pe.szExeFile))
		{
			dwPID = pe.th32ProcessID; // Process ID of the found process
			break;
		}
	} while (Process32Next(hProcSnapShot, &pe));

	CloseHandle(hProcSnapShot);

	return dwPID;

}

// Makes the target process to start our .dll file using LoadLubrary("MY_DLL.dll")
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllName)
{
	HANDLE hProcess, hThread;
	HMODULE hMod;
	LPVOID pRemoteBuf;
	DWORD dwBufSize = lstrlen(szDllName) + 1; // +1 for including NULL character
	LPTHREAD_START_ROUTINE pThreadProc;

	// 1) Using dwPID, retrieve handle of the target process
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		return FALSE;
	}
		
	// 2) Allocate memory with size required by szDllName, into the target process
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

	// 3) Write content of .dll file to the allocated memory
	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllName, dwBufSize, NULL); // pRemoteBuf now holds memory address that hProcess points to, which is our target Process

	// 4) Get address of LoadLibraryA() API
	hMod = GetModuleHandle("kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibrarayA");

	// 5) Start the thread at the target Process
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;

}

int main ()
{

	// POINT& mousePoint = new POINT();
	POINT currentCursorPosition = getMousePointerLocation();

	cout<< "current Position is: (" << currentCursorPosition.x << ", " <<currentCursorPosition.y << ")" <<endl;
	currentCursorPosition.x = 2107;
	currentCursorPosition.y = 619;

	while(true)
	{
		setMousePointerLocation(currentCursorPosition);
		// generateLeftClick();
	}
	return 0;
}