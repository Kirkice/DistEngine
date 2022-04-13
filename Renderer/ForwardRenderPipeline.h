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
		//	Light Setting Panel
		bool show_lightSetting_panel = false;
		bool show_physicsSetting_panel = false;
		bool show_postprocessingSetting_panel = false;

		virtual void Draw(const GameTimer& gt)override;

		//	ForwardRender
		void ForwardRender();

		//	DrawShadowMap
		void DrawShadowMap(ID3D12Resource* matBuffer);

		//	DrawDepthNormal
		void DrawDepthNormal();

		//	Draw SSAO
		void DrawSSAO(ID3D12Resource* matBuffer);

		//	DrawRenderTarget
		void DrawColorToTarget(ID3D12Resource* matBuffer);  

		//	DrawFinalBlit
		void DrawFinalBlit();

		////DrawOutLine
		void DrawOutLine();

		//DrawOpaque
		void DrawOpaque();

		//DrawSkyBox
		void DrawSkyBox();

		//DrawTransparent
		void DrawTransparent();

		//DrawPostProcessing
		void DrawRGBSplit(ID3D12Resource* matBuffer);
		void DrawRadialBlur(ID3D12Resource* matBuffer);
		void DrawVignette(ID3D12Resource* matBuffer);
		void DrawDecolor(ID3D12Resource* matBuffer);
		void DrawBrightness(ID3D12Resource* matBuffer);
		void DrawHSV(ID3D12Resource* matBuffer);
		void DrawMosaic(ID3D12Resource* matBuffer);
		void DrawSharpen(ID3D12Resource* matBuffer);
		void DrawSpherize(ID3D12Resource* matBuffer);
		void DrawWhiteBalance(ID3D12Resource* matBuffer);
		void DrawOilPaint(ID3D12Resource* matBuffer);
		void DrawRelief(ID3D12Resource* matBuffer);
		void DrawEdgeDetection(ID3D12Resource* matBuffer);

		void DrawBounding();

		void DrawGizmo();







		//DrawImgui
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

		void DrawLightSettings();

		void DrawPhysicsSettings();

		void DrawPostProcessingSettings();

		void SetDefaultStyle();

		void CopyColorPass();
	};
}