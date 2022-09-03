#pragma once
#include <iostream>
#include "yaml-cpp/yaml.h"
#include <fstream>
#include "SceneData.h"

class YamlManager
{
public:
	static void LoadYamlFromFile(std::string path);
};