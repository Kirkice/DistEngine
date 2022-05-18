#pragma once
#include "../Core/Mathf/Mathf.h"
#include "GraphicsCore.h"
#include "ForwardRenderPipeline.h"

#include "../ThirdParty/ImGUI/imgui.h"
#include "../ThirdParty/ImGUI/imgui_impl_win32.h"
#include "../ThirdParty/ImGUI/imgui_impl_dx12.h"
 
#pragma comment(lib,"x64\\Debug\\Core.lib")
#pragma comment(lib,"x64\\Debug\\ImGUI.lib") 
#pragma comment(lib,"x64\\Debug\\System.lib")
#pragma comment(lib,"x64\\Debug\\Component.lib")
#pragma comment(lib,"x64\\Debug\\Stb.lib")
#pragma comment(lib,"x64\\Debug\\TinyObjLoader.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class EngineApp : public ForwardRenderer
{
public:
	EngineApp(HINSTANCE hInstance);
	EngineApp(const EngineApp& rhs) = delete;
	EngineApp& operator=(const EngineApp& rhs) = delete;
	~EngineApp();
};