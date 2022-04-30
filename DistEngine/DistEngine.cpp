#include "DistEngine.h"


using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
		PSTR cmdLine, int showCmd)
	{
		// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

		try
		{
			DistEngine theApp(hInstance);
			if (!theApp.Initialize())
				return 0;

			return theApp.Run();
		}
		catch (DxException& e)
		{
			MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
			return 0;
		}
	}

	DistEngine::DistEngine(HINSTANCE hInstance) : RenderApp(hInstance)
	{
	}

	DistEngine::~DistEngine()
	{
		if (md3dDevice != nullptr)
			FlushCommandQueue();
	}
}