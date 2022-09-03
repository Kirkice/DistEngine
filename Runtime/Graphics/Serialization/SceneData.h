#pragma once
#include "DX12Utils.h"
#include "Mathf.h"

using namespace Mathf;

struct SceneData
{
	/////////////////////////////
	///			主光源设置
	/////////////////////////////
	//	设为主光源
	bool isMainLight = true;
	//	灯光颜色
	float MainLightColor[4] = { 1, 0.9568627f, 0.8392157f, 1 };
	//	设置灯光位置
	float MainLightPosition[3] = { -20, 30, 20 };
	//	设置灯光欧拉角
	float MainLightEulerangle[3] = { 30, -60, -50 };
	//	设置forward
	float MainLightForward[3] = { 0.57f, -0.57f, 0.57f };
	//	主光源名字
	std::string MainLightName = "Direction Light";

	/////////////////////////////
	///			RenderObject
	/////////////////////////////
	std::string RenderObjectName = "ak47";
	int mMaterialType = 0;
	std::wstring MaterialPath = L"Asset/Materials/ak47.mat";
	std::wstring MeshPath = L"Asset/Mesh/ak47.obj";
	float RenderObjectPosition[3] = { 0, 0, 0 };
	float RenderObjectEulerAngle[3] = { 0, 0, 0 };
	float RenderObjectScale[3] = { 1, 1, 1 };
};

struct MaterialLitData
{
	std::string Name = "ak47_mat";
	int MatCBIndex = -1;
	int NumFramesDirty = gNumFrameResources;

	//	固有色
	float DiffuseColor[4] = { 1, 1, 1 , 1};
	//	粗糙度
	float Smoothness = 0.5f;
	//	金属度
	float Metallic = 0.0f;
	//	AO
	float Occlusion = 1.0f;
	//	自发光强度
	float EmissionStrength = 0.0f;
	//	自发光颜色
	float EmissionColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	//	Diffuse贴图Index
	UINT DiffuseMapIndex = 0;
	//	法线贴图Index
	UINT NormalMapIndex = 0;
	//	MSO贴图Index
	UINT MsoMapIndex = 0;
	//	自发光贴图Index
	UINT EmissionMapIndex = 0;
	//	LUT贴图Index
	UINT LUTMapIndex = 0;
};