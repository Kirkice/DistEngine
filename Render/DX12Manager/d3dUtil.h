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

const int gNumFrameResources = 3;

inline void d3dSetDebugName(IDXGIObject* obj, const char* name)
{
    if(obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}
inline void d3dSetDebugName(ID3D12Device* obj, const char* name)
{
    if(obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}
inline void d3dSetDebugName(ID3D12DeviceChild* obj, const char* name)
{
    if(obj)
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

class d3dUtil
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

//struct MaterialConstants
//{
//	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
//	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
//	float Roughness = 0.25f;
//    float Metallic = 0.85f;
//	DirectX::XMFLOAT4X4 MatTransform = Mathf::Identity4x4();
//
//    //���
//	float OutLineWidth = 0.5f;
//    DirectX::XMFLOAT4 OutLineColor = { 0, 0, 0, 1 };
//
//    //��Ӱ
//    float shadowRange_1st = 0.5f;
//    float shadowFeather_1st = 0.5f;
//    DirectX::XMFLOAT4 shadowColor_1st = { 1,1,1,1 };
//	float shadowRange_2nd = 0.5f;
//	float shadowFeather_2nd = 0.5f;
//    DirectX::XMFLOAT4 shadowColor_2nd = { 1,1,1,1 };
//
//    //��Ե��
//    float rimRange = 0.5f;
//    float rimFeather = 0.5f;
//    DirectX::XMFLOAT4 rimColor = { 1,1,1,1 };
//
//    //�Է���
//    float emissStrength = 0.5f;
//    DirectX::XMFLOAT4 emissColor = { 1,1,1,1 };
//
//    //�߹�
//    float specularRange = 0.5;
//    float specularFeather = 0.5f;
//    DirectX::XMFLOAT4 specularColor = { 1,1,1,1 };
//
//	//������Ӱ��ɫ
//	DirectX::XMFLOAT4 LightShadowColor = { 1,1,1,1 };
//};
//
//struct Texture
//{
//	// Unique material name for lookup.
//	std::string Name;
//
//	std::wstring Filename;
//
//	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
//};


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