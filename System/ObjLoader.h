#pragma once
#include "tiny_obj_loader.h"
#include "../Component/MeshFliter.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>


class ObjLoader
{
public:
	static void LoadObj(MeshData& data, char* filename);
	static void ReconStructNormals(MeshData& data);
	static void ComputeTangentSpace(MeshData& data);
};


