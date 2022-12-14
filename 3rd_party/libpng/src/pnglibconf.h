/* pnglibconf.h - library build configuration */

/* libpng version 1.6.36 */

/* Copyright (c) 2018 Cosmin Truta */
/* Copyright (c) 1998-2002,2004,2006-2018 Glenn Randers-Pehrson */

/* This code is released under the libpng license. */
/* For conditions of distribution and use, see the disclaimer */
/* and license in png.h */

/* pnglibconf.h */
/* Machine generated file: DO NOT EDIT. UNITY-specific: edit as needed */
/* Derived from: scripts/pnglibconf.dfa */
#ifndef PNGLCONF_H
#define PNGLCONF_H

// UNITY-specific options
#if defined(UNITY_USE_NEON) && !(defined(__arm__) && defined(__LP64__))
  #define PNG_ARM_NEON_OPT 1
#else
  #define PNG_ARM_NEON_OPT 0
#endif

/* options */
#define PNG_16BIT_SUPPORTED
#define PNG_ALIGNED_MEMORY_SUPPORTED
/*#undef PNG_ARM_NEON_API_SUPPORTED*/
/*#undef PNG_ARM_NEON_CHECK_SUPPORTED*/
#define PNG_BENIGN_ERRORS_SUPPORTED
#define PNG_BENIGN_READ_ERRORS_SUPPORTED
/*#undef PNG_BENIGN_WRITE_ERRORS_SUPPORTED*/
#define PNG_BUILD_GRAYSCALE_PALETTE_SUPPORTED
#define PNG_CHECK_FOR_INVALID_INDEX_SUPPORTED
#define PNG_COLORSPACE_SUPPORTED
// #define PNG_CONSOLE_IO_SUPPORTED			// UNITY-specific: we want to handle error printing ourselves
#define PNG_CONVERT_tIME_SUPPORTED
#define PNG_EASY_ACCESS_SUPPORTED
/*#undef PNG_ERROR_NUMBERS_SUPPORTED*/
#define PNG_ERROR_TEXT_SUPPORTED
#define PNG_FIXED_POINT_SUPPORTED
#define PNG_FLOATING_ARITHMETIC_SUPPORTED
#define PNG_FLOATING_POINT_SUPPORTED
#define PNG_FORMAT_AFIRST_SUPPORTED
// #define PNG_FORMAT_BGR_SUPPORTED			// UNITY-specific: we don't support BGR
#define PNG_GAMMA_SUPPORTED
#define PNG_GET_PALETTE_MAX_SUPPORTED
#define PNG_HANDLE_AS_UNKNOWN_SUPPORTED
#define PNG_INCH_CONVERSIONS_SUPPORTED
// #define PNG_INFO_IMAGE_SUPPORTED			// UNITY-specific: we don't use high level API disabled by this
#define PNG_IO_STATE_SUPPORTED
#define PNG_MNG_FEATURES_SUPPORTED
#define PNG_POINTER_INDEXING_SUPPORTED
/*#undef PNG_POWERPC_VSX_API_SUPPORTED*/
/*#undef PNG_POWERPC_VSX_CHECK_SUPPORTED*/
// #define PNG_PROGRESSIVE_READ_SUPPORTED	// UNITY-specific: we don't read images progressively
#define PNG_READ_16BIT_SUPPORTED
#define PNG_READ_ALPHA_MODE_SUPPORTED
#define PNG_READ_ANCILLARY_CHUNKS_SUPPORTED
#define PNG_READ_BACKGROUND_SUPPORTED
// #if defined(LINUX) || defined(UNITY_EDITOR)	// UNITY: used only there, a candidate for disabling
#define PNG_READ_BGR_SUPPORTED
// #endif
#define PNG_READ_CHECK_FOR_INVALID_INDEX_SUPPORTED
#define PNG_READ_COMPOSITE_NODIV_SUPPORTED
#define PNG_READ_COMPRESSED_TEXT_SUPPORTED
// #define PNG_READ_EXPAND_16_SUPPORTED		// UNITY-specific: we don't expand to 16-bit channels
#define PNG_READ_EXPAND_SUPPORTED
#define PNG_READ_FILLER_SUPPORTED
#define PNG_READ_GAMMA_SUPPORTED
#define PNG_READ_GET_PALETTE_MAX_SUPPORTED
#define PNG_READ_GRAY_TO_RGB_SUPPORTED
#define PNG_READ_INTERLACING_SUPPORTED
#define PNG_READ_INT_FUNCTIONS_SUPPORTED
#define PNG_READ_INVERT_ALPHA_SUPPORTED
// #define PNG_READ_INVERT_SUPPORTED		// UNITY-specific: we don't invert images
#define PNG_READ_OPT_PLTE_SUPPORTED
// #define PNG_READ_PACKSWAP_SUPPORTED		// UNITY-specific: we don't support swapping bits in packed bytes
#define PNG_READ_PACK_SUPPORTED
// #define PNG_READ_QUANTIZE_SUPPORTED		// UNITY-specific: we don't quantize decoded images
// #define PNG_READ_RGB_TO_GRAY_SUPPORTED	// UNITY-specific: we don't convert RGB to gray
#define PNG_READ_SCALE_16_TO_8_SUPPORTED
#define PNG_READ_SHIFT_SUPPORTED
#define PNG_READ_STRIP_16_TO_8_SUPPORTED
#define PNG_READ_STRIP_ALPHA_SUPPORTED
#define PNG_READ_SUPPORTED
#define PNG_READ_SWAP_ALPHA_SUPPORTED
#define PNG_READ_SWAP_SUPPORTED
#define PNG_READ_TEXT_SUPPORTED
#define PNG_READ_TRANSFORMS_SUPPORTED
// #define PNG_READ_UNKNOWN_CHUNKS_SUPPORTED	// UNITY-specific: we discard unknown chunks
// #define PNG_READ_USER_CHUNKS_SUPPORTED		// UNITY-specific: we have no use for user chunks
#define PNG_READ_USER_TRANSFORM_SUPPORTED
#define PNG_READ_bKGD_SUPPORTED
#define PNG_READ_cHRM_SUPPORTED
#define PNG_READ_eXIf_SUPPORTED
#define PNG_READ_gAMA_SUPPORTED
// #define PNG_READ_hIST_SUPPORTED			// UNITY-specific: we have no use for histograms
#define PNG_READ_iCCP_SUPPORTED
#define PNG_READ_iTXt_SUPPORTED
#define PNG_READ_oFFs_SUPPORTED
#define PNG_READ_pCAL_SUPPORTED
#define PNG_READ_pHYs_SUPPORTED
#define PNG_READ_sBIT_SUPPORTED
#define PNG_READ_sCAL_SUPPORTED
#define PNG_READ_sPLT_SUPPORTED
#define PNG_READ_sRGB_SUPPORTED
// #define PNG_READ_tEXt_SUPPORTED			// UNITY-specific: we don't use text chunk
// #define PNG_READ_tIME_SUPPORTED			// UNITY-specific: we don't use image modification time chunk
#define PNG_READ_tRNS_SUPPORTED
// #define PNG_READ_zTXt_SUPPORTED			// UNITY-specific: we don't use text chunk
/*#undef PNG_SAFE_LIMITS_SUPPORTED*/
#define PNG_SAVE_INT_32_SUPPORTED
#define PNG_SAVE_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_SEQUENTIAL_READ_SUPPORTED
#define PNG_SETJMP_SUPPORTED
#define PNG_SET_CHUNK_CACHE_LIMIT_SUPPORTED
#define PNG_SET_CHUNK_MALLOC_LIMIT_SUPPORTED
#define PNG_SET_OPTION_SUPPORTED
#define PNG_SET_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_SET_USER_LIMITS_SUPPORTED
// #define PNG_SIMPLIFIED_READ_AFIRST_SUPPORTED	// UNITY-specific: we don't use simplified API
// #define PNG_SIMPLIFIED_READ_BGR_SUPPORTED	// UNITY-specific: we don't use simplified API
// #define PNG_SIMPLIFIED_READ_SUPPORTED		// UNITY-specific: we don't use simplified API
// #define PNG_SIMPLIFIED_WRITE_AFIRST_SUPPORTED // UNITY-specific: we don't use simplified API
// #define PNG_SIMPLIFIED_WRITE_BGR_SUPPORTED	// UNITY-specific: we don't use simplified API
// #define PNG_SIMPLIFIED_WRITE_SUPPORTED		// UNITY-specific: we don't use simplified API
// #ifdef LINUX									// UNITY-specific: candidate for disabling
#define PNG_STDIO_SUPPORTED
// #endif
//#define PNG_STORE_UNKNOWN_CHUNKS_SUPPORTED	// UNITY-specific: we don't store unknown chunks
#define PNG_TEXT_SUPPORTED
#define PNG_TIME_RFC1123_SUPPORTED
#define PNG_UNKNOWN_CHUNKS_SUPPORTED
//#undef PNG_USER_CHUNKS_SUPPORTED				// UNITY-specific: we don't handle user chunks in any way
#define PNG_USER_LIMITS_SUPPORTED
#define PNG_USER_MEM_SUPPORTED
#define PNG_USER_TRANSFORM_INFO_SUPPORTED
#define PNG_USER_TRANSFORM_PTR_SUPPORTED
// #define PNG_WARNINGS_SUPPORTED				// UNITY-specific: we don't use warnings
#define PNG_WRITE_16BIT_SUPPORTED
#define PNG_WRITE_ANCILLARY_CHUNKS_SUPPORTED
// #define PNG_WRITE_BGR_SUPPORTED				// UNITY-specific: we don't write BGR data
#define PNG_WRITE_CHECK_FOR_INVALID_INDEX_SUPPORTED
#define PNG_WRITE_COMPRESSED_TEXT_SUPPORTED
#define PNG_WRITE_CUSTOMIZE_COMPRESSION_SUPPORTED
#define PNG_WRITE_CUSTOMIZE_ZTXT_COMPRESSION_SUPPORTED
#define PNG_WRITE_FILLER_SUPPORTED
#define PNG_WRITE_FILTER_SUPPORTED
#define PNG_WRITE_FLUSH_SUPPORTED
#define PNG_WRITE_GET_PALETTE_MAX_SUPPORTED
// #define PNG_WRITE_INTERLACING_SUPPORTED		// UNITY-specific: we don't write interlaced images
#define PNG_WRITE_INT_FUNCTIONS_SUPPORTED
#define PNG_WRITE_INVERT_ALPHA_SUPPORTED
// #define PNG_WRITE_INVERT_SUPPORTED			// UNITY-specific: we don't invert images
#define PNG_WRITE_OPTIMIZE_CMF_SUPPORTED
// #define PNG_WRITE_PACKSWAP_SUPPORTED			// UNITY-specific: we don't use packswap
// #define PNG_WRITE_PACK_SUPPORTED				// UNITY-specific: we don't pack pixels
// #define PNG_WRITE_SHIFT_SUPPORTED			// UNITY-specific: we don't use write shift
#define PNG_WRITE_SUPPORTED
#define PNG_WRITE_SWAP_ALPHA_SUPPORTED
#define PNG_WRITE_SWAP_SUPPORTED
#define PNG_WRITE_TEXT_SUPPORTED
#define PNG_WRITE_TRANSFORMS_SUPPORTED
// #define PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED	// UNITY-specific: we don't write unknown chunks
#define PNG_WRITE_USER_TRANSFORM_SUPPORTED
// #define PNG_WRITE_WEIGHTED_FILTER_SUPPORTED	// UNITY-specific: we want the simpliest and fastest way to write png
#define PNG_WRITE_bKGD_SUPPORTED
#define PNG_WRITE_cHRM_SUPPORTED
#define PNG_WRITE_eXIf_SUPPORTED
#define PNG_WRITE_gAMA_SUPPORTED
// #define PNG_WRITE_hIST_SUPPORTED // UNITY-specific: we have no use for histograms
#define PNG_WRITE_iCCP_SUPPORTED
#define PNG_WRITE_iTXt_SUPPORTED
#define PNG_WRITE_oFFs_SUPPORTED
#define PNG_WRITE_pCAL_SUPPORTED
#define PNG_WRITE_pHYs_SUPPORTED
#define PNG_WRITE_sBIT_SUPPORTED
#define PNG_WRITE_sCAL_SUPPORTED
#define PNG_WRITE_sPLT_SUPPORTED
#define PNG_WRITE_sRGB_SUPPORTED
#define PNG_WRITE_tEXt_SUPPORTED
// #define PNG_WRITE_tIME_SUPPORTED // UNITY-specific: we don't use image modification time chunk
#define PNG_WRITE_tRNS_SUPPORTED
#define PNG_WRITE_zTXt_SUPPORTED
#define PNG_bKGD_SUPPORTED
#define PNG_cHRM_SUPPORTED
#define PNG_eXIf_SUPPORTED
#define PNG_gAMA_SUPPORTED
// #define PNG_hIST_SUPPORTED // UNITY-specific: we have no use for histograms
#define PNG_iCCP_SUPPORTED
#define PNG_iTXt_SUPPORTED
#define PNG_oFFs_SUPPORTED
#define PNG_pCAL_SUPPORTED
#define PNG_pHYs_SUPPORTED
#define PNG_sBIT_SUPPORTED
#define PNG_sCAL_SUPPORTED
#define PNG_sPLT_SUPPORTED
#define PNG_sRGB_SUPPORTED
#define PNG_tEXt_SUPPORTED
// #define PNG_tIME_SUPPORTED // UNITY-specific: we don't use image modification time chunk
#define PNG_tRNS_SUPPORTED
#define PNG_zTXt_SUPPORTED
/* end of options */
/* settings */
#define PNG_API_RULE 0
#define PNG_CALLOC_SUPPORTED
#define PNG_COST_SHIFT 3
#define PNG_DEFAULT_READ_MACROS 1
#define PNG_GAMMA_THRESHOLD_FIXED 5000
#define PNG_IDAT_READ_SIZE PNG_ZBUF_SIZE
#define PNG_INFLATE_BUF_SIZE 1024
#define PNG_LINKAGE_API extern
#define PNG_LINKAGE_CALLBACK extern
#define PNG_LINKAGE_DATA extern
#define PNG_LINKAGE_FUNCTION extern
#define PNG_MAX_GAMMA_8 11
#define PNG_QUANTIZE_BLUE_BITS 5
#define PNG_QUANTIZE_GREEN_BITS 5
#define PNG_QUANTIZE_RED_BITS 5
#define PNG_TEXT_Z_DEFAULT_COMPRESSION (-1)
#define PNG_TEXT_Z_DEFAULT_STRATEGY 0
#define PNG_USER_CHUNK_CACHE_MAX 1000
#define PNG_USER_CHUNK_MALLOC_MAX 8000000
#define PNG_USER_HEIGHT_MAX 1000000
#define PNG_USER_WIDTH_MAX 1000000
#define PNG_ZBUF_SIZE 8192
#define PNG_ZLIB_VERNUM 0 /* unknown */
#define PNG_Z_DEFAULT_COMPRESSION (-1)
#define PNG_Z_DEFAULT_NOFILTER_STRATEGY 0
#define PNG_Z_DEFAULT_STRATEGY 1
#define PNG_sCAL_PRECISION 5
#define PNG_sRGB_PROFILE_CHECKS 2
/* end of settings */
#endif /* PNGLCONF_H */
