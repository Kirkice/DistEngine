#ifndef JPEGMEMSRC_H
#define JPEGMEMSRC_H

#include "External/libjpeg-turbo/src/jinclude.h"
#include "External/libjpeg-turbo/src/jpeglib.h"
#include "External/libjpeg-turbo/src/jerror.h"
#include "Modules/ImageConversion/Public/ImageConversion.h"

#if ENABLE_JPG
void jpeg_user_src (j_decompress_ptr cinfo, ImageDecoder::UserContext* ctx);
#endif

#endif
