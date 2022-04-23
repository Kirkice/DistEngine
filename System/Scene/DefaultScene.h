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
		//	����Դ
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

		//	��ʼ������
		void InitScene();

		//	���³���
		void UpdateScene();

	private:

		//	���������ƹ�
		void BuildLights();

		//	������ǩ��
		void BuildRootSignature();

		//	����SSAO��ǩ��
		void BuildSsaoRootSignature();

		//	����������
		void BuildDescriptorHeaps();

		//	������ɫ��
		void BuildShadersAndInputLayout();

		//	������Ⱦ��
		void BuildRenderItems();

		//	���µƹ�
		void UpdateLights(const GameTimer& gt);

		//	��������CB
		void UpdateObjectCBs(const GameTimer& gt);

		//	���²���
		void UpdateMaterials(const GameTimer& gt);

		//	������Ӱλ��
		void UpdateShadowTransform(const GameTimer& gt);

		//	����Pass CB
		void UpdateMainPassCB(const GameTimer& gt);

		//	����Shadow CB
		void UpdateShadowPassCB(const GameTimer& gt);

		//	����SSAO CB
		void UpdateSsaoCB(const GameTimer& gt);
	};
}
