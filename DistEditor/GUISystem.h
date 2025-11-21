#pragma once
#include "GUIUtils.h"
#include "../Core/Mathf/Mathf.h"
#include "../Runtime/Graphics/Render/RenderCore.h"
#include "Component/PointLight.h"
#include "Component/SpotLight.h"

#include "../ThirdParty/ImGUI/imgui.h"
#include "../ThirdParty/ImGUI/imgui_impl_win32.h"
#include "../ThirdParty/ImGUI/imgui_impl_dx12.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct HierachyItem
{
	std::string Name = "";
	HierachyType type = HierachyType::MeshRender;
	bool selected = false;
	int MeshRenderIndex; 
};

class GUISystem : public DistRender::RenderCore
{
public:
	GUISystem(HINSTANCE hInstance);
	GUISystem(const GUISystem& rhs) = delete; 
	GUISystem& operator=(const GUISystem& rhs) = delete;
	~GUISystem();

	virtual bool Initialize()override;

private:

	virtual void Draw(const GameTimer& gt)override;

	virtual void Update(const GameTimer& gt)override;

	void UpdateGizmosObjectCBs();


	bool show_app_dockspace = true;

	bool show_app_overlay = true;

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

	//	�����������
	void DrawCameraEditor();

	//	����Debug
	void DrawProjectEditor();

	//	OverlayButton
	void DrawOverLayButton(bool* p_open);

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

	//	��ʼ�����������
	void InitHierachyItems(); 

private:
	std::vector<std::unique_ptr<HierachyItem>> mHierachyItems;
};
