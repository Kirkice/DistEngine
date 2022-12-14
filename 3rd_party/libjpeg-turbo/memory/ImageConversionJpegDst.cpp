/*
 * jdatadst.c
 *
 * This file was part of the Independent JPEG Group's software:
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * For conditions of distribution and use, see the accompanying README file.
 */
#include <stdio.h>
#include "External/libjpeg-turbo/src/jpeglib.h"
#include "ImageConversionJpegDst.h"

/*
 This a custom destination manager for jpeglib that
 enables the use of memory to memory compression.

 See IJG documentation for details.
 */

/* ------------------------------------------------------------- */
/*	 MEMORY DESTINATION INTERFACE METHODS	 */
/* ------------------------------------------------------------- */


/* This function is called by the library before any data gets written */
METHODDEF(void)
dst_init_destination (j_compress_ptr cinfo)
{
	mem_dest_ptr dest = (mem_dest_ptr)cinfo->dest;

	dest->pub.next_output_byte = dest->buffer;	 /* set destination buffer */
	dest->pub.free_in_buffer	= dest->bufsize;	/* input buffer size */
	dest->datasize = 0;	 /* reset output size */
	dest->errcount = 0;	 /* reset error count */
}

/* This function is called by the library if the buffer fills up

 I just reset destination pointer and buffer size here.
 Note that this behavior, while preventing seg faults
 will lead to invalid output streams as data is over-
 written.
 */
METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo)
{
	mem_dest_ptr dest = (mem_dest_ptr)cinfo->dest;

	if(dest->emptyufunc)
		dest->emptyufunc(dest->udata, dest->buffer, dest->bufsize);
	else
		++dest->errcount;	/* need to increase error count */

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = dest->bufsize;

	return TRUE;
}

/* Usually the library wants to flush output here.

 I will calculate output buffer size here.
 Note that results become incorrect, once
 empty_output_buffer was called.
 This situation is notified by errcount.
 */
METHODDEF(void)
term_destination (j_compress_ptr cinfo)
{
	mem_dest_ptr dest = (mem_dest_ptr)cinfo->dest;
	dest->datasize = dest->bufsize - dest->pub.free_in_buffer;
	if (dest->outsize) *dest->outsize += (int)dest->datasize;
}

/* Override the default destination manager initialization
 provided by jpeglib. Since we want to use memory-to-memory
 compression, we need to use our own destination manager.
 */
GLOBAL(void)
jpeg_memory_dest (j_compress_ptr cinfo, JOCTET* buffer, int bufsize, int* outsize)
{
	mem_dest_ptr dest;

	/* first call for this instance - need to setup */
	if (cinfo->dest == 0) {
		cinfo->dest = (struct jpeg_destination_mgr *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
									sizeof (memory_destination_mgr));
	}

	dest = (mem_dest_ptr) cinfo->dest;
	dest->bufsize = bufsize;
	dest->buffer = buffer;
	dest->outsize = outsize;
	dest->udata = 0;
	dest->emptyufunc = 0;
	/* set method callbacks */
	dest->pub.init_destination = dst_init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
}

// same but with callback
GLOBAL(void)
jpeg_memory_dest (j_compress_ptr cinfo, JOCTET* buffer, int bufsize, int* outsize, void* udata, EmptyCallbackFunc emptyufunc)
{
	mem_dest_ptr dest;

	/* first call for this instance - need to setup */
	if (cinfo->dest == 0) {
		cinfo->dest = (struct jpeg_destination_mgr *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
									sizeof (memory_destination_mgr));
	}

	dest = (mem_dest_ptr) cinfo->dest;
	dest->bufsize = bufsize;
	dest->buffer = buffer;
	dest->outsize = outsize;
	dest->udata = udata;
	dest->emptyufunc = emptyufunc;
	/* set method callbacks */
	dest->pub.init_destination = dst_init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
}
