#include "Includes.h"
#include "WindowClass.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	WindowClass wc;
	if (!wc.InitializeWindow(hInstance, nCmdShow)) {
		ERRORBOX(L"Failed to Initialize Window", L"3D-Tetris");
		return -1;
	}
	return wc.MessageLoop();
}