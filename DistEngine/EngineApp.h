#pragma once
#include "Mathf.h"
#include "GraphicsCore.h"
#include "ForwardRenderPipeline.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#pragma comment(lib,"x64\\Debug\\Core.lib")
#pragma comment(lib,"x64\\Debug\\ImGUI.lib") 
#pragma comment(lib,"x64\\Debug\\ModelLoader.lib")
#pragma comment(lib,"x64\\Debug\\Renderer.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Renderer;

class EngineApp : public ForwardRenderer
{
public:
	EngineApp(HINSTANCE hInstance);
	EngineApp(const EngineApp& rhs) = delete;
	EngineApp& operator=(const EngineApp& rhs) = delete;
	~EngineApp();
};