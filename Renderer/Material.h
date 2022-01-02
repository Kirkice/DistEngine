#pragma once
#include "d3dApp.h"
#include "Mathf.h"

using namespace Mathf;

namespace Renderer
{
	//PBR_Matreials
	struct Material
	{
		std::string Name;
		int MatCBIndex = -1;
		int NumFramesDirty = gNumFrameResources;

		Vector4 SkyBoxTint = { 1.0f, 1.0f, 1.0f, 1.0f };
		float SkyBoxExposure = 1;
		float SkyBoxRotation = 0;
		float ACES = 1;

		Vector4 BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Smoothness = 0.5f;
		float Metallic = 0.5;
		float Occlusion = 0.0f;
		Vector4 EmissionColor = { 0,0,0,1 };
		float EmissionStrength = 0.0f;
		float ReceiveShadow = 0;
		XMFLOAT4X4 MatTransform = Mathf::Identity4x4();

		int DiffuseSrvHeapIndex = -1;
		int NormalSrvHeapIndex = -1;
		int MsoSrvHeapIndex = -1;
		int EmissionSrvHeapIndex = -1;
		int LUTSrvHeapIndex = -1;
	};
}