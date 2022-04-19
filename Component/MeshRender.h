#pragma once
#ifndef MESH_RENDER_H
#define MESH_RENDER_H
#include "Transform.h"
#include "../Core/Core/Singleton.h"
#include "../Core/Mathf/Mathf.h"
#include "Material.h"
#include "MeshFliter.h"
#include "BoundingBox.h"

namespace Dist
{
	class PBRMeshRender : public Transform
	{
	public:
		//	²ÄÖÊ
		PBRMaterial material;
		//	Íø¸ñ
		MeshFliter mesh;
		//	°üÎ§ºÐ
		BoundingBox bound;


	public:
		PBRMeshRender();
		PBRMeshRender(PBRMaterial mat, MeshFliter mesh, BoundingBox bound);
		~PBRMeshRender();

	};
}

#endif