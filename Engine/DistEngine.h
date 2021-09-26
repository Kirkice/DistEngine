
/* $Header: /Engine/DistEngine.h						8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : DistEngine.h										      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "../common/Core/include/PCH.h"

#include "../common/Core/include/App.h"
#include "../common/Core/include/InterfacePointers.h"
#include "../common/Core/include/Input.h"
#include "../common/Render/include/Camera.h"
#include "../common/Render/include/Model.h"
#include "../common/Render/include/SkyBox.h"
#include "../common/Render/include/GraphicsTypes.h"

#include "PostProcessing.h"
#include "MeshRenderer.h"

using namespace render;

class DistEngine : public App
{

protected:

	FirstPersonCamera camera;

	Skybox skybox;
	SkyCache skyCache;

	PostProcessing postProcessor;

	// Model
	Model sceneModels[uint64(Scenes::NumValues)];
	const Model* currentModel = nullptr;
	MeshRenderer meshRenderer;

	RenderTexture mainTarget;
	RenderTexture resolveTarget;
	RenderTexture deferredMSAATarget;
	DepthBuffer depthBuffer;

	Array<SpotLight> spotLights;
	ConstantBuffer spotLightBuffer;
	StructuredBuffer spotLightBoundsBuffer;
	StructuredBuffer spotLightInstanceBuffer;
	RawBuffer spotLightClusterBuffer;
	uint64 numIntersectingSpotLights = 0;

	ID3D12RootSignature* clusterRS = nullptr;
	CompiledShaderPtr clusterVS;
	CompiledShaderPtr clusterFrontFacePS;
	CompiledShaderPtr clusterBackFacePS;
	CompiledShaderPtr clusterIntersectingPS;
	ID3D12PipelineState* clusterFrontFacePSO = nullptr;
	ID3D12PipelineState* clusterBackFacePSO = nullptr;
	ID3D12PipelineState* clusterIntersectingPSO = nullptr;
	RenderTexture clusterMSAATarget;

	StructuredBuffer spotLightClusterVtxBuffer;
	FormattedBuffer spotLightClusterIdxBuffer;
	Array<Vector3> coneVertices;

	CompiledShaderPtr fullScreenTriVS;
	CompiledShaderPtr resolvePS[NumMSAAModes];
	ID3D12RootSignature* resolveRootSignature = nullptr;
	ID3D12PipelineState* resolvePSO = nullptr;

	bool32 stablePowerState = false;

	// Ray tracing resources
	CompiledShaderPtr rayTraceLib;
	RenderTexture rtTarget;
	ID3D12RootSignature* rtRootSignature = nullptr;
	ID3D12StateObject* rtPSO = nullptr;
	bool buildAccelStructure = true;
	uint64 lastBuildAccelStructureFrame = uint64(-1);
	RawBuffer rtBottomLevelAccelStructure;
	RawBuffer rtTopLevelAccelStructure;
	StructuredBuffer rtRayGenTable;
	StructuredBuffer rtHitTable;
	StructuredBuffer rtMissTable;
	StructuredBuffer rtGeoInfoBuffer;
	FirstPersonCamera rtCurrCamera;
	bool rtShouldRestartPathTrace = false;
	uint32 rtCurrSampleIdx = 0;


	virtual void Initialize() override;
	virtual void Shutdown() override;

	virtual void Render(const Timer& timer) override;
	virtual void Update(const Timer& timer) override;

	virtual void BeforeReset() override;
	virtual void AfterReset() override;

	virtual void CreatePSOs() override;
	virtual void DestroyPSOs() override;

	void CreateRenderTargets();
	void InitializeScene();

	void InitRayTracing();
	void CreateRayTracingPSOs();

	void UpdateLights();

	void RenderClusters();
	void RenderForward();
	void RenderResolve();
	void RenderRayTracing();
	void RenderHUD(const Timer& timer);

	void BuildRTAccelerationStructure();

public:

	DistEngine(const wchar* cmdLine);
};