#include "Includes.h"
#include "WindowClass.h"
#include "GameClass.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	WindowClass wc;
	GameClass gc;
	if (!wc.InitializeWindow(hInstance, nCmdShow)) {
		ERRORBOX(L"Failed to Initialize Window", L"3D-Tetris");
		return -1;
	}
	return wc.MessageLoop();
}