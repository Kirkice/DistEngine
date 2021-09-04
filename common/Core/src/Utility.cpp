/* $Header: /common/core/include/Utility.cpp					8/11/21 20:55p Kirk			$ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Utility.cpp												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#include "../include/PCH.h"

#include "../include/Utility.h"
#include "../include/Expections.h"
#include "../include/App.h"

namespace core
{
	void WriteLog(const wchar* format, ...)
	{
		wchar buffer[4096] = { 0 };
		va_list args;
		va_start(args, format);
		int32 len = vswprintf_s(buffer, ArraySize_(buffer) - 1, format, args);
		if (GlobalApp != nullptr)
			GlobalApp->AddToLog(WStringToAnsi(buffer).c_str());

		buffer[len] = L'\n';
		buffer[len + 1] = 0;
		OutputDebugStringW(buffer);
	}

	void WriteLog(const char* format, ...)
	{
		char buffer[4096] = { 0 };
		va_list args;
		va_start(args, format);
		int32 len = vsprintf_s(buffer, ArraySize_(buffer) - 1, format, args);
		if (GlobalApp != nullptr)
			GlobalApp->AddToLog(buffer);

		buffer[len] = '\n';
		buffer[len + 1] = 0;
		OutputDebugStringA(buffer);
	}

	std::wstring MakeString(const wchar* format, ...)
	{
		wchar buffer[4096] = { 0 };
		va_list args;
		va_start(args, format);
		vswprintf_s(buffer, ArraySize_(buffer), format, args);
		return std::wstring(buffer);
	}

	std::string MakeString(const char* format, ...)
	{
		char buffer[4096] = { 0 };
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, ArraySize_(buffer), format, args);
		return std::string(buffer);
	}

	std::wstring SampleFrameworkDir()
	{
		return std::wstring(SampleFrameworkDir_);
	}
}