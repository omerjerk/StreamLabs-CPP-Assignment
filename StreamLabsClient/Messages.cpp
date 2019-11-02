#include "Messages.h"

#include <string>

#include <json.hpp>

using namespace std;
using json = nlohmann::json;

string Messages::getStringMessage(string str) {
	return ("str " + str);
}

string Messages::getCreateClassMessage(string className) {
	return ("ccc " + className);
}

string Messages::getCreateObjMessage(string className, string jsondata) {
	auto data = nlohmann::json::parse(jsondata);
	json mainobj;
	mainobj["classname"] = className;
	mainobj["obj"] = data;
	return ("cob " + mainobj.dump());
}