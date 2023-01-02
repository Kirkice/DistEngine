#pragma once
#include "GUIUtils.h"
#include "../Core/Mathf/Mathf.h"
#include "Render/RenderCore.h"
#include "Component/PointLight.h"
#include "Component/SpotLight.h"

#include "../ThirdParty/ImGUI/imgui.h"
#include "../ThirdParty/ImGUI/imgui_impl_win32.h"
#include "../ThirdParty/ImGUI/imgui_impl_dx12.h"
#include "../ThirdParty/ImGUI/ImGuizmo.h"
#include "../ThirdParty/ImGUI/ImSequencer.h"
#include "../ThirdParty/ImGUI/ImZoomSlider.h"
#include "../ThirdParty/ImGUI/ImCurveEdit.h"
#include "../ThirdParty/ImGUI/GraphEditor.h"

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

class GUISystem : public RenderCore
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

	//	灯光设置
	bool show_lightSetting_panel = false;
	//	物理设置
	bool show_physicsSetting_panel = false;
	//	后处理设置
	bool show_postprocessingSetting_panel = false;
	//	Frame Debugger
	bool show_frame_debugger_panel = false;

	//	显示文件夹界面
	bool show_folder_panel = true;
	//	显示纹理界面
	bool show_Textures_panel = false;
	//	显示Mesh界面
	bool show_Mesh_panel = false;
	//	显示材质界面
	bool show_Materials_panel = false;
	//	显示场景界面
	bool show_Scene_panel = false;

	//	绘制Editor
	void DrawEditor();

	//	绘制主菜单
	void DrawMenuEditor();

	//	绘制场景列表
	void DrawHierachyEditor();

	//	绘制渲染项目
	void DrawInspectorEditor();

	//	绘制相机设置
	void DrawCameraEditor();

	//	绘制Debug
	void DrawProjectEditor();

	//	OverlayButton
	void DrawOverLayButton(bool* p_open);

	//	绘制物理设置
	void DrawPhysicsSettings();

	//	绘制后处理设置
	void DrawPostProcessingSettings();

	//	绘制风格
	void SetDefaultStyle();

	//	绘制Scene和Game View
	void DrawSceneGameView();

	//	绘制渲染设置
	void DrawRenderSetting();

	//	绘制FrameDebugger
	void DrawFrameDebugger();

	//	绘制性能分析
	void DrawProfileView();




	//	设置DockSpace
	void SetDockSpace(bool* p_open);

	//	初始化场景物件表
	void InitHierachyItems(); 

private:
	std::vector<std::unique_ptr<HierachyItem>> mHierachyItems;
};
