#pragma once

#include <windows.h>

#include <string>

using namespace std;

class Messages
{
public:
	static LPCTSTR getStringMessage(wstring str);
};

