#pragma once
#include "../System/InputSystem/InputSystem.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	class RenderApp : public InputSystem
	{

	public:
		RenderApp(HINSTANCE hInstance);
		RenderApp(const InputSystem& rhs) = delete;
		RenderApp& operator=(const RenderApp& rhs) = delete;
		~RenderApp();

		//	初始化
		virtual bool Initialize()override;

	private:
		//	创建RTV SRV
		virtual void CreateRtvAndDsvDescriptorHeaps()override;
		//	重新设置
		virtual void OnResize()override;
		//	更新
		virtual void Update(const GameTimer& gt)override;
	};
}