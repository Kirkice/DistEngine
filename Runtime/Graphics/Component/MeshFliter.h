/***********************************************************************************************
 ***				      G R A P H I C S C O R E  ---  D I S T E N G I N E                  ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : MeshFliter.h								   *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/12                                        *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once
#include "DX12Utils.h"
#include "Component.h";
#include "Mathf.h"

using namespace Mathf;

using uint16 = std::uint16_t;
using uint32 = std::uint32_t;

//	����
#include "../Geometry/GeometryStructures.h"



//	Mesh����


class MeshFliter : public Component
{
public:
	MeshData data;

public:
	MeshFliter(std::string _name);
	~MeshFliter();

	//	����Box
	void CreateBox(float width, float height, float depth, uint32 numSubdivisions);

	//	������
	void CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

	//	����Geo��
	void CreateGeosphere(float radius, uint32 numSubdivisions);

	//	����Բ����
	void CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

	//	������
	void CreateGrid(float width, float depth, uint32 m, uint32 n);

	//	����Quad
	void CreateQuad(float x, float y, float w, float h, float depth);

private:
	void Subdivide(MeshData& meshData);
	Vertex MidPoint(const Vertex& v0, const Vertex& v1);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
};