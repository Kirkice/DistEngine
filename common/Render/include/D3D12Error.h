/* $Header: /common/Render/include/D3D12Error.h	       8/28/21 13:13pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : D3D12Error.h											  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "../../Core/include/PCH.h"

// DXGetErrorString
const wchar* WINAPI DXGetErrorStringW(_In_ HRESULT hr);
#define DXGetErrorString DXGetErrorStringW


// DXGetErrorDescription has to be modified to return a copy in a buffer rather than
// the original static string.
void WINAPI DXGetErrorDescriptionW(_In_ HRESULT hr, _Out_cap_(count) wchar* desc, _In_ size_t count);
#define DXGetErrorDescription DXGetErrorDescriptionW

// Helper macros
HRESULT WINAPI DXTraceW(_In_z_ const wchar* strFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ const wchar* strMsg, _In_ bool bPopMsgBox);
#define DXTrace DXTraceW

#if defined(DEBUG) || defined(_DEBUG)
#define DXTRACE_MSG(str)              DXTrace( __FILEW__, (DWORD)__LINE__, 0, str, false )
#define DXTRACE_ERR(str,hr)           DXTrace( __FILEW__, (DWORD)__LINE__, hr, str, false )
#define DXTRACE_ERR_MSGBOX(str,hr)    DXTrace( __FILEW__, (DWORD)__LINE__, hr, str, true )
#else
#define DXTRACE_MSG(str)              (0L)
#define DXTRACE_ERR(str,hr)           (hr)
#define DXTRACE_ERR_MSGBOX(str,hr)    (hr)
#endif