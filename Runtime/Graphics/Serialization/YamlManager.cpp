#include "YamlManager.h"

void YamlManager::LoadYamlFromFile(std::string path)
{
	YAML::Node Data = YAML::LoadFile(path);
}