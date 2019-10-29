#include "Messages.h"

#include "windows.h"

LPCTSTR Messages::getStringMessage(wstring str) {
	wstring res = L"str " + str;
	return res.c_str();
}