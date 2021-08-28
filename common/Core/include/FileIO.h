/* $Header: /common/core/include/FileIO.h			         8/11/21 20:55p Kirk			$ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : FileIO.h											      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "../../Core/include/PCH.h"

#include "../../Core/include/Expections.h"
#include "../../Core/include/Utility.h"
#include "../../Core/include/Containers.h"

namespace core
{
	// Utility functions
	bool FileExists(const wchar* filePath);
	bool DirectoryExists(const wchar* dirPath);
	std::wstring GetDirectoryFromFilePath(const wchar* filePath);
	std::wstring GetFileName(const wchar* filePath);
	std::wstring GetFileNameWithoutExtension(const wchar* filePath);
	std::wstring GetFilePathWithoutExtension(const wchar* filePath);
	std::wstring GetFileExtension(const wchar* filePath);
	uint64 GetFileTimestamp(const wchar* filePath);

	std::string ReadFileAsString(const wchar* filePath);
	void WriteStringAsFile(const wchar* filePath, const std::string& data);

	void ReadFileAsByteArray(const wchar* filePath, Array<uint8>& data);
	void WriteFileAsByteArray(const wchar* filePath, const Array<uint8>& data);

	enum class FileOpenMode
	{
		Read = 0,
		Write = 1,
	};

	class File
	{

	private:

		HANDLE fileHandle;
		FileOpenMode openMode;

	public:

		// Lifetime
		File();
		File(const wchar* filePath, FileOpenMode openMode);
		~File();

		// Explicit Open and close
		void Open(const wchar* filePath, FileOpenMode openMode);
		void Close();

		// I/O
		void Read(uint64 size, void* data) const;
		void Write(uint64 size, const void* data) const;

		template<typename T> void Read(T& data) const;
		template<typename T> void Write(const T& data) const;

		// Accessors
		uint64 Size() const;
	};

	// == File ========================================================================================

	template<typename T> void File::Read(T& data) const
	{
		Read(sizeof(T), &data);
	}

	template<typename T> void File::Write(const T& data) const
	{
		Write(sizeof(T), &data);
	}

	// Reads a POD type from a file
	template<typename T> void ReadFromFile(const wchar* fileName, T& val)
	{
		File file(fileName, FileOpenMode::Read);
		file.Read(val);
	}

	// Writes a POD type to a file
	template<typename T> void WriteToFile(const wchar* fileName, const T& val)
	{
		File file(fileName, FileOpenMode::Write);
		file.Write(val);
	}
}
