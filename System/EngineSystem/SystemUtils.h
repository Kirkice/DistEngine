#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"
#include <Mathf.h>
#include <Material.h>
#include "MeshGeometry.h" 
#include "../../Component/MeshFliter.h"

namespace Dist
{
	const int gNumFrameResources = 3;

	inline void d3dSetDebugName(IDXGIObject* obj, const char* name)
	{
		if (obj)
		{
			obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
		}
	}
	inline void d3dSetDebugName(ID3D12Device* obj, const char* name)
	{
		if (obj)
		{
			obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
		}
	}
	inline void d3dSetDebugName(ID3D12DeviceChild* obj, const char* name)
	{
		if (obj)
		{
			obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
		}
	}

	inline std::wstring AnsiToWString(const std::string& str)
	{
		WCHAR buffer[512];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
		return std::wstring(buffer);
	}

	class SystemUtils
	{
	public:

		static bool IsKeyDown(int vkeyCode);

		static std::string ToString(HRESULT hr);

		static UINT CalcConstantBufferByteSize(UINT byteSize)
		{
			return (byteSize + 255) & ~255;
		}

		static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

		static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
			ID3D12Device* device,
			ID3D12GraphicsCommandList* cmdList,
			const void* initData,
			UINT64 byteSize,
			Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

		static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
			const std::wstring& filename,
			const D3D_SHADER_MACRO* defines,
			const std::string& entrypoint,
			const std::string& target);

		static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();

		static MeshGeometry* BuidlMeshGeometryFromMeshData(std::string name, MeshData data, Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList);
	};

	class DxException
	{
	public:
		DxException() = default;
		DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

		std::wstring ToString()const;

		HRESULT ErrorCode = S_OK;
		std::wstring FunctionName;
		std::wstring Filename;
		int LineNumber = -1;
	};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif

	//	PBR RenderItem
	struct PBRRenderItem
	{
		PBRRenderItem() = default;
		PBRRenderItem(const PBRRenderItem& rhs) = delete;

		//World Matrix
		XMFLOAT4X4 World = Dist::Identity4x4();
		//Tex Transform
		XMFLOAT4X4 TexTransform = Dist::Identity4x4();

		int NumFramesDirty = gNumFrameResources;

		UINT ObjCBIndex = -1;

		PBRMaterial* Mat = nullptr;
		MeshGeometry* Geo = nullptr;

		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

}