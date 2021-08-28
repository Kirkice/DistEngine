/* $Header: /common/Render/include/Profiler.h	          8/28/21 13:13pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Profiler.h												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "../../Core/include/PCH.h"
#include "../../Core/include/InterfacePointers.h"
#include "../../Core/include/Timer.h"
#include "../../Core/include/Containers.h"
#include "../../render/include/GraphicsTypes.h"

namespace render
{
	struct ProfileData;

	class Profiler
	{

	public:

		static Profiler GlobalProfiler;

		void Initialize();
		void Shutdown();

		uint64 StartProfile(ID3D12GraphicsCommandList* cmdList, const char* name);
		void EndProfile(ID3D12GraphicsCommandList* cmdList, uint64 idx);

		uint64 StartCPUProfile(const char* name);
		void EndCPUProfile(uint64 idx);

		void EndFrame(uint32 displayWidth, uint32 displayHeight);

		double GPUProfileTiming(const char* name) const;

	protected:

		Array<ProfileData> profiles;
		Array<ProfileData> cpuProfiles;
		uint64 numProfiles = 0;
		uint64 numCPUProfiles = 0;
		Timer timer;
		ID3D12QueryHeap* queryHeap = nullptr;
		ReadbackBuffer readbackBuffer;
		bool enableGPUProfiling = false;
		bool showUI = false;
		bool logToClipboard = false;
	};

	class ProfileBlock
	{
	public:

		ProfileBlock(ID3D12GraphicsCommandList* cmdList, const char* name);
		~ProfileBlock();

	protected:

		ID3D12GraphicsCommandList* cmdList = nullptr;
		uint64 idx = uint64(-1);
	};

	class CPUProfileBlock
	{
	public:

		CPUProfileBlock(const char* name);
		~CPUProfileBlock();

	protected:

		uint64 idx = uint64(-1);
	};
}