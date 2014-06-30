#ifndef NEXTAR_DOTZIP_H
#define NEXTAR_DOTZIP_H

#include <NexBase.h>
#include <Archive.h>

#if defined(__cplusplus)
extern "C" {
#endif
#include <zlib.h>

#if defined(__cplusplus)
}
#endif

#define CENTRAL_HDR_SIG	'\001','\002'	/* the infamous "PK" signature bytes, */
#define LOCAL_HDR_SIG	'\003','\004'	/*  sans "PK" (so unzip executable not */
#define END_CENTRAL_SIG	'\005','\006'	/*  mistaken for zipfile itself) */
#define EXTD_LOCAL_SIG	'\007','\010'	/* [ASCII "\113" == EBCDIC "\080" ??] */

#define DEF_WBITS	15		/* Default LZ77 window size */
#define ZIP_STORE	0		/* 'STORED' method id */
#define ZIP_DEFLATE	8		/* 'DEFLATE' method id */

namespace nextar {
namespace zip {
typedef uint8 uch;
typedef uint16 ush;

#define CRCVAL_INITIAL  0L

typedef struct {
	uch versionNeededToExtract[2];
	ush generalPurposeBitFlag;
	ush compressionMethod;
	ush lastModFileTime;
	ush lastModFileDate;
	uint32 crc32;
	uint32 csize;
	uint32 ucsize;
	ush filenameLength;
	ush extraFieldLength;
} zLocalFileHdr;

typedef struct {
	uch versionMadeBy[2];
	uch versionNeededToExtract[2];
	ush generalPurposeBitFlag;
	ush compressionMethod;
	ush lastModFileTime;
	ush lastModFileDate;
	uint32 crc32;
	uint32 csize;
	uint32 ucsize;
	ush filenameLength;
	ush extraFieldLength;
	ush fileCommentLength;
	ush diskNumberStart;
	ush internalFileAttributes;
	uint32 externalFileAttributes;
	uint32 relativeOffsetLocalHeader;
} zCentralDirFileHdr;

typedef struct {
	ush numberThisDisk;
	ush numDiskStartCdir;
	ush numEntriesCentrlDirThsDisk;
	ush totalEntriesCentralDir;
	uint32 sizeCentralDirectory;
	uint32 offsetStartCentralDirectory;
	ush zipfileCommentLength;
} zEndCentralDirRecord;

//--- ZIP_local_file_header layout ---------------------------------------------
#define ZIP_LOCAL_FILE_HEADER_SIZE              26
#define L_VERSION_NEEDED_TO_EXTRACT_0     0
#define L_VERSION_NEEDED_TO_EXTRACT_1     1
#define L_GENERAL_PURPOSE_BIT_FLAG        2
#define L_COMPRESSION_METHOD              4
#define L_LAST_MOD_FILE_TIME              6
#define L_LAST_MOD_FILE_DATE              8
#define L_CRC32                           10
#define L_COMPRESSED_SIZE                 14
#define L_UNCOMPRESSED_SIZE               18
#define L_FILENAME_LENGTH                 22
#define L_EXTRA_FIELD_LENGTH              24

//--- ZIP_central_directory_file_header layout ---------------------------------
#define ZIP_CENTRAL_DIRECTORY_FILE_HEADER_SIZE  42
#define C_VERSION_MADE_BY_0               0
#define C_VERSION_MADE_BY_1               1
#define C_VERSION_NEEDED_TO_EXTRACT_0     2
#define C_VERSION_NEEDED_TO_EXTRACT_1     3
#define C_GENERAL_PURPOSE_BIT_FLAG        4
#define C_COMPRESSION_METHOD              6
#define C_LAST_MOD_FILE_TIME              8
#define C_LAST_MOD_FILE_DATE              10
#define C_CRC32                           12
#define C_COMPRESSED_SIZE                 16
#define C_UNCOMPRESSED_SIZE               20
#define C_FILENAME_LENGTH                 24
#define C_EXTRA_FIELD_LENGTH              26
#define C_FILE_COMMENT_LENGTH             28
#define C_DISK_NUMBER_START               30
#define C_INTERNAL_FILE_ATTRIBUTES        32
#define C_EXTERNAL_FILE_ATTRIBUTES        34
#define C_RELATIVE_OFFSET_LOCAL_HEADER    38

//--- ZIP_end_central_dir_record layout ----------------------------------------
#define ZIP_END_CENTRAL_DIR_RECORD_SIZE         18
#define E_NUMBER_THIS_DISK                0
#define E_NUM_DISK_WITH_START_CENTRAL_DIR 2
#define E_NUM_ENTRIES_CENTRL_DIR_THS_DISK 4
#define E_TOTAL_ENTRIES_CENTRAL_DIR       6
#define E_SIZE_CENTRAL_DIRECTORY          8
#define E_OFFSET_START_CENTRAL_DIRECTORY  12
#define E_ZIPFILE_COMMENT_LENGTH          16

}
}

#endif //NEXTAR_DOTZIP_H