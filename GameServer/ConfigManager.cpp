#include "stdafx.h"
#include "ConfigManager.h"
#include <fstream>
#include <json/json.h>

std::shared_ptr<ConfigManager> gConfigManager = MakeShared<ConfigManager>();

bool ConfigManager::Init(std::string_view projectPath, std::string_view fileName)
{
	ProjectPath = projectPath;

	ostringstream oss;
	oss << ProjectPath;
	oss <<  "\\" << fileName;

	std::ifstream t;
	t.open(oss.str());
	
	Json::Reader reader;
	Json::Value root;
	
	if (!reader.parse(t, root)) {

		cout << "Parsing Failed!!" << endl;
		return false;
	}

	Configs["db"] = root["db"];
	Configs["redis"] = root["redis"];

	cout << "Configuration File Name Is " << oss.str() << endl;

	return true;
}
