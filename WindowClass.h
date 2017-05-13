#pragma once
#include "Includes.h"


/******
* Window class for the basic WIN32 window
****/
class WindowClass {
protected:
	// Window Width (in px)
	int windowWidth;
	// Window Height (in px)
	int windowHeight;
	// Window register class
	WNDCLASSEXW wcex;
	// Window title
	LPCWSTR windowTitle;
	// Windows instance assigned by program manager
	HINSTANCE hInstance;
	// Window class name
	LPCWSTR szWindowClass;
	// Handle to the window
	HWND hWnd;
public:
	// Default constructor
	WindowClass();
	// Initializes and displays window
	bool InitializeWindow(HINSTANCE hInstance, int nCmdShow);
	// Default message loop
	int MessageLoop();
	// Default destructor
	~WindowClass();

	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

// Windows standard procedure callback