#pragma once
#include "../Core/Mathf/Mathf.h"
#include "DistRenderPipeline.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class RenderCore : public DistRenderPipeline
{
public:
	RenderCore(HINSTANCE hInstance);
	RenderCore(const RenderCore& rhs) = delete;
	RenderCore& operator=(const RenderCore& rhs) = delete;
	~RenderCore();

	virtual void Draw(const GameTimer& gt)override;
};