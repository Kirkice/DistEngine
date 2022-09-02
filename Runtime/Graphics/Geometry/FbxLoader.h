#pragma once
#include <fbxsdk.h>
#include "../Core/Mathf/Mathf.h"
#include "../Component/MeshFliter.h"


class FbxLoader
{
public:
	static void LoadFbx(MeshData& data, const std::string filename);
};