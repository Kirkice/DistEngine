#pragma once
#include "EngineApp.h"
#include "../../Component/Camera.h"
#include "../Scene/DefaultScene.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	class SystemApp : public EngineApp
	{

	public:
		SystemApp(HINSTANCE hInstance);
		SystemApp(const SystemApp& rhs) = delete;
		SystemApp& operator=(const SystemApp& rhs) = delete;
		~SystemApp();

		virtual bool Initialize()override;

		virtual void Update(const GameTimer& gt)override;

	public:
		DefaultScene default_scene;
	};

}