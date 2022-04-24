#pragma once
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "../Component/MeshFliter.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Dist
{
	class ObjLoader
	{
	public:
		MeshData LoadObj(const char* filename);
	};
}


