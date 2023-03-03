#pragma once
#include <json/json.h>

class ConfigManager
{
public:
	bool Init(std::string_view projectPath, std::string_view fileName);
	Json::Value GetJson(std::string_view key) { return Configs[key.data()]; }

	std::string ProjectPath;
	std::string ConfigPath;
	Map < std::string, Json::Value > Configs;
};


extern std::shared_ptr<ConfigManager> gConfigManager;