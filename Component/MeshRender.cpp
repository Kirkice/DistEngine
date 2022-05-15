#include "MeshRender.h"


PBRMeshRender::PBRMeshRender()
{
	this->material = PBRMaterial();
	this->mesh = MeshFliter();
	this->bound = BoundingBox();
}

PBRMeshRender::PBRMeshRender(PBRMaterial mat, MeshFliter mesh, BoundingBox bound)
{
	this->material = mat;
	this->mesh = mesh;
	this->bound = bound;
}

PBRMeshRender::~PBRMeshRender()
{

}