#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <iostream>

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

UINT setMousePointerLocation()
{
	long displayWidth = GetSystemMetrics(SM_CXSCREEN); // The width of the screen of the PRIMARY monitor in pixel
	long displayHeight = GetSystemMetrics(SM_CYSCREEN);
	INPUT newInput = {0};

	// When dwFlags is set to MOUSEEVENTF_ABSOLUTE, value stored in dx and dy are normalized to 0 and 65535
	// 0,0 being the top-left corner and 65535,65535 being the bottom-right corner

	// Calculating how much 1 in normalized field is equivalent in pixel
	float fdx = displayWidth / 
	float fdy = displayHeight / 
	newInput.type = INPUT_MOUSE;
	newInput.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;

	


}

int main ()
{
	while(true)
	{
		generateLeftClick();
	}
	return 0;
}