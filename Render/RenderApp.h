#pragma once
#include "../System/InputSystem/InputSystem.h"
#include "RenderScene/DefaultSceneRender.h"
#include "../System/Scene/DefaultScene.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	class RenderApp : public InputSystem
	{

	public:
		RenderApp(HINSTANCE hInstance);
		RenderApp(const RenderApp& rhs) = delete;
		RenderApp& operator=(const RenderApp& rhs) = delete;
		~RenderApp();

		//	初始化
		virtual bool Initialize()override;

	//private:
	//	//	创建RTV SRV
	//	virtual void CreateRtvAndDsvDescriptorHeaps()override;
	//	//	重新设置 
	//	virtual void OnResize()override;
	//	//	更新
	//	virtual void Update(const GameTimer& gt)override;
	//	//	绘制
	//	virtual void Draw(const GameTimer& gt)override;

	public:
		DefaultSceneRender m_SceneRender;
	};
}