#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "../Core/Mathf/Color.h"
#include <string>
#include <unordered_map>

//	PBR材质
class Material
{
public:
	//	名字
	std::string Name;
	//	Index 值
	int MatCBIndex = -1;
	//	材质变化值
	int NumFramesDirty;
	//	固有色
	Color DiffuseColor;
	//	粗糙度
	float Smoothness;
	//	金属度
	float Metallic;
	//	AO
	float Occlusion;
	//	自发光强度
	float EmissionStrength;
	//	自发光颜色
	Color EmissionColor;

	// 纹理映射矩阵
	DirectX::XMFLOAT4X4 MatTransform;

	//	Diffuse贴图Index
	UINT DiffuseMapIndex;
	//	法线贴图Index
	UINT NormalMapIndex;
	//	MSO贴图Index
	UINT MsoMapIndex;
	//	自发光贴图Index
	UINT EmissionMapIndex;
	//	LUT贴图Index
	UINT LUTMapIndex;  



	//	固有色
	Color Tint;
	//	曝光度
	float Exposure;
	//	旋转
	float Rotation;
	//	ACES开启
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