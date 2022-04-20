#pragma once
#include <fbxsdk.h>
#include "../Core/Mathf/Mathf.h"
#include "../Component/MeshFliter.h"

namespace Dist
{
	class FbxLoader
	{
	public:
		MeshData LoadFbx(const std::string filename);
	};
}