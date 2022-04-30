#include "SystemApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	SystemApp::SystemApp(HINSTANCE hInstance) : EngineApp(hInstance)
	{
	}

	SystemApp::~SystemApp()
	{
		if (md3dDevice != nullptr)
			FlushCommandQueue();
	}

	bool SystemApp::Initialize()
	{
		if (!EngineApp::Initialize())
			return false;

		//	默认场景
		default_scene = DefaultScene();

		//	初始化场景
		default_scene.InitScene(md3dDevice, mCommandList);

		return true;
	}

	void SystemApp::Update(const GameTimer& gt)
	{
		default_scene.UpdateScene(gt);
	}
}