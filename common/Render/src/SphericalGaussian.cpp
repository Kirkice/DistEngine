/* $Header: /common/Render/src/SphericalGaussian.cpp	8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : SphericalGaussian.cpp									  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#include "../../Core/include/PCH.h"

#if EnableEigen_

#pragma warning(push)
#pragma warning(disable : 4701)

#define EIGEN_MPL2_ONLY
#include "../Externals/eigen/Eigen/Dense"
#include "../Externals/eigen/Eigen/NNLS"

#include "../Externals/eigen/unsupported/Eigen/NonLinearOptimization"
#include "../Externals/eigen/unsupported/Eigen/NumericalDiff"

#pragma warning(pop)

#endif // EnableEigen_

#include "../../Render/include/SphericalGaussian.h"
#include "../../Render/include/Textures.h"
#include "../../Core/include/Containers.h"

namespace render
{
#if EnableEigen_

	struct EigenInitializer
	{
		EigenInitializer()
		{
			Eigen::initParallel();
		}
	};

	EigenInitializer initializer;

#endif // EnableEigen_

	// Generate uniform spherical gaussians on the sphere or hemisphere
	void GenerateUniformSGs(SG* outSGs, uint64 numSGs, SGDistribution distribution)
	{
		const uint64 N = distribution == SGDistribution::Hemispherical ? numSGs * 2 : numSGs;

		Vector3 meansStack[24];
		Array<Vector3> meansHeap;
		Vector3* means = meansStack;
		if (N > ArraySize_(meansStack))
		{
			meansHeap.Init(N);
			means = meansHeap.Data();
		}

		float inc = math::DIST_Pi * (3.0f - std::sqrt(5.0f));
		float off = 2.0f / N;
		for (uint64 k = 0; k < N; ++k)
		{
			float y = k * off - 1.0f + (off / 2.0f);
			float r = std::sqrt(1.0f - y * y);
			float phi = k * inc;
			means[k] = Vector3(std::cos(phi) * r, std::sin(phi) * r, y);
		}

		uint64 currSG = 0;
		for (uint64 i = 0; i < N; ++i)
		{
			// For the sphere we always accept the sample point but for the hemisphere we only accept
			// sample points on the correct side of the hemisphere
			if (distribution == SGDistribution::Spherical || Vector3::Dot(means[i].z, Vector3(0.0f, 0.0f, 1.0f)) >= 0.0f)
			{
				SG sample;
				sample.Axis = Vector3::Normalize(means[i]);
				outSGs[currSG++] = sample;
			}
		}

		float minDP = 1.0f;
		for (uint64 i = 1; i < numSGs; ++i)
		{
			Vector3 h = Vector3::Normalize(outSGs[i].Axis + outSGs[0].Axis);
			minDP = Min(minDP, Vector3::Dot(h, outSGs[0].Axis));
		}

		float sharpness = (std::log(0.65f) * numSGs) / (minDP - 1.0001f);

		for (uint32 i = 0; i < numSGs; ++i)
			outSGs[i].Sharpness = sharpness;
	}

#if EnableEigen_

	// Solve for SG's using non-negative least squares
	static void SolveNNLS(SGSolveParams& params)
	{
		Assert_(params.SampleDirs != nullptr);
		Assert_(params.SampleValues != nullptr);

		// -- Linearly solve for the rgb channels one at a time
		Eigen::MatrixXf Ar, Ag, Ab;
		Ar.resize(params.NumSamples, int64(params.NumSGs));
		Ag.resize(params.NumSamples, int64(params.NumSGs));
		Ab.resize(params.NumSamples, int64(params.NumSGs));
		Eigen::VectorXf br(params.NumSamples);
		Eigen::VectorXf bg(params.NumSamples);
		Eigen::VectorXf bb(params.NumSamples);
		for (uint32 i = 0; i < params.NumSamples; ++i)
		{
			// compute difference squared from actual observed data
			for (uint32 j = 0; j < params.NumSGs; ++j)
			{
				float exponent = exp((Vector3::Dot(params.SampleDirs[i], params.OutSGs[j].Axis) - 1.0f) *
					params.OutSGs[j].Sharpness);
				Ar(i, j) = exponent;
				Ag(i, j) = exponent;
				Ab(i, j) = exponent;
			}
			br(i) = params.SampleValues[i].x;
			bg(i) = params.SampleValues[i].y;
			bb(i) = params.SampleValues[i].z;
		}

		Eigen::NNLS<Eigen::MatrixXf> nnlsr(Ar);
		Eigen::NNLS<Eigen::MatrixXf> nnlsg(Ag);
		Eigen::NNLS<Eigen::MatrixXf> nnlsb(Ab);
		nnlsr.solve(br);
		nnlsg.solve(bg);
		nnlsb.solve(bb);
		Eigen::VectorXf rchan = nnlsr.x();
		Eigen::VectorXf gchan = nnlsg.x();
		Eigen::VectorXf bchan = nnlsb.x();

		for (uint32 j = 0; j < params.NumSGs; ++j)
		{
			params.OutSGs[j].Amplitude.x = rchan[j];
			params.OutSGs[j].Amplitude.y = gchan[j];
			params.OutSGs[j].Amplitude.z = bchan[j];
		}
	}

	// Solve for SG's using singular value decomposition
	static void SolveSVD(SGSolveParams& params)
	{
		// -- Linearly solve for the rgb channels one at a time
		Eigen::MatrixXf Ar, Ag, Ab;

		Ar.resize(params.NumSamples, params.NumSGs);
		Ag.resize(params.NumSamples, params.NumSGs);
		Ab.resize(params.NumSamples, params.NumSGs);
		Eigen::VectorXf br(params.NumSamples);
		Eigen::VectorXf bg(params.NumSamples);
		Eigen::VectorXf bb(params.NumSamples);
		for (uint32 i = 0; i < params.NumSamples; ++i)
		{
			// compute difference squared from actual observed data
			for (uint32 j = 0; j < params.NumSGs; ++j)
			{
				float exponent = std::exp((Vector3::Dot(params.SampleDirs[i], params.OutSGs[j].Axis) - 1.0f) *
					params.OutSGs[j].Sharpness);
				Ar(i, j) = exponent;
				Ag(i, j) = exponent;
				Ab(i, j) = exponent;
			}
			br(i) = params.SampleValues[i].x;
			bg(i) = params.SampleValues[i].y;
			bb(i) = params.SampleValues[i].z;
		}

		Eigen::VectorXf rchan = Ar.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(br);
		Eigen::VectorXf gchan = Ag.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(bg);
		Eigen::VectorXf bchan = Ab.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(bb);

		for (uint32 j = 0; j < params.NumSGs; ++j) {
			params.OutSGs[j].Amplitude.x = rchan[j];
			params.OutSGs[j].Amplitude.y = gchan[j];
			params.OutSGs[j].Amplitude.z = bchan[j];
		}
	}

#endif

	// Project sample onto SGs
	void ProjectOntoSGs(const Vector3& dir, const Vector3& color, SG* outSGs, uint64 numSGs)
	{
		for (uint64 i = 0; i < numSGs; ++i)
		{
			SG sg1;
			SG sg2;
			sg1.Amplitude = outSGs[i].Amplitude;
			sg1.Axis = outSGs[i].Axis;
			sg1.Sharpness = outSGs[i].Sharpness;
			sg2.Amplitude = color;
			sg2.Axis = Vector3::Normalize(dir);

			if (Vector3::Dot(dir, sg1.Axis) > 0.0f)
			{
				float dot = Vector3::Dot(sg1.Axis, sg2.Axis);
				float factor = (dot - 1.0f) * sg1.Sharpness;
				float wgt = exp(factor);
				outSGs[i].Amplitude += sg2.Amplitude * wgt;
			}
		}
	}

	// Do a projection of the colors onto the SG's
	static void SolveProjection(SGSolveParams& params)
	{
		Assert_(params.SampleDirs != nullptr);
		Assert_(params.SampleValues != nullptr);

		// Project color samples onto the SGs
		for (uint32 i = 0; i < params.NumSamples; ++i)
			ProjectOntoSGs(params.SampleDirs[i], params.SampleValues[i], params.OutSGs, params.NumSGs);

		// Weight the samples by the monte carlo factor for uniformly sampling the sphere/hemisphere
		float monteCarloFactor = ((2.0f * math::DIST_Pi) / params.NumSamples);
		if (params.Distribution == SGDistribution::Spherical)
			monteCarloFactor *= 2.0f;

		// Fudge factor to help correct the intensity from our bad projection algorithim
		if (params.Distribution == SGDistribution::Spherical && params.NumSGs == 9)
			monteCarloFactor *= math::DIST_Pi / 2.46373701f;

		for (uint32 i = 0; i < params.NumSGs; ++i)
			params.OutSGs[i].Amplitude *= monteCarloFactor;
	}

	// Solve the set of spherical gaussians based on input set of data
	void SolveSGs(SGSolveParams& params)
	{
		GenerateUniformSGs(params.OutSGs, params.NumSGs, params.Distribution);

#if EnableEigen_
		if (params.SolveMode == SGSolveMode::NNLS)
			SolveNNLS(params);
		else if (params.SolveMode == SGSolveMode::SVD)
			SolveSVD(params);
		else
			SolveProjection(params);
#else
		SolveProjection(params);
#endif
	}

	void SolveSGsForCubemap(const Texture& texture, SG* outSGs, uint64 numSGs, SGSolveMode solveMode)
	{
		Assert_(texture.Cubemap);
		Assert_(numSGs > 0);
		Assert_(outSGs != nullptr);

		TextureData<math::Vector4> textureData;
		GetTextureData(texture, textureData);
		Assert_(textureData.NumSlices == 6); 
		const uint32 width = textureData.Width;
		const uint32 height = textureData.Height;

		Array<Vector3> sampleDirs(width * height * 6);
		Array<Vector3> sampleValues(width * height * 6);
		for (uint32 face = 0; face < 6; ++face)
		{
			for (uint32 y = 0; y < height; ++y)
			{
				for (uint32 x = 0; x < width; ++x)
				{
					const uint32 idx = face * (width * height) + y * (width)+x;
					sampleValues[idx] = textureData.Texels[idx].To3D();
					sampleDirs[idx] = MapXYSToDirection(x, y, face, width, height);
				}
			}
		}

		SGSolveParams params;
		params.SampleDirs = sampleDirs.Data();
		params.SampleValues = sampleValues.Data();
		params.NumSamples = sampleDirs.Size();
		params.SolveMode = solveMode;
		params.Distribution = SGDistribution::Spherical;
		params.NumSGs = numSGs;
		params.OutSGs = outSGs;
		SolveSGs(params);
	}
}