#include "Includes.h"
#include "WindowClass.h"
#include "GameClass.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	//WindowClass wc;
	GameClass gc;
	if (!gc.InitializeWindow(hInstance, nCmdShow)) {
		ERRORBOX(L"Failed to Initialize Window", L"3D-Tetris");
		return -1;
	}
	if (!gc.InitD3D()) {
		ERRORBOX(L"Failed to Initialize Direct3D", L"3D-Tetris");
		return -1;
	}
	
	return gc.MessageLoop();
}