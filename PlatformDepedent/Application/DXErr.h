//
// Created by Administrator on 2022/12/26.
//

#ifndef DISTENGINE_DXERR_H
#define DISTENGINE_DXERR_H

#pragma once
#include "PCH.h"

class DXErr {
//--------------------------------------------------------------------------------------
// DXGetErrorString
//--------------------------------------------------------------------------------------
    const wchar* WINAPI DXGetErrorStringW( _In_ HRESULT hr );

#define DXGetErrorString DXGetErrorStringW

//--------------------------------------------------------------------------------------
// DXGetErrorDescription has to be modified to return a copy in a buffer rather than
// the original static string.
//--------------------------------------------------------------------------------------
    void WINAPI DXGetErrorDescriptionW( _In_ HRESULT hr, _Out_cap_(count) wchar* desc, _In_ size_t count );

#define DXGetErrorDescription DXGetErrorDescriptionW

//--------------------------------------------------------------------------------------
//  DXTrace
//
//  Desc:  Outputs a formatted error message to the debug stream
//
//  Args:  wchar* strFile   The current file, typically passed in using the
//                         __FILEW__ macro.
//         DWORD dwLine    The current line number, typically passed in using the
//                         __LINE__ macro.
//         HRESULT hr      An HRESULT that will be traced to the debug stream.
//         CHAR* strMsg    A string that will be traced to the debug stream (may be NULL)
//         BOOL bPopMsgBox If true, then a message box will popup also containing the passed info.
//
//  Return: The hr that was passed in.
//--------------------------------------------------------------------------------------
    HRESULT WINAPI DXTraceW( _In_z_ const wchar* strFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ const wchar* strMsg, _In_ bool bPopMsgBox );

#define DXTrace DXTraceW

//--------------------------------------------------------------------------------------
//
// Helper macros
//
//--------------------------------------------------------------------------------------
#if defined(DEBUG) || defined(_DEBUG)
#define DXTRACE_MSG(str)              DXTrace( __FILEW__, (DWORD)__LINE__, 0, str, false )
#define DXTRACE_ERR(str,hr)           DXTrace( __FILEW__, (DWORD)__LINE__, hr, str, false )
#define DXTRACE_ERR_MSGBOX(str,hr)    DXTrace( __FILEW__, (DWORD)__LINE__, hr, str, true )
#else
    #define DXTRACE_MSG(str)              (0L)
#define DXTRACE_ERR(str,hr)           (hr)
#define DXTRACE_ERR_MSGBOX(str,hr)    (hr)
#endif
};


#endif //DISTENGINE_DXERR_H
