#include "MeshRender.h"


PBRMeshRender::PBRMeshRender()
{
	this->material = PBRMaterial();
	this->mesh = MeshFliter();
	this->bound = DistBound::BoundingBox();
}

PBRMeshRender::PBRMeshRender(PBRMaterial mat, MeshFliter mesh, DistBound::BoundingBox bound)
{
	this->material = mat;
	this->mesh = mesh;
	this->bound = bound;
}

PBRMeshRender::~PBRMeshRender()
{

}

SkyBoxMeshRender::SkyBoxMeshRender()
{
	this->material = SkyBoxMaterial();
	this->mesh = MeshFliter();
}

SkyBoxMeshRender::SkyBoxMeshRender(SkyBoxMaterial mat, MeshFliter mesh)
{
	this->material = mat;
	this->mesh = mesh;
}

SkyBoxMeshRender::~SkyBoxMeshRender()
{

}