/* $Header: /common/Render/include/SphericalGaussian.h	  8/28/21 15:23pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : SphericalGaussian.h										  *
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
	struct Texture;

	// SphericalGaussian(dir) := Amplitude * exp(Sharpness * (dot(Axis, Direction) - 1.0f))
	struct SG
	{
		Vector3 Amplitude;
		Vector3 Axis;
		float Sharpness = 1.0f;
	};

	struct SG9
	{
		SG Lobes[9];
	};

	// Evaluates an SG given a direction on a unit sphere
	inline Vector3 EvaluateSG(const SG& sg, Vector3 dir)
	{
		return sg.Amplitude * std::exp(sg.Sharpness * (Vector3::Dot(dir, sg.Axis) - 1.0f));
	}

	// Computes the inner product of two SG's, which is equal to Integrate(SGx(v) * SGy(v) * dv).
	inline Vector3 SGInnerProduct(const SG& x, const SG& y)
	{
		float umLength = Vector3::Length(x.Sharpness * x.Axis + y.Sharpness * y.Axis);
		Vector3 expo = std::exp(umLength - x.Sharpness - y.Sharpness) * x.Amplitude * y.Amplitude;
		float other = 1.0f - std::exp(-2.0f * umLength);
		return (2.0f * math::DIST_Pi * expo * other) / umLength;
	}

	// Returns an approximation of the clamped cosine lobe represented as an SG
	inline SG CosineLobeSG(Vector3 direction)
	{
		SG cosineLobe;
		cosineLobe.Axis = direction;
		cosineLobe.Sharpness = 2.133f;
		cosineLobe.Amplitude = 1.17f;

		return cosineLobe;
	}

	// Computes the approximate integral of an SG over the entire sphere. The error vs. the
	// non-approximate version decreases as sharpeness increases.
	inline Vector3 ApproximateSGIntegral(const SG& sg)
	{
		return 2 * math::DIST_Pi * (sg.Amplitude / sg.Sharpness);
	}

	// Computes the approximate incident irradiance from a single SG lobe containing incoming radiance.
	// The irradiance is computed using a fitted approximation polynomial. This approximation
	// and its implementation were provided by Stephen Hill.
	inline Vector3 SGIrradianceFitted(const SG& lightingLobe, const Vector3& normal)
	{
		const float muDotN = Vector3::Dot(lightingLobe.Axis, normal);
		const float lambda = lightingLobe.Sharpness;

		const float c0 = 0.36f;
		const float c1 = 1.0f / (4.0f * c0);

		float eml = std::exp(-lambda);
		float em2l = eml * eml;
		float rl = 1.0f / lambda;

		float scale = 1.0f + 2.0f * em2l - rl;
		float bias = (eml - em2l) * rl - em2l;

		float x = std::sqrt(1.0f - scale);
		float x0 = c0 * muDotN;
		float x1 = c1 * x;

		float n = x0 + x1;

		float y = (std::abs(x0) <= x1) ? n * n / x : Saturate(muDotN);

		float normalizedIrradiance = scale * y + bias;

		return normalizedIrradiance * ApproximateSGIntegral(lightingLobe);
	}

	enum class SGSolveMode : uint32
	{
		NNLS,
		SVD,
		Projection
	};
	 
	enum class SGDistribution : uint32
	{
		Spherical,
		Hemispherical,
	};

	// Input parameters for the solve
	struct SGSolveParams
	{
		Vector3* SampleDirs = nullptr;
		Vector3* SampleValues = nullptr;
		uint64 NumSamples = 0;

		SGSolveMode SolveMode = SGSolveMode::NNLS;
		SGDistribution Distribution = SGDistribution::Spherical;

		uint64 NumSGs = 0;                              // number of SG's we want to solve for
		SG* OutSGs = nullptr;                           // output of final SG's we solve for
	};

	void GenerateUniformSGs(SG* outSGs, uint64 numSGs, SGDistribution distribution);

	// Solve for k-number of SG's based on a sphere or hemisphere of samples
	void SolveSGs(SGSolveParams& params);

	// Projects a sample onto a set of SG's
	void ProjectOntoSGs(const Vector3& dir, const Vector3& color, SG* outSGs, uint64 numSGs);

	void SolveSGsForCubemap(const Texture& texture, SG* outSGs, uint64 numSGs, SGSolveMode solveMode = SGSolveMode::NNLS);

}