#pragma once
#include "Mathf.h"
#include "EditorApp.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#pragma comment(lib,"x64\\Debug\\Component.lib")
#pragma comment(lib,"x64\\Debug\\Core.lib") 
#pragma comment(lib,"x64\\Debug\\ImGUI.lib")
#pragma comment(lib,"x64\\Debug\\Render.lib")
#pragma comment(lib,"x64\\Debug\\Resources.lib")
#pragma comment(lib,"x64\\Debug\\System.lib")
#pragma comment(lib,"x64\\Debug\\DistEditor.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	class DistEngine : public EditorApp
	{
	public:
		DistEngine(HINSTANCE hInstance);
		DistEngine(const DistEngine& rhs) = delete;
		DistEngine& operator=(const DistEngine& rhs) = delete;
		~DistEngine();
	};
}