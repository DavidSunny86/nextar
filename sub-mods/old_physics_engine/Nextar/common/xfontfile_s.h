//@ Abhishek Dey
//@ (created: sometimes during the xFontTool creation)
//@ Nov 24, 07

#ifndef X_FONT_FILE_HEADER
#define X_FONT_FILE_HEADER

#include "xfile_s.h"

#undef ALIGN_STRUCT
#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define ALIGN_STRUCT
#elif defined( __GNUC__ )
#	define ALIGN_STRUCT	__attribute__(aligned(1))
#else
#	error "Unsupported compiler"
#endif

#ifdef __cplusplus
extern "C" {
typedef unsigned char boolean;
#ifndef true
#define true  1
#endif
#ifndef false
#define false 0
#endif
#endif
/* Character info structure */
struct char_info
{
	short	code; /* ascii for characters, else user specific code */
	short	width;
	float	u0,v0,u1,v1;
}ALIGN_STRUCT;
/* iEnd Character info structure */
//@ TODO:: The current font support is not satisfactory.
//@ a font file by complete definition should also include support
//@ for underlining and italizing the font.
/* font header */
struct font_header
{
	unsigned long		texture_size;	/* texture size in bytes */
	unsigned long		char_buffer_size;/* character buffer size in bytes */
	unsigned long		bits_per_pix;	/* BPP: usually 16 */
	unsigned long		texture_height;
	unsigned long		texture_width;
	unsigned long		char_count;
	  signed long		char_height;
		  boolean		is_italized;
		  boolean		is_underlined;
		  boolean		is_strokeout;
		  boolean		has_symbols;
		  boolean		has_smoothing; /* in which case 32 bit textures will be needed */
}ALIGN_STRUCT;
/* iEnd font header */

typedef struct char_info char_info;
typedef struct font_header font_header;
#ifdef __cplusplus
}
#endif

#undef ALIGN_STRUCT
#ifdef _MSC_VER
#	pragma pack( pop, packing )
#endif

#endif //X_FONT_FILE_HEADER