/* $Header: /common/Render/src/PostProcessingHelper.cpp	   8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : PostProcessingHelper.cpp								  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#include "../../Core/include/PCH.h"

#include "../../Render/include/PostProcessingHelper.h"
#include "../../Render/include/ShaderCompiler.h"
#include "../../Render/include/D3D12.h"

#include "../../Core/include/Utility.h"

namespace AppSettings
{
	const extern uint32 CBufferRegister;
	void BindCBufferGfx(ID3D12GraphicsCommandList* cmdList, uint32 rootParameter);
}

namespace render
{
	static const uint32 MaxInputs = 8;

	enum RootParams : uint32
	{
		RootParam_StandardDescriptors,
		RootParam_SRVIndices,
		RootParam_AppSettings,

		NumRootParams,
	};

	PostProcessingHelper::PostProcessingHelper()
	{
	}

	PostProcessingHelper::~PostProcessingHelper()
	{
		Assert_(tempRenderTargets.Count() == 0);
		Assert_(pipelineStates.Count() == 0);
	}

	void PostProcessingHelper::Initialize()
	{
		// Load the shaders
		std::wstring fullScreenTriPath = SampleFrameworkDir() + L"Shaders\\FullScreenTriangle.hlsl";
		fullScreenTriVS = CompileFromFile(fullScreenTriPath.c_str(), "FullScreenTriangleVS", ShaderType::Vertex);

		{
			D3D12_ROOT_PARAMETER1 rootParameters[NumRootParams] = {};

			rootParameters[RootParam_StandardDescriptors].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[RootParam_StandardDescriptors].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootParameters[RootParam_StandardDescriptors].DescriptorTable.pDescriptorRanges = d3d12::StandardDescriptorRanges();
			rootParameters[RootParam_StandardDescriptors].DescriptorTable.NumDescriptorRanges = d3d12::NumStandardDescriptorRanges; 

			rootParameters[RootParam_SRVIndices].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[RootParam_SRVIndices].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootParameters[RootParam_SRVIndices].Descriptor.RegisterSpace = 0;
			rootParameters[RootParam_SRVIndices].Descriptor.ShaderRegister = 0;

			rootParameters[RootParam_AppSettings].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[RootParam_AppSettings].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootParameters[RootParam_AppSettings].Descriptor.RegisterSpace = 0;
			rootParameters[RootParam_AppSettings].Descriptor.ShaderRegister = AppSettings::CBufferRegister;

			D3D12_STATIC_SAMPLER_DESC staticSamplers[4] = {};
			staticSamplers[0] = d3d12::GetStaticSamplerState(SamplerState::Point, 0);
			staticSamplers[1] = d3d12::GetStaticSamplerState(SamplerState::LinearClamp, 1);
			staticSamplers[2] = d3d12::GetStaticSamplerState(SamplerState::Linear, 2);
			staticSamplers[3] = d3d12::GetStaticSamplerState(SamplerState::LinearBorder, 3);

			D3D12_ROOT_SIGNATURE_DESC1 rootSignatureDesc = {};
			rootSignatureDesc.NumParameters = ArraySize_(rootParameters);
			rootSignatureDesc.pParameters = rootParameters;
			rootSignatureDesc.NumStaticSamplers = ArraySize_(staticSamplers);
			rootSignatureDesc.pStaticSamplers = staticSamplers;
			rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

			d3d12::CreateRootSignature(&rootSignature, rootSignatureDesc);
			rootSignature->SetName(L"PostProcessingHelper");
		}
	}

	void PostProcessingHelper::Shutdown()
	{
		ClearCache();

		d3d12::Release(rootSignature);
	}

	void PostProcessingHelper::ClearCache()
	{
		for (uint64 i = 0; i < tempRenderTargets.Count(); ++i)
		{
			TempRenderTarget* tempRT = tempRenderTargets[i];
			tempRT->RT.Shutdown();
			delete tempRT;
		}

		tempRenderTargets.RemoveAll(nullptr);

		for (uint64 i = 0; i < pipelineStates.Count(); ++i)
			d3d12::DeferredRelease(pipelineStates[i].PSO);

		pipelineStates.RemoveAll(CachedPSO());
	}

	TempRenderTarget* PostProcessingHelper::GetTempRenderTarget(uint64 width, uint64 height, DXGI_FORMAT format, bool useAsUAV)
	{
		for (uint64 i = 0; i < tempRenderTargets.Count(); ++i)
		{
			TempRenderTarget* tempRT = tempRenderTargets[i];
			if (tempRT->InUse)
				continue;

			const RenderTexture& rt = tempRT->RT;
			if (rt.Texture.Width == width && rt.Texture.Height == height && rt.Texture.Format == format && useAsUAV == (rt.UAV.ptr != 0)) {
				tempRT->InUse = true;
				return tempRT;
			}
		}

		RenderTextureInit rtInit;
		rtInit.Width = width;
		rtInit.Height = height;
		rtInit.Format = format;
		rtInit.CreateUAV = useAsUAV;

		TempRenderTarget* tempRT = new TempRenderTarget();
		tempRT->RT.Initialize(rtInit);
		tempRT->RT.Texture.Resource->SetName(L"PP Temp Render Target");
		tempRT->InUse = true;
		tempRenderTargets.Add(tempRT);

		return tempRT;
	}

	void PostProcessingHelper::Begin(ID3D12GraphicsCommandList* cmdList_)
	{
		Assert_(cmdList == nullptr);
		cmdList = cmdList_;
	}

	void PostProcessingHelper::End()
	{
		Assert_(cmdList != nullptr);
		cmdList = nullptr;

		for (uint64 i = 0; i < tempRenderTargets.Count(); ++i)
			Assert_(tempRenderTargets[i]->InUse == false);
	}

	void PostProcessingHelper::PostProcess(CompiledShaderPtr pixelShader, const char* name, const RenderTexture& input, const RenderTexture& output)
	{
		uint32 inputs[1] = { input.SRV() };
		const RenderTexture* outputs[1] = { &output };
		PostProcess(pixelShader, name, inputs, 1, outputs, 1);
	}

	void PostProcessingHelper::PostProcess(CompiledShaderPtr pixelShader, const char* name, const RenderTexture& input, const TempRenderTarget* output)
	{
		uint32 inputs[1] = { input.SRV() };
		const RenderTexture* outputs[1] = { &output->RT };
		PostProcess(pixelShader, name, inputs, 1, outputs, 1);
	}

	void PostProcessingHelper::PostProcess(CompiledShaderPtr pixelShader, const char* name, const TempRenderTarget* input, const RenderTexture& output)
	{
		uint32 inputs[1] = { input->RT.SRV() };
		const RenderTexture* outputs[1] = { &output };
		PostProcess(pixelShader, name, inputs, 1, outputs, 1);
	}

	void PostProcessingHelper::PostProcess(CompiledShaderPtr pixelShader, const char* name, const TempRenderTarget* input, const TempRenderTarget* output)
	{
		uint32 inputs[1] = { input->RT.SRV() };
		const RenderTexture* outputs[1] = { &output->RT };
		PostProcess(pixelShader, name, inputs, 1, outputs, 1);
	}

	struct HashSource
	{
		DXGI_FORMAT OutputFormats[8] = { };
		uint64 MSAASamples = 0;
	};

	void PostProcessingHelper::PostProcess(CompiledShaderPtr pixelShader, const char* name, const uint32* inputs, uint64 numInputs,
		const RenderTexture* const* outputs, uint64 numOutputs)
	{
		Assert_(cmdList != nullptr);
		Assert_(numOutputs > 0);
		Assert_(outputs != nullptr);
		Assert_(numInputs == 0 || inputs != nullptr);
		Assert_(numInputs <= MaxInputs);

		PIXMarker marker(cmdList, name);

		HashSource hashSource;
		for (uint64 i = 0; i < numOutputs; ++i)
		{
			hashSource.OutputFormats[i] = outputs[i]->Texture.Format;
			hashSource.MSAASamples = outputs[i]->MSAASamples;
		}

		Hash psoHash = GenerateHash(&hashSource, sizeof(HashSource));
		psoHash = CombineHashes(psoHash, pixelShader->ByteCodeHash);

		ID3D12PipelineState* pso = nullptr;

		// The most linear of searches!
		const uint64 numPSOs = pipelineStates.Count();
		for (uint64 i = 0; i < numPSOs; ++i)
		{
			if (pipelineStates[i].Hash == psoHash)
			{
				pso = pipelineStates[i].PSO;
				break;
			}
		}

		if (pso == nullptr)
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = rootSignature;
			psoDesc.VS = fullScreenTriVS.ByteCode();
			psoDesc.PS = pixelShader.ByteCode();
			psoDesc.RasterizerState = d3d12::GetRasterizerState(RasterizerState::NoCull);
			psoDesc.BlendState = d3d12::GetBlendState(BlendState::Disabled);
			psoDesc.DepthStencilState = d3d12::GetDepthState(DepthState::Disabled);
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = uint32(numOutputs);
			for (uint64 i = 0; i < numOutputs; ++i)
				psoDesc.RTVFormats[i] = hashSource.OutputFormats[i];
			psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
			psoDesc.SampleDesc.Count = uint32(hashSource.MSAASamples);
			DXCall(d3d12::Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

			CachedPSO cachedPSO;
			cachedPSO.Hash = psoHash;
			cachedPSO.PSO = pso;
			pipelineStates.Add(cachedPSO);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[8] = { };
		for (uint64 i = 0; i < numOutputs; ++i)
			rtvHandles[i] = outputs[i]->RTV;
		cmdList->OMSetRenderTargets(uint32(numOutputs), rtvHandles, false, nullptr);

		cmdList->SetGraphicsRootSignature(rootSignature);
		cmdList->SetPipelineState(pso);

		d3d12::BindStandardDescriptorTable(cmdList, RootParam_StandardDescriptors, CmdListMode::Graphics);

		AppSettings::BindCBufferGfx(cmdList, RootParam_AppSettings);

		uint32 srvIndices[MaxInputs] = { };
		for (uint64 i = 0; i < numInputs; ++i)
			srvIndices[i] = inputs[i];
		for (uint64 i = numInputs; i < MaxInputs; ++i)
			srvIndices[i] = d3d12::NullTexture2DSRV;

		d3d12::BindTempConstantBuffer(cmdList, srvIndices, RootParam_SRVIndices, CmdListMode::Graphics);

		d3d12::SetViewport(cmdList, outputs[0]->Texture.Width, outputs[0]->Texture.Height);

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->DrawInstanced(3, 1, 0, 0);

	}
}