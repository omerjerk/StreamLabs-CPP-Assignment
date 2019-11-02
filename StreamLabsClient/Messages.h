#pragma once

#include <windows.h>

#include <string>

using namespace std;

class Messages
{
public:
	static string getStringMessage(string str);
	static string getCreateClassMessage(string className);
	static string getCreateObjMessage(string className, string jsondata);
};

