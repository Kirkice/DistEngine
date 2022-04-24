#include "Material.h"
#include "../Core/Mathf/Color.h"

namespace Dist
{
	/// <summary>
	/// PBRMaterial
	/// </summary>
	PBRMaterial::PBRMaterial()
	{
		Name = "New Material";
		MatCBIndex = -1;
		NumFramesDirty = 3;
		DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		Smoothness = 0.5f;
		Metallic = 0.0f;
		Occlusion = 1;
		EmissionStrength = 0;
		EmissionColor = Color(0.0f, 0.0f, 0.0f, 1.0f);

		MatTransform = Dist::Identity4x4();

		DiffuseMapIndex = 0;
		NormalMapIndex = 0;
		MsoMapIndex = 0;
		EmissionMapIndex = 0;
		LUTMapIndex = 0;
	}

	PBRMaterial::~PBRMaterial()
	{

	}
}