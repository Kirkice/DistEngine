#pragma once
#ifndef MATERIAL_LIGHT_H
#define MATERIAL_LIGHT_H

#include "../Core/Core/Singleton.h"
#include "../Core/Mathf/Color.h"

namespace Dist
{
	class PBRMaterial
	{
	public:
		//	����ɫ
		Color DiffuseColor;
		//	�ֲڶ�
		float Smoothness;
		//	������
		float Metallic;
		//	AO
		float Occlusion;
		//	�Է���ǿ��
		float EmissionStrength;
		//	�Է�����ɫ
		Color EmissionColor;

		// ����ӳ�����
		DirectX::XMFLOAT4X4 MatTransform;

		//	Diffuse��ͼIndex
		UINT DiffuseMapIndex;
		//	������ͼIndex
		UINT NormalMapIndex;
		//	MSO��ͼIndex
		UINT MsoMapIndex;
		//	�Է�����ͼIndex
		UINT EmissionMapIndex;
		//	LUT��ͼIndex
		UINT LUTMapIndex;

	public:
		PBRMaterial();
		~PBRMaterial();
	};

}

#endif