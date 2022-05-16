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
	//	����
	PBRMaterial material;
	//	����
	MeshFliter mesh;
	//	��Χ��
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
	//	����
	SkyBoxMaterial material;
	//	����
	MeshFliter mesh;

public:
	SkyBoxMeshRender(); 
	SkyBoxMeshRender(SkyBoxMaterial mat, MeshFliter mesh);
	~SkyBoxMeshRender();
};


#endif