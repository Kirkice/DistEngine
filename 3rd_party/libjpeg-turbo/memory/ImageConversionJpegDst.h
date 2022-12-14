#ifndef IMAGE_CONVERSION_JPEG_DST_H
#define IMAGE_CONVERSION_JPEG_DST_H

#include "External/libjpeg-turbo/src/jpeglib.h"

typedef void (*EmptyCallbackFunc)(void* udata, void* buf, int bufsize);

typedef struct {
	struct jpeg_destination_mgr pub;	/* base class */
	JOCTET*	buffer;	 /* buffer start address */
	int	 bufsize;	 /* size of buffer */
	size_t	datasize;	 /* final size of compressed data */
	int*	outsize;	 /* user pointer to datasize */
	int	 errcount;	 /* counts up write errors due to
	 buffer overruns */

	void*		 		udata;
	EmptyCallbackFunc	emptyufunc;
} memory_destination_mgr;

typedef memory_destination_mgr* mem_dest_ptr;

GLOBAL(void)
jpeg_memory_dest(j_compress_ptr cinfo, JOCTET* buffer, int bufsize, int* outsize, void* udata, EmptyCallbackFunc emptyufunc);
GLOBAL(void)
jpeg_memory_dest(j_compress_ptr cinfo, JOCTET* buffer, int bufsize, int* outsize);

#endif
