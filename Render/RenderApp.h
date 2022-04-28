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

		//	��ʼ��
		virtual bool Initialize()override;

	private:
		//	����RTV SRV
		virtual void CreateRtvAndDsvDescriptorHeaps()override;
		//	��������
		virtual void OnResize()override;
		//	����
		virtual void Update(const GameTimer& gt)override;
	};
}