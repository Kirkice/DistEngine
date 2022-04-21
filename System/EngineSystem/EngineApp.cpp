#include "EngineApp.h"
#include <WindowsX.h>


using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

namespace Dist
{
	#define IDI_ICON1                       101


	LRESULT CALLBACK
		MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		return EngineApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
	}

	EngineApp* EngineApp::mApp = nullptr;
	EngineApp* EngineApp::GetApp()
	{
		return mApp;
	}

	EngineApp::EngineApp(HINSTANCE hInstance)
		: mhAppInst(hInstance)
	{
		// Only one EngineApp can be constructed.
		assert(mApp == nullptr);
		mApp = this;
	}

	EngineApp::~EngineApp()
	{
	}

	HINSTANCE EngineApp::AppInst()const
	{
		return mhAppInst;
	}

	HWND EngineApp::MainWnd()const
	{
		return mhMainWnd;
	}

	float EngineApp::AspectRatio()const
	{
		return static_cast<float>(mClientWidth) / mClientHeight;
	}

	int EngineApp::Run()
	{
		MSG msg = { 0 };

		mTimer.Reset();

		while (msg.message != WM_QUIT)
		{
			// If there are Window messages then process them.
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// Otherwise, do animation/game stuff.
			else
			{
				mTimer.Tick();

				if (!mAppPaused)
				{
					CalculateFrameStats();
					Update(mTimer);
				}
				else
				{
					Sleep(100);
				}
			}
		}

		return (int)msg.wParam;
	}

	bool EngineApp::Initialize()
	{
		if (!InitMainWindow())
			return false;

		return true;
	}

	LRESULT EngineApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	bool EngineApp::InitMainWindow()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = mhAppInst;
		wc.hIcon = LoadIcon(mhAppInst, MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"MainWnd";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass Failed.", 0, 0);
			return false;
		}

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, mClientWidth, mClientHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
		if (!mhMainWnd)
		{
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}

		ShowWindow(mhMainWnd, SW_SHOW);
		UpdateWindow(mhMainWnd);

		return true;
	}

	void EngineApp::CalculateFrameStats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.

		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		// Compute averages over one second period.
		if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			//wstring fpsStr = to_wstring(fps);
			//wstring mspfStr = to_wstring(mspf);

			//wstring windowText = mMainWndCaption +
			//	L"    fps: " + fpsStr +
			//	L"   mspf: " + mspfStr;

			//SetWindowText(mhMainWnd, windowText.c_str());

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}
}