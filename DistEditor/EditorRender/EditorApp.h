#pragma once
#include "../../System/InputSystem/InputSystem.h"
#include "../../Render/RenderApp.h"
#include "../EditorUtils/EditorUtils.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace Dist;
using namespace DirectX::PackedVector;
 

class EditorApp : public RenderApp
{
public:
	EditorApp(HINSTANCE hInstance);
	EditorApp(const EditorApp& rhs) = delete;
	EditorApp& operator=(const EditorApp& rhs) = delete;
	~EditorApp();

	virtual bool Initialize()override;

private:

	//	绘制
	virtual void Draw(const GameTimer& gt)override;

	bool show_app_dockspace = true;


	//	灯光设置
	bool show_lightSetting_panel = false;
	//	物理设置
	bool show_physicsSetting_panel = false;
	//	后处理设置
	bool show_postprocessingSetting_panel = false;


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

	//	绘制Debug
	void DrawProjectEditor();

	//	或者Overlay
	void DrawOverLayProfile();

	//	绘制灯光设置
	void DrawLightSettings();

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

	//	绘制性能分析
	void DrawProfileView();

	//	设置DockSpace
	void SetDockSpace(bool* p_open);
};