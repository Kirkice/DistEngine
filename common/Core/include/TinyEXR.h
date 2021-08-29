/* $Header: /common/core/include/TinyEXR.h			         8/11/21 20:55p Kirk			$ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : TinyEXR.h											      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#ifndef __TINYEXR_H__
#define __TINYEXR_H__

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct {
		int num_channels;
		const char** channel_names;
		float** images; // image[channels][pixels]
		int width;
		int height;
	} EXRImage;

	typedef struct {
		int num_channels;
		const char** channel_names;
		float*** image;     // image[channels][scanlines][samples]
		int** offset_table; // offset_table[scanline][offsets]
		int width;
		int height;
	} DeepImage;

	// Loads single-frame OpenEXR image. Assume EXR image contains RGB(A) channels.
	// Application must free image data as returned by `out_rgba`
	// Result image format is: float x RGBA x width x hight
	// Return 0 if success
	// Returns error string in `err` when there's an error
	extern int LoadEXR(float** out_rgba, int* width, int* height,
		const char* filename, const char** err);

	// Loads multi-channel, single-frame OpenEXR image.
	// Application must free EXRImage
	// Return 0 if success
	// Returns error string in `err` when there's an error
	extern int LoadMultiChannelEXR(EXRImage* image, const char* filename,
		const char** err);

	// Saves floating point RGBA image as OpenEXR.
	// Image is compressed with ZIP.
	// Return 0 if success
	// Returns error string in `err` when there's an error
	//extern int SaveEXR(const float *in_rgba, int width, int height,
	//                   const char *filename, const char **err);

	// Saves multi-channel, single-frame OpenEXR image.
	// Application must free EXRImage
	// Return 0 if success
	// Returns error string in `err` when there's an error
	extern int SaveMultiChannelEXR(const EXRImage* image, const char* filename,
		const char** err);

	// Loads single-frame OpenEXR deep image.
	// Application must free memory of variables in DeepImage(image, offset_table)
	// Return 0 if success
	// Returns error string in `err` when there's an error
	extern int LoadDeepEXR(DeepImage* out_image, const char* filename,
		const char** err);

	// NOT YET IMPLEMENTED:
	// Saves single-frame OpenEXR deep image.
	// Return 0 if success
	// Returns error string in `err` when there's an error
	// extern int SaveDeepEXR(const DeepImage *in_image, const char *filename,
	//                       const char **err);

	// NOT YET IMPLEMENTED:
	// Loads multi-part OpenEXR deep image.
	// Application must free memory of variables in DeepImage(image, offset_table)
	// extern int LoadMultiPartDeepEXR(DeepImage **out_image, int num_parts, const
	// char *filename,
	//                       const char **err);

#ifdef __cplusplus
}
#endif

#endif // __TINYEXR_H__
