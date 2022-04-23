#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "SystemUtils.h"
#include "GameTimer.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Dist
{
	class EngineApp
	{
	protected:
		EngineApp(HINSTANCE hInstance);
		EngineApp(const EngineApp& rhs) = delete;
		EngineApp& operator=(const EngineApp& rhs) = delete;
		virtual ~EngineApp();

	public:

		static EngineApp* GetApp();

		HINSTANCE AppInst()const;
		HWND      MainWnd()const;
		float     AspectRatio()const;

		bool Get4xMsaaState()const;
		void Set4xMsaaState(bool value);

		int Run();

		//	初始化
		virtual bool Initialize();
		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	protected:
		virtual void CreateRtvAndDsvDescriptorHeaps();
		virtual void OnResize();
		virtual void Update(const GameTimer& gt) = 0;
		virtual void Draw(const GameTimer& gt) = 0;

		// Convenience overrides for handling mouse input.
		virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
		virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
		virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

	protected:
		bool InitMainWindow();
		bool InitDirect3D();
		void CreateCommandObjects();
		void CreateSwapChain();

		void FlushCommandQueue();

		//	CurrentBackBuffer SRV DSV
		ID3D12Resource* CurrentBackBuffer()const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

		void CalculateFrameStats();

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	protected:

		static EngineApp* mApp;

		HINSTANCE mhAppInst = nullptr; // application instance handle
		HWND      mhMainWnd = nullptr; // main window handle
		bool      mAppPaused = false;  // is the application paused?
		bool      mMinimized = false;  // is the application minimized?
		bool      mMaximized = false;  // is the application maximized?
		bool      mResizing = false;   // are the resize bars being dragged?
		bool      mFullscreenState = false;// fullscreen enabled

		// Set true to use 4X MSAA (?.1.8).  The default is false.
		bool      m4xMsaaState = false;    // 4X MSAA enabled
		UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA

		// Used to keep track of the �delta-time?and game time (?.4).
		GameTimer mTimer;
		
		//	dxgi Factory
		Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
		//	交换链
		Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
		//	d3dDevice
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
		//	根签名
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		//	栅栏
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		//	当前栅栏
		UINT64 mCurrentFence = 0;

		//	命令队列
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		//	命令分配器
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
		//	命令列表
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

		//	交换链Buffer个数
		static const int SwapChainBufferCount = 2;
		int mCurrBackBuffer = 0;
		//	交换链Buffer
		Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
		//	深度模板Buffer
		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

		//	RTV DSV SRV
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;
		//	PSO
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		// Derived class should set these in derived constructor to customize starting values.
		std::wstring mMainWndCaption = L"Dist Engine v1.2 - Writer: Kirk - <DX12>";
		D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		int mClientWidth = 1920;
		int mClientHeight = 1080;
		float RectXY[2] = { 0, 0 };
		float RectWH[2] = { 1 ,1 };
	};
}