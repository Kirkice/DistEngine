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

		//	初始化
		virtual bool Initialize()override;
		//	绘制
		virtual void Draw(const GameTimer& gt)override;

	private:
		//	创建RTV SRV
		virtual void CreateRtvAndDsvDescriptorHeaps()override;
		//	重新设置 
		virtual void OnResize()override;
		//	更新
		virtual void Update(const GameTimer& gt)override;
		//	绘制渲染项目
		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<PBRRenderItem*>& ritems);

		//	绘制阴影
		void DrawShadowMap(ID3D12Resource* matBuffer);
		void DrawSceneToShadowMap();


	public:
		DefaultSceneRender m_SceneRender;
		ResourceManager mResourceManager{};
		float SolidColor[4] = { 0,0,0,0 };
	};
}