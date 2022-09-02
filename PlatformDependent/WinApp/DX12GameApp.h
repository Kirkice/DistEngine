/***********************************************************************************************
 ***						W I N A P P  ---  D I S T E N G I N E                            ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : WinApp									   *
 *                                                                                             *
 *                                   File Name : DX12GameApp.h                                 *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/4                                         *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once
#include "DX12Utils.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;
using namespace Microsoft::WRL;


class DX12GameApp : public WinApp
{
public:
	DX12GameApp(HINSTANCE hInstance);
	DX12GameApp(const DX12GameApp& rhs) = delete;
	DX12GameApp& operator=(const DX12GameApp& rhs) = delete;
	~DX12GameApp();

public:
	virtual bool Initialize()override;
	virtual void OnResize()override;
	virtual void Tick(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

protected:
	//	初始化d3d
	bool InitDirect3D();

	//	初始化DXGI
	void InitDXGIFactory();

	//	初始化DX12 设备
	void InitDX12Device();

	//	初始化栅栏同步
	void InitFence();

	//	初始化MSAA
	void InitMsAA();

	//	重置视口
	void OnResizeViewPort();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

protected:

	//	dxgi
	ComPtr<IDXGIFactory4> mdxgiFactory;
	//	d3d 设备
	ComPtr<ID3D12Device> md3dDevice;
	//	栅栏
	ComPtr<ID3D12Fence> mFence;
	UINT64 mCurrentFence = 0;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	float RectXY[2] = { 0, 0 };
	float RectWH[2] = { 1 ,1 };
}; 