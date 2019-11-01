#include "Messages.h"

#include <string>

using namespace std;

wstring Messages::getStringMessage(wstring str) {
	return (L"str " + str);
}

wstring Messages::getCreateClassMessage(wstring className) {
	return (L"ccc " + className);
}