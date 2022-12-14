/*
 * ... this file ought really to have been a part of jpeglib to begin with.
 *
 * memsrc.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains decompression data source routines for the case of
 * reading JPEG data from a memory buffer that is preloaded with the entire
 * JPEG file.  This would not seem especially useful at first sight, but
 * a number of people have asked for it.
 * This is really just a stripped-down version of jdatasrc.c.  Comparison
 * of this code with jdatasrc.c may be helpful in seeing how to make
 * custom source managers for other purposes.
 */

/* this is not a core library module, so it doesn't define JPEG_INTERNALS */
#include "UnityPrefix.h"
#include "External/libjpeg-turbo/src/jinclude.h"
#include "External/libjpeg-turbo/src/jpeglib.h"
#include "External/libjpeg-turbo/src/jerror.h"
#include "ImageConversionJpegSrc.h"
#include "Modules/ImageConversion/Public/ImageConversion.h"

#if ENABLE_JPG

/* Expanded data source object for memory input */

#define JPEG_BUFFER_SIZE (1024*64)

struct my_source_mgr {
	jpeg_source_mgr pub;				// public fields
	ImageDecoder::UserContext* ctx;
	JOCTET buffer[JPEG_BUFFER_SIZE+1]; // +1 for fake EOI marker
};

/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
  /* No work, since jpeg_user_src set up the buffer pointer and count.
   * Indeed, if we want to read multiple JPEG images from one buffer,
   * this *must* not do anything to the pointer.
   */
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * But what we choose to do in this code is to supply dummy EOI markers
 * in order to force the decompressor to finish processing and supply
 * some sort of output image, no matter how corrupted.
 */

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
	my_source_mgr* src = (my_source_mgr*) cinfo->src;

	size_t read_len = src->ctx->func(src->ctx->context, src->buffer, JPEG_BUFFER_SIZE);
	if (read_len < JPEG_BUFFER_SIZE)
	{
		src->buffer[read_len] = (JOCTET) 0xFF;
		src->buffer[read_len+1] = (JOCTET) JPEG_EOI;
		read_len += 2;
	}
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = read_len;

	return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * If we overrun the end of the buffer, we let fill_input_buffer deal with
 * it.  An extremely large skip could cause some time-wasting here, but
 * it really isn't supposed to happen ... and the decompressor will never
 * skip more than 64K anyway.
 */

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	my_source_mgr* src = (my_source_mgr*) cinfo->src;

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			(void) fill_input_buffer(cinfo);
			/* note we assume that fill_input_buffer will never return FALSE,
			 * so suspension need not be handled.
			 */
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}


/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */


/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}


/*
 * Prepare for input from a memory buffer.
 */
void jpeg_user_src(j_decompress_ptr cinfo, ImageDecoder::UserContext* ctx)
{
  my_source_mgr* src;

  /* The source object is made permanent so that a series of JPEG images
   * can be read from a single buffer by calling jpeg_user_src
   * only before the first one.
   * This makes it unsafe to use this manager and a different source
   * manager serially with the same JPEG object.  Caveat programmer.
   */
  if (cinfo->src == NULL) {     /* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_large) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                  SIZEOF(my_source_mgr));
  }

  src = (my_source_mgr*) cinfo->src;
  src->ctx = ctx;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;

  fill_input_buffer(cinfo);
}

#endif
