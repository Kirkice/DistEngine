#pragma once
#ifndef MESH_RENDER_H
#define MESH_RENDER_H
#include "Transform.h"
#include "../Core/Singleton.h"
#include "../Core/Mathf/Mathf.h"
#include "Material.h"
#include "MeshFliter.h"
#include "BoundingBox.h"

//	PBR MeshRender
class PBRMeshRender : public Transform
{
public:
	//	材质
	PBRMaterial material;
	//	网格
	MeshFliter mesh;
	//	包围盒
	DistBound::BoundingBox bound;


public:
	PBRMeshRender();
	PBRMeshRender(PBRMaterial mat, MeshFliter mesh, DistBound::BoundingBox bound);
	~PBRMeshRender();

};

//	SkyBoxMeshRender
class SkyBoxMeshRender : public Transform
{
public:
	//	材质
	SkyBoxMaterial material;
	//	网格
	MeshFliter mesh;

public:
	SkyBoxMeshRender(); 
	SkyBoxMeshRender(SkyBoxMaterial mat, MeshFliter mesh);
	~SkyBoxMeshRender();
};


#endif