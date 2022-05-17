#pragma once
#ifndef MESH_RENDER_H
#define MESH_RENDER_H
#include "Transform.h"
#include "../Core/Singleton.h"
#include "../Core/Mathf/Mathf.h"
#include "Material.h"
#include "MeshFliter.h"
#include "BoundingBox.h"

// MeshRender
class MeshRender : public Transform
{
public:
	//	����
	Material material;
	//	����
	MeshFliter mesh;
	//	��Χ��
	DistBound::BoundingBox bound;


public:
	MeshRender();
	MeshRender(Material mat, MeshFliter mesh, DistBound::BoundingBox bound);
	~MeshRender();

};

#endif