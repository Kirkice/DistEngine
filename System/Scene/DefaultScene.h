#pragma once
#include "../../Component/MeshRender.h"
#include "../../Component/DirectionLight.h"
#include "../../Component/Camera.h"
#include "../EngineSystem/GameTimer.h"
#include <string.h>

namespace Dist
{
	struct DefaultSceneStruct
	{
		//	主光源
		DirectionLight mMainLight;

		//	Mesh Render	
		std::vector<PBRMeshRender> mMeshRender;

		//	Camera
		Camera mCamera;

		//	SkyBox Textures
		std::wstring mSkyBoxTexPath;
	};

	class DefaultScene
	{
		DefaultSceneStruct mScene;

	public:
		DefaultScene();
		~DefaultScene();

		//	初始化场景
		void InitScene();

		//	更新场景
		void UpdateScene();

	private:

		//	构建场景灯光
		void BuildLights();

		//	构建根签名
		void BuildRootSignature();

		//	构建SSAO根签名
		void BuildSsaoRootSignature();

		//	构建描述符
		void BuildDescriptorHeaps();

		//	构建着色器
		void BuildShadersAndInputLayout();

		//	构建渲染项
		void BuildRenderItems();

		//	更新灯光
		void UpdateLights(const GameTimer& gt);

		//	更新物体CB
		void UpdateObjectCBs(const GameTimer& gt);

		//	更新材质
		void UpdateMaterials(const GameTimer& gt);

		//	更新阴影位置
		void UpdateShadowTransform(const GameTimer& gt);

		//	更新Pass CB
		void UpdateMainPassCB(const GameTimer& gt);

		//	更新Shadow CB
		void UpdateShadowPassCB(const GameTimer& gt);

		//	更新SSAO CB
		void UpdateSsaoCB(const GameTimer& gt);
	};
}
