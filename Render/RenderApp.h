#pragma once
#include "../System/InputSystem/InputSystem.h"
#include "RenderScene/DefaultSceneRender.h"
#include "../System/Scene/DefaultScene.h"
#include <ResourcesManager.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	class RenderApp : public InputSystem
	{

	public:
		RenderApp(HINSTANCE hInstance);
		RenderApp(const RenderApp& rhs) = delete;
		RenderApp& operator=(const RenderApp& rhs) = delete;
		~RenderApp();

		//	��ʼ��
		virtual bool Initialize()override;
		//	����
		virtual void Draw(const GameTimer& gt)override;

	private:
		//	����RTV SRV
		virtual void CreateRtvAndDsvDescriptorHeaps()override;
		//	�������� 
		virtual void OnResize()override;
		//	����
		virtual void Update(const GameTimer& gt)override;
		//	������Ⱦ��Ŀ
		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<PBRRenderItem*>& ritems);

		//	������Ӱ
		void DrawShadowMap(ID3D12Resource* matBuffer);
		void DrawSceneToShadowMap();


	public:
		DefaultSceneRender m_SceneRender;
		ResourceManager mResourceManager{};
		float SolidColor[4] = { 0,0,0,0 };
	};
}