
#ifndef X_FILE_HEADER
#define X_FILE_HEADER

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
#endif

enum File_Magic_Numbers
{
	Magic_FontType	=	0x00010001,
	Magic_MeshType  =   0x00010002,
};

struct file_header
{
	float	version;
	union {
		unsigned long	code;
		char			magic[4];
	};
}ALIGN_STRUCT;

typedef struct file_header file_header;

#ifdef __cplusplus
}
#endif


#undef ALIGN_STRUCT
#ifdef _MSC_VER
#	pragma pack( pop, packing )
#endif

#endif//X_FILE_HEADER
