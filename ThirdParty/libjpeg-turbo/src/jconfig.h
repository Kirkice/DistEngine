/* Version ID for the JPEG library.
 * Might be useful for tests like "#if JPEG_LIB_VERSION >= 60".
 */
#define JPEG_LIB_VERSION  62	/* Version 6b */

/* libjpeg-turbo version */
#define LIBJPEG_TURBO_VERSION 0

/* Support arithmetic encoding */
#undef C_ARITH_CODING_SUPPORTED

/* Support arithmetic decoding */
#undef D_ARITH_CODING_SUPPORTED

/* Support in-memory source/destination managers */
#define MEM_SRCDST_SUPPORTED

/* Compiler supports function prototypes. */
#define HAVE_PROTOTYPES

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H

/* Compiler supports 'unsigned char'. */
#define HAVE_UNSIGNED_CHAR

/* Compiler supports 'unsigned short'. */
#define HAVE_UNSIGNED_SHORT

/* Compiler does not support pointers to unspecified structures. */
#undef INCOMPLETE_TYPES_BROKEN

/* Compiler has <strings.h> rather than standard <string.h>. */
#undef NEED_BSD_STRINGS

/* Linker requires that global names be unique in first 15 characters. */
#undef NEED_SHORT_EXTERNAL_NAMES

/* Need to include <sys/types.h> in order to obtain size_t. */
#undef NEED_SYS_TYPES_H

/* Broken compiler shifts signed values as an unsigned shift. */
#undef RIGHT_SHIFT_IS_UNSIGNED

/* Use accelerated SIMD routines. */
/* Don't enable NEON on ARM64 */
#if defined(UNITY_USE_NEON) && !(defined(__arm__) && defined(__LP64__))
# define WITH_SIMD
#endif

/* Define to 1 if type `char' is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
# undef __CHAR_UNSIGNED__
#endif

