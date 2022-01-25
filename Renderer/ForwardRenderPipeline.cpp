#include "ForwardRenderPipeline.h"
#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

ForwardRenderer::ForwardRenderer(HINSTANCE hInstance) : GraphicsCore(hInstance)
{
}

ForwardRenderer::~ForwardRenderer() 
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

void ForwardRenderer::Draw(const GameTimer& gt)
{
	ForwardRender();
}


void ForwardRenderer::ForwardRender()
{
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());
	ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));

	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
	auto matBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();

	//m_hdrScene->BeginScene(mCommandList.Get());
	//m_hdrScene->SetDevice(md3dDevice.Get(), m_resourceDescriptors->GetCpuHandle(Descriptors::SceneTex), m_renderDescriptors->GetCpuHandle(RTVDescriptors::HDRScene));
	//m_hdrScene->EndScene(mCommandList.Get());

	ForwardRenderer::DrawShadowMap(matBuffer);

	ForwardRenderer::DrawDepthNormal();


	mCommandList->SetGraphicsRootSignature(mSsaoRootSignature.Get());
	mSsao->ComputeSsao(mCommandList.Get(), mCurrFrameResource, 2);


	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	matBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	mCommandList->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());


	CD3DX12_GPU_DESCRIPTOR_HANDLE skyTexDescriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	skyTexDescriptor.Offset(mSkyTexHeapIndex, mCbvSrvUavDescriptorSize);
	mCommandList->SetGraphicsRootDescriptorTable(4, skyTexDescriptor);

	CD3DX12_GPU_DESCRIPTOR_HANDLE iblTexDescriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	iblTexDescriptor.Offset(mIBLTexHeapIndex, mCbvSrvUavDescriptorSize);
	mCommandList->SetGraphicsRootDescriptorTable(4, iblTexDescriptor);

	//ForwardRenderer::DrawOutLine();

	ForwardRenderer::DrawOpaque();

	//SkyBox
	auto skyMatBuufer = mCurrFrameResource->SkyBoxMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, skyMatBuufer->GetGPUVirtualAddress());
	ForwardRenderer::DrawSkyBox();

	//Draw Transparent
	matBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());
	DrawTransparent();

	//mCommandList->SetPipelineState(mPSOs["debug"].Get());
	//DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Debug]);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ForwardRenderer::DrawBounding();

	ForwardRenderer::DrawGizmo();

	ForwardRenderer::DrawImgui();



	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	mCurrFrameResource->Fence = ++mCurrentFence;

	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

//��Ⱦ��Ӱ��ͼ
void ForwardRenderer::DrawShadowMap(ID3D12Resource* matBuffer)
{
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

	mCommandList->SetGraphicsRootDescriptorTable(4, mNullSrv);

	mCommandList->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	DrawSceneToShadowMap();
}

//��Ⱦ��ȷ���
void ForwardRenderer::DrawDepthNormal()
{
	DrawNormalsAndDepth();
}

////��Ⱦ�⹴��
void ForwardRenderer::DrawOutLine()
{
	mCommandList->SetPipelineState(mPSOs["outline"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);
}

//��Ⱦ��͸������
void ForwardRenderer::DrawOpaque()
{
	mCommandList->SetPipelineState(mPSOs["litOpaque"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);
}

//��Ⱦ�����
void ForwardRenderer::DrawSkyBox()
{
	if (renderSkyBox)
	{
		mCommandList->SetPipelineState(mPSOs["sky"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Sky]);
	}
}


void ForwardRenderer::DrawTransparent()
{
	mCommandList->SetPipelineState(mPSOs["transparent"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Transparent]);

	mCommandList->SetPipelineState(mPSOs["Unlit"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Unlit]);
}

void ForwardRenderer::DrawBounding()
{
	if (ShowBounding)
	{
		mCommandList->SetPipelineState(mPSOs["bounding"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Bounding]);
	}
}

void ForwardRenderer::DrawGizmo()
{
	mCommandList->SetPipelineState(mPSOs["Gizmo"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Gizmo]);
}

void ForwardRenderer::DrawPostProcessing()
{

}

void ForwardRenderer::DrawImgui()
{
	DrawEditor();
}


//����UI
void ForwardRenderer::DrawEditor()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Draw Stylize
	ImGui::StyleColorsClassic();

	//Draw Menu
	ForwardRenderer::DrawMenuEditor();

	//Draw Light Setting
	if (show_lightSetting_panel)
		ForwardRenderer::DrawLightSettings();
	if (show_physicsSetting_panel)
		ForwardRenderer::DrawPhysicsSettings();


	//Draw Graphics Item
	ForwardRenderer::DrawGraphicsItemEditor();
	////Render Item
	ForwardRenderer::DrawRenderItemEditor();
	////Console
	ForwardRenderer::DrawConsoleEditor();

	mCommandList->SetDescriptorHeaps(1, mSrvHeap.GetAddressOf());
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());
}

//���Ʋ˵�
void ForwardRenderer::DrawMenuEditor()
{
	ImGui::SetNextWindowBgAlpha(0.8f);
	if (ImGui::BeginMainMenuBar())
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
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

//����Graphics Item Editor
void ForwardRenderer::DrawGraphicsItemEditor()
{
	//Graphics Item
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("Graphics Item");
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(320, mClientHeight), ImGuiCond_Always);

	//Camera
	if (ImGui::CollapsingHeader("MainCamera"))
	{
		mCamera.UpdateCamPosArray();
		mCamera.UpdateCamRotArray();
		ImGui::Separator();
		ImGui::Text("Transform");
		ImGui::InputFloat3("Position", mCamera.mPosArray);
		ImGui::InputFloat3("Rotation", mCamera.mRotArray);
		ImGui::InputFloat3("Scale", mCamera.mScaleArray);
		ImGui::Separator();
		ImGui::Text("Camera");
		ImGui::SliderFloat("FOV(Y)", &mCamFov,0,179);
		ImGui::InputFloat("Near Clip", &mCamClipN);
		ImGui::InputFloat("Far Clip", &mCamClipF);
		ImGui::Checkbox("RenderSkyBox", &renderSkyBox);
		ImGui::ColorEdit3("SolidColor", SolidColor);
		ImGui::InputFloat2("ViewPort", RectXY);
		ImGui::InputFloat2("Rect", RectWH);
		ImGui::Separator();
	}

	//Light
	if (ImGui::CollapsingHeader("Light"))
	{
		if (ImGui::TreeNode("Direction Light"))
		{
			//Direction Light
			ImGui::Text("Transform");
			float dirLightPos[3] = { mDirectionLightsPos.x,mDirectionLightsPos.y,mDirectionLightsPos.z };
			ImGui::InputFloat3("(D)Position", dirLightPos);
			mDirectionLightsPos = XMFLOAT3(dirLightPos[0], dirLightPos[1], dirLightPos[2]);
			ImGui::InputFloat3("(D)Rotation", mDirectionLightsAngle);
			ImGui::InputFloat3("(D)Scale", mDirectionLightsScale);
			ImGui::Text("Settings");
			ImGui::Checkbox("Direction Enable", &mDirectionLightsActive);
			ImGui::ColorEdit3("(D)Color", mDirectionLightsColor);
			ImGui::SliderFloat("(D)Intensity", &mDirectionLightsStrength, 0.0f,10);
			ImGui::Checkbox("Cast Shadow", &mDirectionLightsCastShadow);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Point Light"))
		{
			//Point Light
			ImGui::Text("Transform");
			float pointLightPos[3] = { mPointLightsPos.x,mPointLightsPos.y,mPointLightsPos.z };
			ImGui::InputFloat3("(P)Position", pointLightPos);
			mPointLightsPos = XMFLOAT3(pointLightPos[0], pointLightPos[1], pointLightPos[2]);
			ImGui::InputFloat3("(P)Rotation", mPointLightsRot);
			ImGui::InputFloat3("(P)Scale", mPointLightsScale);
			ImGui::Text("Settings");
			ImGui::Checkbox("Point Enable", &mPointLightsActive);
			ImGui::ColorEdit3("(P)Color", mPointLightsColor);
			ImGui::SliderFloat("(P)Intensity", &mPointLightsStrength, 0, 10);
			ImGui::InputFloat("(P)Range", &mPointLightsRange);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Spot Light"))
		{
			//Spot Light
			ImGui::Text("Transform");
			float spotLightPos[3] = { mSpotLightsPos.x,mSpotLightsPos.y,mSpotLightsPos.z };
			ImGui::InputFloat3("(S)Position", spotLightPos);
			ImGui::InputFloat3("(S)Rotation", mSpotLightsAngle);
			ImGui::InputFloat3("(S)Scale", mSpotLightsScale);
			ImGui::Text("Settings");
			ImGui::Checkbox("Spot Enable", &mSpotLightsActive);
			ImGui::ColorEdit3("(S)Color", mSpotLightsColor);
			ImGui::DragFloat("(S)Intensity", &mSpotLightsStrength, 0.003f, 0, 3);
			ImGui::InputFloat("(S)Range", &mSpotLightsRange);
			ImGui::InputFloat("Cut Angle", &mSpotLightsCutAngle);
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

//����Render Item Editor
void ForwardRenderer::DrawRenderItemEditor()
{
	//Render Item
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("RenderItem Properties");
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x + mClientWidth - 300, main_viewport->WorkPos.y), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(300, mClientHeight), ImGuiCond_Always);

	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Separator();
		ImGui::Text("Transform");
		ImGui::InputFloat3("Position", TargetTransform);
		ImGui::InputFloat3("Rotation", TargetRotationAngle);
		ImGui::InputFloat3("Scale", TargetScale);
		ImGui::Separator();
	}
	if (ImGui::CollapsingHeader("Material"))
	{
		Material* currentMat = mMaterials["sphere"].get();

		if (ImGui::TreeNode("PBR Settings"))
		{
			float colorMain[4] = { currentMat->BaseColor.x,currentMat->BaseColor.y,currentMat->BaseColor.z,1 };
			ImGui::ColorEdit3("MainColor", colorMain);
			currentMat->BaseColor = Vector4(colorMain[0], colorMain[1], colorMain[2], colorMain[3]);

			ImGui::SliderFloat("Metallic", &currentMat->Metallic, 0, 1);
			ImGui::SliderFloat("Smoothness", &currentMat->Smoothness, 0, 1);
			ImGui::SliderFloat("Occlusion", &currentMat->Occlusion, 0, 1);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Addition Settings"))
		{
			float colorEmission[4] = { currentMat->EmissionColor.x,currentMat->EmissionColor.y,currentMat->EmissionColor.z,1 };
			ImGui::ColorEdit3("Emission", colorEmission);
			currentMat->EmissionColor = Vector4(colorEmission[0], colorEmission[1], colorEmission[2], colorEmission[3]);
			ImGui::SliderFloat("Strength", &currentMat->EmissionStrength, 0, 1);
			ImGui::TreePop();
		}

		currentMat->NumFramesDirty++;
	}
	ImGui::End();
}

void ForwardRenderer::DrawConsoleEditor()
{
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("Console");
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x + 320, main_viewport->WorkPos.y + mClientHeight - 180), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(mClientWidth - 620, 180), ImGuiCond_Always);
	if (ImGui::SmallButton("[Debug] Add 5 entries"))
	{
		static int counter = 0;
		const char* categories[3] = { "info", "warn", "error" };
		const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
		for (int n = 0; n < 5; n++)
		{
			const char* category = categories[counter % IM_ARRAYSIZE(categories)];
			const char* word = words[counter % IM_ARRAYSIZE(words)];
			counter++;
		}
	}
	ImGui::End();
}

void ForwardRenderer::DrawOverLayProfile()
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

void ForwardRenderer::DrawPhysicsSettings()
{
	//Render Item
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::SetWindowSize(ImVec2(180, 500), ImGuiCond_Always);
	ImGui::Begin("Physics Settings", &show_physicsSetting_panel);

	ImGui::Text("Bounce Range");
	ImGui::Checkbox("Bounce Show", &ShowBounding);
	ImGui::End();
}

//Light Settings
void ForwardRenderer::DrawLightSettings()
{
	//Render Item
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::SetWindowSize(ImVec2(180, 500), ImGuiCond_Always);
	ImGui::Begin("Light Settings", &show_lightSetting_panel);

	Material* currentMat = mMaterials["sky"].get();

	//ImGui::Text("Environment");

	//ImGui::ColorEdit3("Shadow Color", EnvironmentShadowColor);

	ImGui::Text("SkyBox");

	float colorMain[4] = { currentMat->SkyBoxTint.x,currentMat->SkyBoxTint.y,currentMat->SkyBoxTint.z,1 };
	ImGui::ColorEdit3("Tint", colorMain);
	currentMat->SkyBoxTint = Vector4(colorMain[0], colorMain[1], colorMain[2], colorMain[3]);

	ImGui::SliderFloat("Exposure", &currentMat->SkyBoxExposure, 0, 8);
	ImGui::SliderFloat("Rotation", &currentMat->SkyBoxRotation, 0, 360);

	bool UseAces = (bool)currentMat->ACES;
	ImGui::Checkbox("Aces Enable", &UseAces);
	currentMat->ACES = (int)UseAces;

	currentMat->NumFramesDirty++;
	ImGui::End();
}