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

		//ǰ����Ⱦ
		void ForwardRender();

		//��Ⱦ��Ӱ��ͼ
		void DrawShadowMap(ID3D12Resource* matBuffer);

		//��Ⱦ��ȷ���
		void DrawDepthNormal();

		////��Ⱦ�⹴��
		void DrawOutLine();

		//��Ⱦ��͸������
		void DrawOpaque();

		//��Ⱦ�����
		void DrawSkyBox();

		//��Ⱦ͸������
		void DrawTransparent();

		//��Ⱦ����
		void DrawPostProcessing();

		//��ȾImgui
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