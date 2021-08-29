/* $Header: /common/Render/include/SkyBox.h	            8/24/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : SkyBox.h								                  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "../../Core/include/PCH.h"

#include "../../Core/include/InterfacePointers.h"
#include "../../DistMath/include/DistMath.h"
#include "../../Render/include/ShaderCompiler.h"
#include "../../Render/include/GraphicsTypes.h"
#include "../../Render/include/SphericalHarmonics.h"
#include "../../Render/include/SphericalGaussian.h"

struct ArHosekSkyModelState;

namespace render
{
#if EnableSkyModel_

	// Cached data for the procedural sky model
	struct SkyCache
	{
		ArHosekSkyModelState* StateR = nullptr;
		ArHosekSkyModelState* StateG = nullptr;
		ArHosekSkyModelState* StateB = nullptr;
		Vector3 SunDirection;
		Vector3 SunRadiance;
		Vector3 SunIrradiance;
		Vector3 SunRenderColor;
		float SunSize = 0.0f;
		float Turbidity = 0.0f;
		Vector3 Albedo;
		float Elevation = 0.0f;
		Texture CubeMap;
		SH9Color SH;
		SG9 SG;

		bool Init(const Vector3& sunDirection, float sunSize, const Vector3& groundAlbedo, float turbidity, bool createCubemap);
		void Shutdown();
		~SkyCache();

		bool Initialized() const { return StateR != nullptr; }

		Vector3 Sample(Vector3 sampleDir) const;
	};

#endif // EnableSkyModel_

	class Skybox
	{

	public:

		Skybox();
		~Skybox();

		void Initialize();
		void Shutdown();

		void CreatePSOs(DXGI_FORMAT rtFormat, DXGI_FORMAT depthFormat, uint32 numMSAASamples);
		void DestroyPSOs();

		void RenderEnvironmentMap(ID3D12GraphicsCommandList* cmdList,
			const Float4x4& view,
			const Float4x4& projection,
			const Texture* environmentMap,
			Vector3 scale = Vector3(1.0f, 1.0f, 1.0f));

#if EnableSkyModel_

		void RenderSky(ID3D12GraphicsCommandList* cmdList,
			const Float4x4& view,
			const Float4x4& projection,
			const SkyCache& skyCache,
			bool enableSun = true,
			const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f));

#endif // EnableSkyModel_

	protected:

		void RenderCommon(ID3D12GraphicsCommandList* cmdList,
			const Texture* environmentMap,
			const Float4x4& view,
			const Float4x4& projection,
			Vector3 scale);

		struct VSConstants 
		{
			Float4x4 View;
			Float4x4 Projection;
		};

		struct PSConstants
		{
			Vector3 SunDirection = Vector3(0.0f, 1.0f, 0.0f);
			float CosSunAngularRadius = 0.0f;
			Vector3 SunColor = 1.0f;
			uint32 Padding = 0;
			Vector3 Scale = 1.0f;
			uint32 EnvMapIdx = uint32(-1);
		};

		CompiledShaderPtr vertexShader;
		CompiledShaderPtr pixelShader;
		StructuredBuffer vertexBuffer;
		FormattedBuffer indexBuffer;
		VSConstants vsConstants;
		PSConstants psConstants;
		ID3D12RootSignature* rootSignature;
		ID3D12PipelineState* pipelineState;
	};
}