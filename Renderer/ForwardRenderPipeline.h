#pragma once
#include "Mathf.h"
#include "GraphicsCore.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Renderer
{
	class ForwardRenderer : public GraphicsCore
	{
	public:
		ForwardRenderer(HINSTANCE hInstance);
		ForwardRenderer(const ForwardRenderer& rhs) = delete;
		ForwardRenderer& operator=(const ForwardRenderer& rhs) = delete;
		~ForwardRenderer();

	private:
		virtual void Draw(const GameTimer& gt)override;

		//前项渲染
		void ForwardRender();

		//渲染阴影贴图
		void DrawShadowMap(ID3D12Resource* matBuffer);

		//渲染深度法线
		void DrawDepthNormal();

		////渲染外勾边
		void DrawOutLine();

		//渲染不透明物体
		void DrawOpaque();

		//渲染天空球
		void DrawSkyBox();

		//渲染透明物体
		void DrawTransparent();

		//渲染后处理
		void DrawPostProcessing();

		//渲染Imgui
		void DrawImgui();

		//Imgui - Editor
		void DrawEditor();

		//Imgui - Menu
		void DrawMenuEditor();

		//Imgui - GraphicsItem
		void DrawGraphicsItemEditor();

		//Imgui - RenderItem
		void DrawRenderItemEditor();

		//Imgui - Console
		void DrawConsoleEditor();

		//Imgui - Overlay Profile
		void DrawOverLayProfile();
	};
}