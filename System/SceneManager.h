#pragma once
#include "../Component/MeshRender.h"
#include "../Component/DirectionLight.h"
#include "../Component/Camera.h"
#include "GraphicsUtils.h"
#include "GameTimer.h"
#include "d3dUtil.h"
#include "Texture2D.h"
#include "FrameResource.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class SceneManager
{
public:
	SceneManager(SceneType type);
	~SceneManager();

public:
	//	��������
	SceneType Type;

	//	����Դ
	DirectionLight mMainLight;

	//	Mesh Render	
	std::vector<std::unique_ptr<PBRMeshRender>> mMeshRender;

	//	��պ�����
	SkyBoxSetting mSkyBoxSetting;

	//	�ƹ�����
	LightSetting mLightSetting;

private:
	void BuildScene();
	void BuildDefaultScene();
	void BuildConelBoxScene();
	void BuildToonScene();
	void BuildWaterScene();
};
