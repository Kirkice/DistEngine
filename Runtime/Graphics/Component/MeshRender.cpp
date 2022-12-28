/***********************************************************************************************
 ***						W I N A P P  ---  D I S T E N G I N E                            ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : MeshRender.h								   *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/12                                        *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "MeshRender.h"


MeshRender::MeshRender(std::string _name = "MeshRender") : Component(_name)
{
	this->mesh = &MeshFliter("Mesh Fliter");
	this->mat = &Material();
}

MeshRender::MeshRender(
	MeshFliter* mesh,
	Material* mat,
	std::string _name = "MeshRender") : Component(_name)
{
	this->mesh = mesh;
	this->mat = mat;
}

MeshRender::~MeshRender()
{

}

void MeshRender::Draw(const ComPtr<ID3D12GraphicsCommandList>& pCmdList, bool bundleUsingOverride = false)
{

}