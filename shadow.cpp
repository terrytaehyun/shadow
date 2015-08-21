#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <iostream>

using namespace std;

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