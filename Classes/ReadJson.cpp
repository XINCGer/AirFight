#include "ReadJson.h"

std::string ReadJson::getString(std::string name){
	Json::Reader reader;
	Json::Value root;

	std::string data = FileUtils::getInstance()->getStringFromFile("language.json");

	if (reader.parse(data, root, false) == true)
	{
		return root[name].asCString();
	}
}