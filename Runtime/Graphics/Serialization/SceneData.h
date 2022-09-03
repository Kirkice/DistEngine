#pragma once
#include "DX12Utils.h"
#include "Mathf.h"

using namespace Mathf;

struct SceneData
{
	/////////////////////////////
	///			����Դ����
	/////////////////////////////
	//	��Ϊ����Դ
	bool isMainLight = true;
	//	�ƹ���ɫ
	float MainLightColor[4] = { 1, 0.9568627f, 0.8392157f, 1 };
	//	���õƹ�λ��
	float MainLightPosition[3] = { -20, 30, 20 };
	//	���õƹ�ŷ����
	float MainLightEulerangle[3] = { 30, -60, -50 };
	//	����forward
	float MainLightForward[3] = { 0.57f, -0.57f, 0.57f };
	//	����Դ����
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

	//	����ɫ
	float DiffuseColor[4] = { 1, 1, 1 , 1};
	//	�ֲڶ�
	float Smoothness = 0.5f;
	//	������
	float Metallic = 0.0f;
	//	AO
	float Occlusion = 1.0f;
	//	�Է���ǿ��
	float EmissionStrength = 0.0f;
	//	�Է�����ɫ
	float EmissionColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	//	Diffuse��ͼIndex
	UINT DiffuseMapIndex = 0;
	//	������ͼIndex
	UINT NormalMapIndex = 0;
	//	MSO��ͼIndex
	UINT MsoMapIndex = 0;
	//	�Է�����ͼIndex
	UINT EmissionMapIndex = 0;
	//	LUT��ͼIndex
	UINT LUTMapIndex = 0;
};