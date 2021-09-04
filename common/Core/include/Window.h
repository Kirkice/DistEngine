/* $Header: /common/Core/include/Window.h				8/25/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Window.h											      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "PCH.h"

#include "Expections.h"
#include "Containers.h"

namespace core
{
	class Window
	{

		// Public types
	public:

		typedef void (*MsgFunction)(void*, HWND, UINT, WPARAM, LPARAM);

		// Constructor and destructor
	public:

		Window(HINSTANCE hinstance,
			LPCWSTR name = L"SampleCommon Window",
			DWORD style = WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU,
			DWORD exStyle = WS_EX_APPWINDOW,
			DWORD clientWidth = 1280,
			DWORD clientHeight = 720,
			LPCWSTR iconResource = NULL,
			LPCWSTR smallIconResource = NULL,
			LPCWSTR menuResource = NULL,
			LPCWSTR accelResource = NULL);
		~Window();



		// Public methods
	public:

		HWND GetHwnd() const;
		HMENU GetMenu() const;
		HINSTANCE GetHinstance() const;
		void MessageLoop();

		BOOL IsAlive() const;
		BOOL IsMinimized() const;
		BOOL HasFocus() const;
		LONG_PTR GetWindowStyle() const;
		LONG_PTR GetExtendedStyle() const;
		void SetWindowStyle(DWORD newStyle);
		void SetExtendedStyle(DWORD newExStyle);
		void Maximize();
		void SetWindowPos(INT posX, INT posY);
		void GetWindowPos(INT& posX, INT& posY) const;
		void ShowWindow(bool show = true);
		void SetClientArea(INT clientX, INT clientY);
		void GetClientArea(INT& clientX, INT& clientY) const;
		void SetWindowTitle(LPCWSTR title);
		void SetScrollRanges(INT scrollRangeX,
			INT scrollRangeY,
			INT posX,
			INT posY);
		void Destroy();

		INT	CreateMessageBox(LPCWSTR message, LPCWSTR title = NULL, UINT type = MB_OK);

		void RegisterMessageCallback(MsgFunction msgFunction, void* context);

		operator HWND() { return hwnd; }		//conversion operator

	// Private methods
	private:
		void MakeWindow(LPCWSTR iconResource, LPCWSTR smallIconResource, LPCWSTR menuResource);

		LRESULT	MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		// Private members
	private:

		// Window properties
		HWND hwnd;			        // The window handle
		HINSTANCE hinstance;		// The HINSTANCE of the application
		std::wstring appName;		// The name of the application
		DWORD style;			    // The current window style
		DWORD exStyle;		        // The extended window style
		HACCEL accelTable;		    // Accelerator table handle

		struct Callback
		{
			MsgFunction Function;
			void* Context;
		};

		GrowableList<Callback> messageCallbacks;		    // Message callback list
	};
}