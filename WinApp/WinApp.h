/***********************************************************************************************
 ***						W I N A P P  ---  D I S T E N G I N E                            ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : WinApp									   *
 *                                                                                             *
 *                                   File Name : WinApp.h                                      *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/3                                         *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "Event.h"
#include "AppUtils.h"

class WinApp
{
protected:

	WinApp(HINSTANCE hInstance);
	WinApp(const WinApp& rhs) = delete;
	WinApp& operator=(const WinApp& rhs) = delete;
	virtual ~WinApp();

public:

	//using EventFunc = std::function<void(Event&)>;

	static WinApp* GetApp();

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void OnResize() = 0;
	virtual void Tick(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;

	//Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }
	virtual void OnKeyboardInput(const GameTimer & gt) {}

protected:
	bool InitMainWindow();
	void InitLogSystem();
	void CalculateFrameStats();

protected:

	static WinApp* mApp;

	HINSTANCE mhAppInst = nullptr; // application instance handle
	HWND      mhMainWnd = nullptr; // main window handle
	bool      mAppPaused = false;  // is the application paused?
	bool      mMinimized = false;  // is the application minimized?
	bool      mMaximized = false;  // is the application maximized?
	bool      mResizing = false;   // are the resize bars being dragged?
	bool      mFullscreenState = false;// fullscreen enabled

	GameTimer mTimer;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption = L"Dist Engine v1.3 - Writer: Kirk - <DX12>";

	int mClientWidth = 1920;
	int mClientHeight = 1080;

	float fps = 0;
	float mspf = 0;
};