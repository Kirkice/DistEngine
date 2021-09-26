
/* $Header: /Engine/PostProcessing.h					8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : PostProcessing.h										  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#pragma once

#include "../common/Core/include/PCH.h"
#include "../common/Render/include/GraphicsTypes.h"
#include "../common/Render/include/PostProcessingHelper.h"

#include "AppSettings.h"

using namespace render;

class PostProcessing
{

public:

	void Initialize();
	void Shutdown();

	void CreatePSOs();
	void DestroyPSOs();

	void Render(ID3D12GraphicsCommandList* cmdList, const RenderTexture& input, const RenderTexture& output);

protected:

	TempRenderTarget* Bloom(ID3D12GraphicsCommandList* cmdList, const RenderTexture& input);

	PostProcessingHelper helper;

	CompiledShaderPtr toneMap;
	CompiledShaderPtr scale;
	CompiledShaderPtr bloom;
	CompiledShaderPtr blurH;
	CompiledShaderPtr blurV;
};