/* $Header: /common/Render/include/BRDF.h	            8/28/21 13:13pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : BRDF.h													  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "../../Core/include/PCH.h"
#include "../../DistMath/include/DistMath.h"
using namespace math;

namespace render
{
	// Calculates the Fresnel factor using Schlick's approximation
	inline Vector3 Fresnel(Vector3 specAlbedo, Vector3 h, Vector3 l)
	{
		Vector3 fresnel = specAlbedo + (Vector3(1.0f) - specAlbedo) * std::pow((1.0f - Saturate(Vector3::Dot(l, h))), 5.0f);

		// Fade out spec entirely when lower than 0.1% albedo
		fresnel *= Saturate(Vector3::Dot(specAlbedo, 333.0f));

		return fresnel;
	}

	// Calculates the Fresnel factor using Schlick's approximation
	inline Vector3 Fresnel(Vector3 specAlbedo, Vector3 fresnelAlbedo, Vector3 h, Vector3 l) 
	{
		Vector3 fresnel = specAlbedo + (fresnelAlbedo - specAlbedo) * std::pow((1.0f - Saturate(Vector3::Dot(l, h))), 5.0f);

		// Fade out spec entirely when lower than 0.1% albedo
		fresnel *= Saturate(Vector3::Dot(specAlbedo, 333.0f));

		return fresnel;
	}
	 
	// Helper for computing the GGX visibility term
	inline float GGX_V1(float m2, float nDotX)
	{
		return 1.0f / (nDotX + sqrt(m2 + (1 - m2) * nDotX * nDotX));
	}

	// Returns the PDF for a particular GGX sample
	inline float GGX_PDF(const Vector3& n, const Vector3& h, const Vector3& v, float roughness)
	{
		float nDotH = Saturate(Vector3::Dot(n, h));
		float hDotV = Saturate(Vector3::Dot(h, v));
		float m2 = roughness * roughness;
		float d = m2 / (math::DIST_Pi * Square(nDotH * nDotH * (m2 - 1) + 1));
		float pM = d * nDotH;
		return pM / (4 * hDotV);
	}

	// Computes the specular term using a GGX microfacet distribution, with a matching
// geometry factor and visibility term. Based on "Microfacet Models for Refraction Through
// Rough Surfaces" [Walter 07]. m is roughness, n is the surface normal, h is the half vector,
// l is the direction to the light source, and specAlbedo is the RGB specular albedo
	inline float GGX_Specular(float m, const Vector3& n, const Vector3& h, const Vector3& v, const Vector3& l)
	{
		float nDotH = Saturate(Vector3::Dot(n, h));
		float nDotL = Saturate(Vector3::Dot(n, l));
		float nDotV = Saturate(Vector3::Dot(n, v)); 

		float m2 = m * m;

		// Calculate the distribution term
		float d = m2 / (math::DIST_Pi * Square(nDotH * nDotH * (m2 - 1) + 1));

		// Calculate the matching visibility term
		float v1i = GGX_V1(m2, nDotL);
		float v1o = GGX_V1(m2, nDotV);
		float vis = v1i * v1o;

		return d * vis;
	}
	// Computes the radiance reflected off a surface towards the eye given
	// the differential irradiance from a given direction
	inline Vector3 CalcLighting(const Vector3& normal, const Vector3& lightIrradiance,
		const Vector3& lightDir, const Vector3& diffuseAlbedo, const Vector3& position,
		const Vector3& cameraPos, float roughness, bool includeSpecular, Vector3 specAlbedo,
		const Vector3& msEnergyCompensation)
	{
		Vector3 lighting = diffuseAlbedo * math::DIST_InvPi;

		if (includeSpecular && Vector3::Dot(normal, lightDir) > 0.0f)
		{
			Vector3 view = Vector3::Normalize(cameraPos - position);
			const float nDotV = Saturate(Vector3::Dot(normal, view));
			Vector3 h = Vector3::Normalize(view + lightDir);

			Vector3 fresnel = Fresnel(specAlbedo, h, lightDir);

			float specular = GGX_Specular(roughness, normal, h, view, lightDir);
			lighting += specular * fresnel * msEnergyCompensation;
		}

		return lighting * lightIrradiance;
	}
}
