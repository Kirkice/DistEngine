#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "SystemUtils.h"
#include "GameTimer.h"

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

		static EngineApp*			GetApp();

		HINSTANCE					AppInst()const;
		HWND						MainWnd()const;
		float						AspectRatio()const;

		int Run();

		//	初始化
		virtual bool Initialize();
		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	protected:
		//	更新
		virtual void Update(const GameTimer& gt) = 0;

	protected:
		bool InitMainWindow();

		void CalculateFrameStats();

	protected:

		static EngineApp* mApp;

		HINSTANCE mhAppInst = nullptr; // application instance handle
		HWND      mhMainWnd = nullptr; // main window handle
		bool      mAppPaused = false;  // is the application paused?
		bool      mMinimized = false;  // is the application minimized?
		bool      mMaximized = false;  // is the application maximized?
		bool      mResizing = false;   // are the resize bars being dragged?
		bool      mFullscreenState = false;// fullscreen enabled

		// Used to keep track of the �delta-time?and game time (?.4).
		GameTimer mTimer;

		// Derived class should set these in derived constructor to customize starting values.
		std::wstring mMainWndCaption = L"Dist Engine v2.0 - Writer: Kirk - <DX12>";
		int mClientWidth = 1920;
		int mClientHeight = 1080;
		float RectXY[2] = { 0, 0 };
		float RectWH[2] = { 1 ,1 };
	};
}