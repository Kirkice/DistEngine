#pragma once
#ifndef MATERIAL_LIGHT_H
#define MATERIAL_LIGHT_H

#include "../Core/Mathf/Color.h"

//	PBR����
class Material
{
public:
	//	����
	std::string Name;
	//	Index ֵ
	int MatCBIndex = -1;
	//	���ʱ仯ֵ
	int NumFramesDirty;
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



	//	����ɫ
	Color Tint;
	//	�ع��
	float Exposure;
	//	��ת
	float Rotation;
	//	ACES����
	float ACES;



public:
	Material();
	~Material();
};

#endif