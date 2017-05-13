#include "WindowClass.h"


// Default constructor
WindowClass::WindowClass() :
	hInstance(NULL),
	windowWidth(600),
	windowHeight(800),
	windowTitle(L"3D-Tetris"),
	szWindowClass(L"3DTetris") {
	ZeroMemory(&wcex, sizeof(wcex));
}

// Initializes and displays window
bool WindowClass::InitializeWindow(HINSTANCE hInstance, int nCmdShow) {
	this->hInstance = hInstance;
	// Size up window class (legacy)
	wcex.cbSize = sizeof(wcex);
	// Set style to vertical and horizontal redraw
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	// Set callback for window procedure
	wcex.lpfnWndProc = WndProc;
	// Set handle to the instance
	wcex.hInstance = hInstance;
	// Load default icon // TODO: Change to dope ass icon
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// Load small icon // TODO : Change to dope ass icon
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	// Set background to black (doesn't matter DirectX will override this)
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	// Set cursor to default // TODO: Change to custom dope ass cursor
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	// Set window class name
	wcex.lpszClassName = szWindowClass;

	// Register window class
	if (!RegisterClassExW(&wcex)) {
		ERRORBOX(L"Failed to register window class", L"3D-Tetris");
		return false;
	}

	// Window rectangle
	RECT rc = { 0, 0, windowHeight, windowWidth };
	// Adjusts rect to include scroll bar and title bar within the windowHeight and windowWidth
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	// Create the window and store the handle
	hWnd = CreateWindowExW(NULL,
		szWindowClass, // Window class
		windowTitle,  // Window title
		WS_OVERLAPPEDWINDOW,  // Window style
		CW_USEDEFAULT, CW_USEDEFAULT, // Window starting position
		(rc.right - rc.left), (rc.bottom - rc.top), // Window width and height
		NULL, // Parent window
		NULL, // Menu
		hInstance, // hInstance to process thread
		NULL); // Extra data

	// Check for error
	if (!hWnd) {
		ERRORBOX(L"Failed creating the window", L"3D-Tetris");
		return false;
	}

	// Show the window
	ShowWindow(hWnd, nCmdShow);

	// Return success
	return true;
}

// Default message loop
int WindowClass::MessageLoop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			// Translate keystrokes
			TranslateMessage(&msg);
			// Dispatch message to window procedure
			DispatchMessage(&msg);
		}
		else {
			// Do nothing...
		}
	}
	return (int)msg.wParam;
}

// Windows standard procedure callback
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
	// Paint structure
	PAINTSTRUCT ps;
	// Handle to device context
	HDC hdc;

	switch (uMessage) {
	case WM_PAINT:
		// Start paint call
		hdc = BeginPaint(hWnd, &ps);
		// Don't repaint but allows window to think its been repainted
		// All the repainting will be done by DirectX
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		// Let the standard procedures occur
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
	// Let system know we processed the call
	return 0;
}

// Default destructor
WindowClass::~WindowClass() {
}
