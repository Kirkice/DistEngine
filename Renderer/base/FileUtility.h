#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <wrl/event.h>

#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <memory>
#include <cwctype>
#include <exception>

#include <ppltasks.h>
#include <functional>

#include <vector>
#include <string>
#include <ppl.h>
#include "Utility.h"

namespace Utility
{
	using namespace std;
	using namespace concurrency;

	typedef shared_ptr<vector<byte> > ByteArray;
	extern ByteArray NullFile;

	// Reads the entire contents of a binary file.  If the file with the same name except with an additional
	// ".gz" suffix exists, it will be loaded and decompressed instead.
	// This operation blocks until the entire file is read.
	ByteArray ReadFileSync(const wstring& fileName);

	// Same as previous except that it does not block but instead returns a task.
	task<ByteArray> ReadFileAsync(const wstring& fileName);
}
