#pragma once

/*****
* Basic Utility Helper Functions
****/

// Error Macro
#define ERRORBOX(text, title) MessageBoxW(NULL, text, title, MB_OK | MB_ICONWARNING);

namespace util {
	// Safe delete dynamic memory
	template<typename T>
	void SafeDelete(T* ptr) {
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}

	// Safe delete dynamic memory array
	template<typename T>
	void SafeDeleteArr(T* ptrArr) {
		if (ptrArr) {
			delete ptrArr;
			ptrArr = nullptr;
		}
	}


}