#include "GUISystem.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

GUISystem::GUISystem(HINSTANCE hInstance) : RenderCore(hInstance)
{
}

GUISystem::~GUISystem()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

void GUISystem::Draw(const GameTimer& gt)
{
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());
	ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));

	RenderCore::Draw(gt);

	DrawEditor();

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	mCurrFrameResource->Fence = ++mCurrentFence;

	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

//	DrawEditor
void GUISystem::DrawEditor()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

	//	Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	//	Draw Stylize
	SetDefaultStyle();
	SetDockSpace(&show_app_dockspace);

	//	Scene
	DrawSceneGameView();

	//	Draw MainMenu
	if (show_lightSetting_panel)
		DrawLightSettings();
	if (show_physicsSetting_panel)
		DrawPhysicsSettings();
	if (show_postprocessingSetting_panel)
		DrawPostProcessingSettings();
	if (show_frame_debugger_panel)
		DrawFrameDebugger();

	//	Draw Inspector
	DrawInspectorEditor();

	//	Draw Hierachy
	DrawHierachyEditor();

	//	Project
	DrawProjectEditor();

	//	Render
	DrawRenderSetting();

	//	Profile
	DrawProfileView();

	mCommandList->SetDescriptorHeaps(1, mSrvDescriptorHeap.GetDescriptorHeap().GetAddressOf());
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());
}

//	设置DockSpace
void GUISystem::SetDockSpace(bool* p_open)
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
	ImGui::PopStyleVar();

	ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	//Draw Menu
	GUISystem::DrawMenuEditor();

	ImGui::End();
}

//	DrawMenuEditor
void GUISystem::DrawMenuEditor()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Light Settings", NULL, &show_lightSetting_panel);
			ImGui::MenuItem("Physics Settings", NULL, &show_physicsSetting_panel);
			ImGui::MenuItem("PostProcessing Settings", NULL, &show_postprocessingSetting_panel);
			ImGui::MenuItem("Frame Debugger", NULL, &show_frame_debugger_panel);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

//	DrawHierachyEditor
void GUISystem::DrawHierachyEditor()
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(320, mClientHeight), ImGuiCond_FirstUseEver);

	ImGui::Begin("Hierachy");

	ImGui::End();
}

//	DrawInspectorEditor
void GUISystem::DrawInspectorEditor()
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x + mClientWidth - 300, main_viewport->WorkPos.y), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(300, mClientHeight), ImGuiCond_FirstUseEver);

	//Render Item
	ImGui::Begin("Inspector");

	if (ImGui::CollapsingHeader("Transform"))
	{
	}
	if (ImGui::CollapsingHeader("Material"))
	{
	}
	ImGui::End();
}

//	DrawConsoleEditor
void GUISystem::DrawProjectEditor()
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x + 320, main_viewport->WorkPos.y + mClientHeight - 320), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(mClientWidth - 620, 320), ImGuiCond_FirstUseEver);

	ImGui::Begin("Project");


	if (show_folder_panel)
	{
		//	显示纹理界面
		show_Textures_panel = false;
		//	显示Mesh界面
		show_Mesh_panel = false;
		//	显示材质界面
		show_Materials_panel = false;
		//	显示场景界面
		show_Scene_panel = false;


		//	显示文件夹
		GUIUtils::DrawProjetcFolder(
			md3dDevice,
			mSrvDescriptorHeap.GetDescriptorHeap(),
			mIconTextures, 
			&show_folder_panel, 
			&show_Textures_panel, 
			&show_Mesh_panel, 
			&show_Materials_panel, 
			&show_Scene_panel
		);
	}
	else if (show_Textures_panel)
	{
		//	显示文件夹界面
		show_folder_panel = false;
		//	显示Mesh界面
		show_Mesh_panel = false;
		//	显示材质界面
		show_Materials_panel = false;
		//	显示场景界面
		show_Scene_panel = false;

		//	显示纹理界面
		GUIUtils::DrawProjectTextures(
			md3dDevice, 
			mSrvDescriptorHeap.GetDescriptorHeap(),
			mIconTextures,
			mResourcesTextures,
			mClientWidth - 620, 
			&show_folder_panel, 
			&show_Textures_panel, 
			&show_Mesh_panel, 
			&show_Materials_panel, 
			&show_Scene_panel
		);
	}
	else if (show_Mesh_panel)
	{
		//	显示文件夹界面
		show_folder_panel = false;
		//	显示纹理界面
		show_Textures_panel = false;
		//	显示材质界面
		show_Materials_panel = false;
		//	显示场景界面
		show_Scene_panel = false;

		//	显示Mesh
		GUIUtils::DrawProjectMesh(
			md3dDevice,
			mSrvDescriptorHeap.GetDescriptorHeap(),
			mIconTextures,
			mClientWidth - 620, 
			&show_folder_panel, 
			&show_Textures_panel, 
			&show_Mesh_panel, 
			&show_Materials_panel, 
			&show_Scene_panel
		);

	}
	else if (show_Materials_panel)
	{
		//	显示文件夹界面
		show_folder_panel = false;
		//	显示纹理界面
		show_Textures_panel = false;
		//	显示Mesh界面
		show_Mesh_panel = false;
		//	显示场景界面
		show_Scene_panel = false;

		GUIUtils::DrawProjectMaterials(
			md3dDevice, 
			mSrvDescriptorHeap.GetDescriptorHeap(),
			mIconTextures,
			mClientWidth - 620, 
			&show_folder_panel, 
			&show_Textures_panel, 
			&show_Mesh_panel, 
			&show_Materials_panel, 
			&show_Scene_panel
		);
	}
	else if (show_Scene_panel)
	{
		//	显示文件夹界面
		show_folder_panel = false;
		//	显示纹理界面
		show_Textures_panel = false;
		//	显示Mesh界面
		show_Mesh_panel = false;
		//	显示材质界面
		show_Materials_panel = false;

		GUIUtils::DrawProjectScene(
			md3dDevice, 
			mSrvDescriptorHeap.GetDescriptorHeap(),
			mIconTextures,
			mClientWidth - 620, 
			&show_folder_panel, 
			&show_Textures_panel, 
			&show_Mesh_panel, 
			&show_Materials_panel, 
			&show_Scene_panel
		);
	}

	ImGui::End();

	ImGui::Begin("Console");
	ImGui::End();
}

//	DrawOverLayProfile
void GUISystem::DrawOverLayProfile()
{
	//static int corner = 0;
	//ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	//if (ImGui::Begin("Example: Simple overlay"))
	//{
	//	ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
	//	ImGui::Separator();
	//	if (ImGui::IsMousePosValid())
	//		ImGui::Text("Mouse Position: (%.1f,%.1f)", 0, 0);
	//	else
	//		ImGui::Text("Mouse Position: <invalid>");
	//	if (ImGui::BeginPopupContextWindow())
	//	{
	//		if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
	//		if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
	//		if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
	//		if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
	//		if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
	//		ImGui::EndPopup();
	//	}
	//}
	//ImGui::End();
}

//	DrawPhysicsSettings
void GUISystem::DrawPhysicsSettings()
{
	//Render Item
	ImGui::SetWindowSize(ImVec2(180, 500));
	ImGui::Begin("Physics Settings", &show_physicsSetting_panel);

	ImGui::Text("Bounce Range");
	//ImGui::Checkbox("Bounce Show", &ShowBounding);
	ImGui::End();
}

void GUISystem::DrawSceneGameView()
{
	ImGui::Begin("View");
	ImGui::Image((ImTextureID)mRenderTarget->GpuSrv().ptr, ImVec2(1383, 778));
	ImGui::End();
}

void GUISystem::DrawRenderSetting()
{
	ImGui::Begin("Render Setting");
	ImGui::End();
}

void GUISystem::DrawFrameDebugger()
{
	//Frame Debugger
	ImGui::SetWindowSize(ImVec2(256, 760));
	ImGui::Begin("Frame Debugger", &show_frame_debugger_panel);

	if (ImGui::CollapsingHeader("Draw Shadow Map"))
	{
		ImGui::Image((ImTextureID)mShadowMap->Srv().ptr, ImVec2(256, 256));
	}

	if (ImGui::CollapsingHeader("Draw Depth"))
	{
		ImGui::Image((ImTextureID)mSsao->DepthBufferSrv().ptr, ImVec2(256, 256));
	}

	//if (ImGui::CollapsingHeader("GBuffer0"))
	//{
	//	ImGui::Image((ImTextureID)mGBuffer->GBuffer0Srv().ptr, ImVec2(256, 256));
	//}

	//if (ImGui::CollapsingHeader("GBuffer1"))
	//{
	//	ImGui::Image((ImTextureID)mGBuffer->GBuffer1Srv().ptr, ImVec2(256, 256));
	//}

	//if (ImGui::CollapsingHeader("GBuffer2"))
	//{
	//	ImGui::Image((ImTextureID)mGBuffer->GBuffer2Srv().ptr, ImVec2(256, 256));
	//}

	//if (ImGui::CollapsingHeader("GBuffer3"))
	//{
	//	ImGui::Image((ImTextureID)mGBuffer->GBuffer3Srv().ptr, ImVec2(256, 256));
	//}
	ImGui::End();
}

void GUISystem::DrawProfileView()
{
	ImGui::Begin("Profile");
	static bool animate = true;

	//static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
	//ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));
	//ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));

	static float values[90] = {};
	static int values_offset = 0;
	static double refresh_time = 0.0;
	if (!animate || refresh_time == 0.0)
		refresh_time = ImGui::GetTime();
	while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
	{
		static float phase = 0.0f;
		values[values_offset] = cosf(phase);
		values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
		phase += 0.10f * values_offset;
		refresh_time += 1.0f / 60.0f;
	}

	// Plots can display overlay texts
	// (in this example, we will display an average value)
	{
		float average = 0.0f;
		for (int n = 0; n < IM_ARRAYSIZE(values); n++)
			average += values[n];
		average /= (float)IM_ARRAYSIZE(values);
		char overlay[32];
		sprintf(overlay, "avg %f", average);
		ImGui::PlotLines(" ", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(300, 300));
	}
	ImGui::End();
}


//	Light Settings
void GUISystem::DrawLightSettings()
{
	////Render Item
	//ImGui::SetWindowSize(ImVec2(180, 500));
	//ImGui::Begin("Light Settings", &show_lightSetting_panel);

	//Material* currentMat = mMaterials["sky"].get();

	////ImGui::Text("Environment");

	////ImGui::ColorEdit3("Shadow Color", EnvironmentShadowColor);

	//ImGui::Text("SkyBox");

	//float colorMain[4] = { currentMat->SkyBoxTint.x,currentMat->SkyBoxTint.y,currentMat->SkyBoxTint.z,1 };
	//ImGui::ColorEdit3("Tint", colorMain);
	//currentMat->SkyBoxTint = Vector4(colorMain[0], colorMain[1], colorMain[2], colorMain[3]);

	//ImGui::SliderFloat("Exposure", &currentMat->SkyBoxExposure, 0, 8);
	//ImGui::SliderFloat("Rotation", &currentMat->SkyBoxRotation, 0, 360);

	//bool UseAces = (bool)currentMat->ACES;
	//ImGui::Checkbox("Aces Enable", &UseAces);
	//currentMat->ACES = (int)UseAces;

	//currentMat->NumFramesDirty++;
	//ImGui::End();
}

//	PostProcessing
void GUISystem::DrawPostProcessingSettings()
{
	//Render Item
	ImGui::SetWindowSize(ImVec2(400, 900));
	ImGui::Begin("PostProcessing Settings", &show_postprocessingSetting_panel);


	///// <summary>
	///// RGBSplit
	///// </summary>
	//if (ImGui::CollapsingHeader("RGB Split"))
	//{
	//	Material* currentMat = mMaterials["RGBSplit"].get();
	//	ImGui::Checkbox("Enable RGB Split", &UseRGBSplit);
	//	if (UseRGBSplit)
	//	{
	//		ImGui::SliderFloat("RgbSplit Strength", &currentMat->RGBSplitStrength, 0, 0.3f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}


	///// <summary>
	///// RadialBlur
	///// </summary>
	//if (ImGui::CollapsingHeader("Radial Blur"))
	//{
	//	Material* currentMat = mMaterials["RadialBlur"].get();
	//	ImGui::Checkbox("Enable Radial Blur", &UseRadialBlur);
	//	if (UseRadialBlur)
	//	{
	//		ImGui::SliderFloat("Blur Factory", &currentMat->BlurFactory, 0, 0.1f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}



	///// <summary>
	///// Vignette
	///// </summary>
	//if (ImGui::CollapsingHeader("Vignette"))
	//{
	//	Material* currentMat = mMaterials["Vignette"].get();
	//	ImGui::Checkbox("Enable Vignette", &UseVignette);
	//	if (UseVignette)
	//	{
	//		ImGui::SliderFloat("Width", &currentMat->Width, 0, 2.0f);
	//		ImGui::SliderFloat("Height", &currentMat->Height, 0, 1.0f);
	//		ImGui::SliderFloat("Smooth", &currentMat->Smooth, 1.0, 10.0f);
	//		ImGui::SliderFloat("Alpha", &currentMat->Alpha, 0.0, 1.0f);
	//		float colorMain[4] = { currentMat->VignetteColor.x,currentMat->VignetteColor.y,currentMat->VignetteColor.z,1 };
	//		ImGui::ColorEdit3("Vignette Color", colorMain);
	//		currentMat->VignetteColor = XMFLOAT4(colorMain[0], colorMain[1], colorMain[2], colorMain[3]);
	//	}
	//	currentMat->NumFramesDirty++;
	//}


	///// <summary>
	///// Decolor
	///// </summary>
	//if (ImGui::CollapsingHeader("Decolor"))
	//{
	//	Material* currentMat = mMaterials["Decolor"].get();
	//	ImGui::Checkbox("Enable Decolor", &UseDecolor);
	//	if (UseDecolor)
	//	{
	//		ImGui::SliderFloat("Decolor Strength", &currentMat->DecolorStrength, 0, 1.0f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}


	///// <summary>
	///// Brightness
	///// </summary>
	//if (ImGui::CollapsingHeader("Brightness And Contrast"))
	//{
	//	Material* currentMat = mMaterials["Brightness"].get();
	//	ImGui::Checkbox("Enable Brightness", &UseBrightness);
	//	if (UseBrightness)
	//	{
	//		ImGui::SliderFloat("Bright-ness", &currentMat->Brightness, 0.0f, 3.0f);

	//		ImGui::SliderFloat("Contrast", &currentMat->Contrast, 0.0f, 3.0f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}


	///// <summary>
	///// HSV
	///// </summary>
	//if (ImGui::CollapsingHeader("HSV"))
	//{
	//	Material* currentMat = mMaterials["HSV"].get();
	//	ImGui::Checkbox("Enable HSV", &UseHSV);
	//	if (UseHSV)
	//	{
	//		ImGui::SliderFloat("Hue", &currentMat->Hue, 0, 1.0f);
	//		ImGui::SliderFloat("Saturation", &currentMat->Saturation, -1, 1.0f);
	//		ImGui::SliderFloat("Value", &currentMat->Value, -1, 1.0f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}

	///// <summary>
	///// Mosaic
	///// </summary>
	//if (ImGui::CollapsingHeader("Mosaic"))
	//{
	//	Material* currentMat = mMaterials["Mosaic"].get();
	//	ImGui::Checkbox("Enable Mosaic", &UseMosaic);
	//	if (UseMosaic)
	//	{
	//		ImGui::InputFloat("PixelSize", &currentMat->PixelSize);
	//	}
	//	currentMat->NumFramesDirty++;
	//}


	///// <summary>
	///// Sharpen
	///// </summary>
	//if (ImGui::CollapsingHeader("Sharpen"))
	//{
	//	Material* currentMat = mMaterials["Sharpen"].get();
	//	ImGui::Checkbox("Enable Sharpen", &UseSharpen);
	//	if (UseSharpen)
	//	{
	//		ImGui::SliderFloat("SharpenStrength", &currentMat->SharpenStrength, 0, 5.0f);
	//		ImGui::SliderFloat("SharpenThreshold", &currentMat->SharpenThreshold, 0, 1.0f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}


	///// <summary>
	///// Spherize
	///// </summary>
	//if (ImGui::CollapsingHeader("Spherize"))
	//{
	//	Material* currentMat = mMaterials["Spherize"].get();
	//	ImGui::Checkbox("Enable Spherize", &UseSpherize);
	//	if (UseSpherize)
	//	{
	//		ImGui::SliderFloat("Spherify", &currentMat->Spherify, 0, 1.0f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}


	///// <summary>
	///// WhiteBalance
	///// </summary>
	//if (ImGui::CollapsingHeader("White Balance"))
	//{
	//	Material* currentMat = mMaterials["WhiteBalance"].get();
	//	ImGui::Checkbox("Enable WhiteBalance", &UseWhiteBalance);
	//	if (UseWhiteBalance)
	//	{
	//		ImGui::SliderFloat("Temperature", &currentMat->Temperature, -1.0, 1.0f);
	//		ImGui::SliderFloat("Tint", &currentMat->Tint, -1.0, 1.0f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}


	///// <summary>
	///// OilPaint
	///// </summary>
	//if (ImGui::CollapsingHeader("Oil Paint"))
	//{
	//	Material* currentMat = mMaterials["OilPaint"].get();
	//	ImGui::Checkbox("Enable OilPaint", &UseOilPaint);
	//	if (UseOilPaint)
	//	{
	//		ImGui::SliderFloat("OilPaint Radius", &currentMat->OilPaintRadius, 0.0, 5.0f);
	//		ImGui::SliderFloat("ResoultionValue", &currentMat->ResoultionValue, 0.0, 5.0f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}


	///// <summary>
	///// Relief
	///// </summary>
	//if (ImGui::CollapsingHeader("Relief"))
	//{
	//	ImGui::Checkbox("Enable Relief", &UseRelief);
	//}

	///// <summary>
	///// EdgeDetection
	///// </summary>
	//if (ImGui::CollapsingHeader("EdgeDetection"))
	//{
	//	Material* currentMat = mMaterials["EdgeDetection"].get();
	//	ImGui::Checkbox("Enable EdgeDetection", &UseEdgeDetection);
	//	if (UseEdgeDetection)
	//	{
	//		ImGui::SliderFloat("Edge Width", &currentMat->EdgeWdith, 0.0, 5.0f);
	//		ImGui::SliderFloat("Edge Neon Fade", &currentMat->EdgeNeonFade, 0.0, 1.0f);

	//		float colorOutLine[4] = { currentMat->OutLineColor.x,currentMat->OutLineColor.y,currentMat->OutLineColor.z,1 };
	//		ImGui::ColorEdit3("OutLine Color", colorOutLine);
	//		currentMat->OutLineColor = XMFLOAT4(colorOutLine[0], colorOutLine[1], colorOutLine[2], colorOutLine[3]);

	//		ImGui::SliderFloat("Background Fade", &currentMat->BackgroundFade, 0.0, 1.0f);

	//		float colorMain[4] = { currentMat->BackgroundColor.x,currentMat->BackgroundColor.y,currentMat->BackgroundColor.z,1 };
	//		ImGui::ColorEdit3("Background Color", colorMain);
	//		currentMat->BackgroundColor = XMFLOAT4(colorMain[0], colorMain[1], colorMain[2], colorMain[3]);

	//	}
	//	currentMat->NumFramesDirty++;
	//}

	///// <summary>
	///// SSAO
	///// </summary>
	//if (ImGui::CollapsingHeader("SSAO"))
	//{
	//	ImGui::Checkbox("Enable SSAO", &UseSSAO);
	//}

	///// <summary>
	///// MotionBlur
	///// </summary>
	//if (ImGui::CollapsingHeader("Motion Blur"))
	//{
	//	Material* currentMat = mMaterials["OilPaint"].get();
	//	ImGui::Checkbox("Enable MotionBlur", &UseMotionBlur);
	//	if (UseMotionBlur)
	//	{
	//		ImGui::SliderFloat("Blur Amount", &currentMat->MotionBlurAmount, 0.0, 5.0f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}

	///// <summary>
	///// DOF
	///// </summary>
	//if (ImGui::CollapsingHeader("Depth Of Field"))
	//{
	//	Material* currentMat = mMaterials["OilPaint"].get();
	//	ImGui::Checkbox("Enable Depth Of Field", &UseDOF);
	//	if (UseDOF)
	//	{
	//		ImGui::InputFloat("Distance", &currentMat->Distance);
	//		ImGui::InputFloat("Aperture", &currentMat->Aperture);
	//		ImGui::SliderFloat("FocalLength", &currentMat->FocalLength, 0.0, 300.0f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}

	///// <summary>
	///// FidelityFX Super Resolution
	///// </summary>
	//if (ImGui::CollapsingHeader("FidelityFX Super Resolution"))
	//{
	//	Material* currentMat = mMaterials["OilPaint"].get();
	//	ImGui::Checkbox("Enable FSR", &UseFSR);
	//	if (UseFSR)
	//	{
	//		ImGui::InputFloat("ResoultionLevel", &currentMat->ResoultionLevel);
	//		ImGui::SliderFloat("FSRSharpen", &currentMat->FSRSharpen, 0.0, 1.0f);
	//	}
	//	currentMat->NumFramesDirty++;
	//}

	///// <summary>
	///// FXAA
	///// </summary>
	//if (ImGui::CollapsingHeader("FXAA"))
	//{
	//	ImGui::Checkbox("Enable FXAA", &UseFXAA);
	//}

	///// <summary>
	///// ToneMapping
	///// </summary>
	//if (ImGui::CollapsingHeader("ToneMapping(ACES)"))
	//{
	//	ImGui::Checkbox("Enable ToneMapping", &UseToneMapping);
	//}

	///// <summary>
	///// Bloom
	///// </summary>
	//if (ImGui::CollapsingHeader("Bloom"))
	//{
	//	Material* currentMat = mMaterials["Bloom"].get();
	//	ImGui::Checkbox("Enable Bloom", &UseBloom);
	//	if (UseBloom)
	//	{
	//		ImGui::SliderFloat("Threshold", &currentMat->Threshold, 0.0, 10.0f);
	//		ImGui::SliderFloat("SoftKnee", &currentMat->SoftKnee, 0.0, 1.0f);
	//		ImGui::SliderFloat("Radius", &currentMat->Radius, 0.0, 7.0f);
	//		ImGui::InputFloat("Intensity", &currentMat->Intensity);
	//	}
	//	currentMat->NumFramesDirty++;
	//}

	//ImGui::End();
}

void GUISystem::SetDefaultStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	//	文字颜色
	colors[ImGuiCol_Text] = ImVec4(0.89f, 0.89f, 0.89f, 1.000f);
	//	文字不显示
	colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	//	窗口背景色
	colors[ImGuiCol_WindowBg] = ImVec4(0.1843f, 0.1882f, 0.2078f, 1.000f);
	//	子窗口颜色
	colors[ImGuiCol_ChildBg] = ImVec4(0.2431f, 0.2470f, 0.2745f, 0.000f);
	//	弹出框
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.8f);
	//	边界线
	colors[ImGuiCol_Border] = ImVec4(0.1098f, 0.1137f, 0.1254f, 1.000f);
	//	子结点分界框
	colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);

	colors[ImGuiCol_FrameBg] = ImVec4(0.3137f, 0.3137f, 0.3456f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4431f, 0.4549f, 0.5019f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.4431f, 0.4549f, 0.5019f, 1.000f);

	//	标题背景
	colors[ImGuiCol_TitleBg] = ImVec4(0.1215f, 0.1176f, 0.1411f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.1215f, 0.1176f, 0.1411f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1215f, 0.1176f, 0.1411f, 1.000f);

	//	主菜单
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.1607f, 0.1568f, 0.1803f, 1.000f);

	//	滚动条
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1843f, 0.1882f, 0.2078f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2470f, 0.2470f, 0.2745f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.2470f, 0.2470f, 0.2745f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.8f, 0.1568f, 0.1803f, 1.000f);

	//	复选框
	colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);

	//	滑动条
	colors[ImGuiCol_SliderGrab] = ImVec4(0.3137f, 0.3137f, 0.3456f, 1.000f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.1568f, 0.1803f, 1.000f);

	//	按钮
	colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);

	//	标题
	colors[ImGuiCol_Header] = ImVec4(0.1215f, 0.1176f, 0.1411f, 1.000f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.8f, 0.1568f, 0.1803f, 1.000f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.8f, 0.1568f, 0.1803f, 1.000f);

	//	分割线
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0862f, 0.0901f, 0.1058f, 1.000f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.0862f, 0.0901f, 0.1058f, 1.000f);

	//	调整大小
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);

	//	Tap
	colors[ImGuiCol_Tab] = ImVec4(0.1215f, 0.1176f, 0.1411f, 1.000f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.8f, 0.1568f, 0.1803f, 1.000f);
	colors[ImGuiCol_TabActive] = ImVec4(0.1098f, 0.1137f, 0.1254f, 1.000f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);

	//	
	colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);

	//	文字选中
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 3.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 0.0f;
	style->WindowRounding = 0.0f;
}