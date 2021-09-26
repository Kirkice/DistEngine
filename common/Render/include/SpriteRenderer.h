/* $Header: /common/Render/include/SpriteFont.h			8/28/21 15:23pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : SpriteFont.h											  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "../../Core/include/PCH.h"

#include "../../Core/include/Expections.h"
#include "../../Core/include/Utility.h"
#include "../../Core/include/InterfacePointers.h"
#include "../../DistMath/include/DistMath.h"

#include "../../render/include/GraphicsTypes.h"
#include "../../render/include/ShaderCompiler.h"

using namespace math;

namespace render
{
	class SpriteFont;

	enum class SpriteFilterMode : uint64
	{
		Linear = 0,
		Point
	};

	enum class SpriteBlendMode : uint64
	{
		AlphaBlend = 0,
		Opaque,

		NumValues
	};

	struct SpriteTransform
	{
		Vector2 Position;
		Vector2 Scale = Vector2(1.0f, 1.0f);
		Vector2 SinCosRotation = Vector2(0.0f, 1.0f);

		SpriteTransform()
		{
		}

		explicit SpriteTransform(Vector2 position) : Position(position)
		{
		}

		explicit SpriteTransform(Vector2 position, float rotation) : Position(position)
		{
			SinCosRotation = Vector2(std::sin(rotation), std::cos(rotation));
		}
	};

	struct SpriteDrawData
	{
		SpriteTransform Transform;
		math::Vector4 Color;
		math::Vector4 DrawRect;
	};

	class SpriteRenderer
	{

	public:

		static const uint64 MaxBatchSize = 1024;

		SpriteRenderer();
		~SpriteRenderer();

		void Initialize();
		void Shutdown();

		void CreatePSOs(DXGI_FORMAT rtFormat);
		void DestroyPSOs();

		void Begin(ID3D12GraphicsCommandList* cmdList, Vector2 viewportSize, SpriteFilterMode filterMode = SpriteFilterMode::Linear,
			SpriteBlendMode = SpriteBlendMode::AlphaBlend);

		void Render(ID3D12GraphicsCommandList* cmdList,
			const Texture* texture,
			const SpriteTransform& transform,
			const math::Vector4& color = math::Vector4(1, 1, 1, 1),
			const Vector4* drawRect = NULL);

		void RenderBatch(ID3D12GraphicsCommandList* cmdList,
			const Texture* texture,
			const SpriteDrawData* drawData, 
			uint64 numSprites);

		void RenderText(ID3D12GraphicsCommandList* cmdList, 
			const SpriteFont& font,
			const wchar* text,
			Vector2 position,
			const math::Vector4& color = math::Vector4(1, 1, 1, 1));

		void End();

	protected:

		ShaderPtr vertexShader;
		ShaderPtr vertexShaderInstanced;
		ShaderPtr pixelShader;
		ShaderPtr pixelShaderOpaque;
		FormattedBuffer indexBuffer;
		StructuredBuffer instanceDataBuffer;
		SpriteDrawData textDrawData[MaxBatchSize];
		ID3D12PipelineState* pipelineStates[uint64(SpriteBlendMode::NumValues)] = { };
		ID3D12RootSignature* rootSignature = nullptr;
		Texture defaultTexture;

		struct PerBatchConstants
		{
			Vector2 TextureSize;
			Vector2 ViewportSize;
			bool32 LinearSampling = false;
		} perBatchData;
	};
}