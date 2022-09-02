#include "RenderCore.h"
#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "DistRenderPipeline.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

RenderCore::RenderCore(HINSTANCE hInstance) : DistRenderPipeline(hInstance)
{
}

RenderCore::~RenderCore()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

void RenderCore::Draw(const GameTimer& gt)
{
	Render();
}