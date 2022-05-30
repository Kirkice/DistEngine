#include "GBufferPass.h"
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

GBuffer::GBuffer(
	UINT width,
	UINT height)
{
	mWidth = width;
	mHeight = height;

	mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
	mScissorRect = { 0, 0, (int)width, (int)height };
}

ID3D12Resource* GBuffer::GetGBuffer0()
{
	return GBuffer0.Get();
}

ID3D12Resource* GBuffer::GetGBuffer1()
{
	return GBuffer1.Get();
}

ID3D12Resource* GBuffer::GetGBuffer2()
{
	return GBuffer2.Get();
}

ID3D12Resource* GBuffer::GetGBuffer3()
{
	return GBuffer3.Get();
}