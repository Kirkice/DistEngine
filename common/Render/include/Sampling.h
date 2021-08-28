/* $Header: /common/Render/include/Sampling.h	          8/28/21 15:23pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Sampling.h												  *
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
	// Shape sampling functions 
	Vector2 SquareToConcentricDiskMapping(float x, float y, float numSides, float polygonAmount);
	Vector2 SquareToConcentricDiskMapping(float x, float y);
	Vector3 SampleDirectionGGX(const Vector3& v, const Vector3& n, float roughness, const Float3x3& tangentToWorld, float u1, float u2);
	Vector3 SampleSphere(float x1, float x2, float x3, float u1);
	Vector3 SampleDirectionSphere(float u1, float u2);
	Vector3 SampleDirectionHemisphere(float u1, float u2);
	Vector3 SampleDirectionCosineHemisphere(float u1, float u2);
	Vector3 SampleDirectionCone(float u1, float u2, float cosThetaMax);
	Vector3 SampleDirectionRectangularLight(float u1, float u2, const Vector3& sourcePos,
		const Vector2& lightSize, const Vector3& lightPos,
		const Quaternion lightOrientation, float& distanceToLight);

	// PDF functions
	float SampleDirectionGGX_PDF(const Vector3& n, const Vector3& h, const Vector3& v, float roughness);
	float SampleDirectionSphere_PDF();
	float SampleDirectionHemisphere_PDF();
	float SampleDirectionCosineHemisphere_PDF(float cosTheta);
	float SampleDirectionCosineHemisphere_PDF(const Vector3& normal, const Vector3& sampleDir);
	float SampleDirectionCone_PDF(float cosThetaMax);
	float SampleDirectionRectangularLight_PDF(const Vector2& lightSize, const Vector3& sampleDir,
		const Quaternion lightOrientation, float distanceToLight);

	// Random sample generation
	Vector2 Hammersley2D(uint64 sampleIdx, uint64 numSamples);
	Vector2 SampleCMJ2D(uint32 sampleIdx, uint32 numSamplesX, uint32 numSamplesY, uint32 pattern);

	// Full random sample set generation
	void GenerateRandomSamples2D(Vector2* samples, uint64 numSamples, Random& randomGenerator);
	void GenerateStratifiedSamples2D(Vector2* samples, uint64 numSamplesX, uint64 numSamplesY, Random& randomGenerator);
	void GenerateGridSamples2D(Vector2* samples, uint64 numSamplesX, uint64 numSamplesY);
	void GenerateHammersleySamples2D(Vector2* samples, uint64 numSamples);
	void GenerateHammersleySamples2D(Vector2* samples, uint64 numSamples, uint64 dimIdx);
	void GenerateLatinHypercubeSamples2D(Vector2* samples, uint64 numSamples, Random& rng);
	void GenerateCMJSamples2D(Vector2* samples, uint64 numSamplesX, uint64 numSamplesY, uint32 pattern);

	// Helpers
	float RadicalInverseBase2(uint32 bits);
	float RadicalInverseFast(uint64 baseIndex, uint64 index);
}