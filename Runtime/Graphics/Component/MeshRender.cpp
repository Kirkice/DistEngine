#include "MeshRender.h"


MeshRender::MeshRender()
{
	this->material = Material();
	this->mesh = MeshFliter();
	this->bound = DistBound::BoundingBox();
}

MeshRender::MeshRender(Material mat, MeshFliter mesh, DistBound::BoundingBox bound)
{
	this->material = mat;
	this->mesh = mesh;
	this->bound = bound;
}

MeshRender::~MeshRender()
{

}