#pragma once
#include "GUIUtils.h"
#include "../Core/Mathf/Mathf.h"
#include "RenderCore.h"

#include "../ThirdParty/ImGUI/imgui.h"
#include "../ThirdParty/ImGUI/imgui_impl_win32.h"
#include "../ThirdParty/ImGUI/imgui_impl_dx12.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class GUISystem : public RenderCore
{
public:
	GUISystem(HINSTANCE hInstance);
	GUISystem(const GUISystem& rhs) = delete;
	GUISystem& operator=(const GUISystem& rhs) = delete;
	~GUISystem();

private:

	virtual void Draw(const GameTimer& gt)override;

	bool show_app_dockspace = true;

	//	�ƹ�����
	bool show_lightSetting_panel = false;
	//	��������
	bool show_physicsSetting_panel = false;
	//	��������
	bool show_postprocessingSetting_panel = false;
	//	Frame Debugger
	bool show_frame_debugger_panel = false;

	//	��ʾ�ļ��н���
	bool show_folder_panel = true;
	//	��ʾ��������
	bool show_Textures_panel = false;
	//	��ʾMesh����
	bool show_Mesh_panel = false;
	//	��ʾ���ʽ���
	bool show_Materials_panel = false;
	//	��ʾ��������
	bool show_Scene_panel = false;

	//	����Editor
	void DrawEditor();

	//	�������˵�
	void DrawMenuEditor();

	//	���Ƴ����б�
	void DrawHierachyEditor();

	//	������Ⱦ��Ŀ
	void DrawInspectorEditor();

	//	����Debug
	void DrawProjectEditor();

	//	����Overlay
	void DrawOverLayProfile();

	//	���Ƶƹ�����
	void DrawLightSettings();

	//	������������
	void DrawPhysicsSettings();

	//	���ƺ�������
	void DrawPostProcessingSettings();

	//	���Ʒ��
	void SetDefaultStyle();

	//	����Scene��Game View
	void DrawSceneGameView();

	//	������Ⱦ����
	void DrawRenderSetting();

	//	����FrameDebugger
	void DrawFrameDebugger();

	//	�������ܷ���
	void DrawProfileView();

	//	����DockSpace
	void SetDockSpace(bool* p_open);
};