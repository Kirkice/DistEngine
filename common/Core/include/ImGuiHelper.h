/* $Header: /common/Core/include/ImGuiHelper.h			8/25/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : ImGuiHelper.h											  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "../include/PCH.h"
#include "../../DistMath/include/DistMath.h"
#include "../../ImGUI/include/imgui.h"
using namespace math;

namespace core
{
	class Window;

	namespace ImGuiHelper
	{

		void Initialize(Window& window);
		void Shutdown();

		void CreatePSOs(DXGI_FORMAT rtFormat);
		void DestroyPSOs();

		void BeginFrame(uint32 displayWidth, uint32 displayHeight, float timeDelta);
		void EndFrame(ID3D12GraphicsCommandList* cmdList, D3D12_CPU_DESCRIPTOR_HANDLE rtv, 
			uint32 displayWidth, uint32 displayHeight);

	} // namespace ImGuiHelper

	inline ImVec2 ToImVec2(Vector2 v) 
	{
		return ImVec2(v.x, v.y);
	}

	inline Vector2 ToFloat2(ImVec2 v)
	{
		return Vector2(v.x, v.y);
	}

	inline ImColor ToImColor(Vector3 v)
	{
		return ImColor(v.x, v.y, v.z);
	}

	inline ImColor ToImColor(Vector4 v)
	{
		return ImColor(v.x, v.y, v.z, v.w);
	}
}