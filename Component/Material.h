#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "../Core/Mathf/Color.h"
#include <string>
#include <unordered_map>

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


class MaterialIndexUtils
{
public:

	MaterialIndexUtils() = default;
	~MaterialIndexUtils() = default;

	static MaterialIndexUtils& getInstance() {
		static MaterialIndexUtils instance;
		return instance;
	}

public:

	void Init();

	int GetIndex();
	void OffsetIndex();

	void SaveTypeIndex(std::string type, int Index);

	int GetTypeIndexStart(std::string type);

private:
	int Index = -1;

	std::unordered_map<std::string, int> mMatTypeIndexMap;
};
#endif