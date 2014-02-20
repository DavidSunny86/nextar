#ifndef NEXTAR_MOUNTPOINT_H
#define NEXTAR_MOUNTPOINT_H

#include "NexSys.h"
#include "DataStream.h"
#include "URL.h"

#define NEX_SET_FILE_TIME(ft,tm) NEX_SET_FILE_TIME_((ft),(tm))
#if defined(NEX_MSVC)
#define NEX_SET_FILE_TIME_(ft,var)				\
	{ struct tm  ts;							\
	localtime_s(&ts, &var);						\
	ft.sec = (int8)ts.tm_sec;					\
	ft.min = (int8)ts.tm_min;					\
	ft.hrs = (int8)ts.tm_hour;					\
	ft.day = (int8)ts.tm_mday;					\
	ft.mon = (int8)ts.tm_mon;					\
	ft.year = (int16)ts.tm_year + 1900; }
#else
#define NEX_SET_FILE_TIME_(ft,var)				\
	{ const struct tm  *ts;						\
	ts = localtime(&var);						\
	ft.sec = (int8)ts->tm_sec;					\
	ft.min = (int8)ts->tm_min;					\
	ft.hrs = (int8)ts->tm_hour;					\
	ft.day = (int8)ts->tm_mday;					\
	ft.mon = (int8)ts->tm_mon;					\
	ft.year = (int16)ts->tm_year + 1900; }
#endif

namespace nextar {

	struct FileAttribute;
	struct FileTime;

	/**
	 * @struct	FileTime
	 *
	 * @brief	Time value for files.
	 *
	 * @author	Abhishek Dey
	 * @date	12/8/2009
	 **/
	struct FileTime {
		//! 0...59
		int8 sec;
		//! 0...59
		int8 min;
		//! 0...23
		int8 hrs;
		//! 1...31
		int8 day;
		//! 1...12
		int8 mon;
		//! padding
		int8 reserved;
		//! 4 digit year
		int16 year;
	};

	/**
	 * @struct	FileAttribute : public AllocGeneral
	 *
	 * @brief	FileAttribute for file.
	 *
	 * @author	Abhishek Dey
	 * @date	10/28/2009
	 **/
	struct FileAttribute: public AllocGeneral {

		enum Flags {
			ATTRIB_DIR = 1 << 1, /**< a directory or a file */
			ATTRIB_ARC = 1 << 2, /**< an achieve  */
			ATTRIB_READONLY = 1 << 3, /**< read only */
			ATTRIB_COMPRESSED = 1 << 6,
			ATTRIB_DEFAULTS = 0
		};
		//! full file name acceptable by this mount point
		URL fileName;
		//! flags
		uint32 flags;
		//! uncompressed size of file.
		size_t uncompressedSize;
		//! compressed size of file.
		size_t compressedSize;
		//! time last written to file
		FileTime fileTime;
	};

	/**
	 * @class	Archive
	 *
	 * @brief	Abstract Mount point.
	 *
	 * \@detail  A mount point can be any file container from
	 *          an archive or directory or a list of them, to
	 *          a remote file location. It is implementation
	 *          specific and instantiated by the file system.
	 *          @par Types of Mount Points
	 *          Mount points can be of various types.
	 *          - dir
	 *          - archive
	 *          - remote
	 *          @par
	 *          Other types can be registered as per requirements
	 *
	 * @author	Abhishek Dey
	 * @date	11/28/2009
	 **/
	class _NexExport Archive: public Referenced<Archive, AllocGeneral> {

	public:

		enum {
			TYPE_INVALID = -1,
			// common types
			TYPE_DIR = ('d' << 24) | ('i' << 16) | ('r' << 8) | 0,
			TYPE_ARC = ('a' << 24) | ('r' << 16) | ('c' << 8) | 0,
		};

		/** @brief Callback during scanning for files **/
		struct ScanCallback {
			/** @brief Callback, called when scanning dir. **/
			virtual void FoundFile(const FileAttribute& attribute,
					Archive*) = 0;
		protected:
			~ScanCallback() {}
		};

		class Factory {
		public:
			virtual ~Factory() {
			}
			/** @remarks Create an instance given a base directory and multiple sub directories	 */
			virtual ArchivePtr CreateInstance(const String& name,
					const String& params) = 0;
		};

	public:

		inline Archive(const String& archName = StringUtils::Null) :
				name(archName) {
		}

		inline virtual ~Archive() {
		}

		/**
		 * @brief	Gets the name.
		 *
		 * @return	The name.
		 **/
		_NexInline const String& GetName() const {
			return name;
		}

		/**
		 * @brief	Opens the file stored in current mount point.
		 *
		 * @param [in,out]	filename	If non-null, filename of the file.
		 *
		 * @return	null if it fails, else the input stream pointer.
		 **/
		virtual InputStreamPtr OpenRead(const String& relativePath) = 0;
		/**
		 * @brief	Opens a writeable stream.
		 * \@detail  @remarks
		 *          For multi valued mount points the file can be created at any directory.
		 *
		 * @param [in,out]	filename	If non-null, filename of the file.
		 * @param	append				true to append.
		 *
		 * @return	.
		 **/
		virtual OutputStreamPtr OpenWrite(const String& relativePath,
				bool append = false) = 0;

		/**
		 *
		 * @brief	Scans.
		 *
		 * @param	callback		The ScanCallback callback function.
		 * @param 	pattern			If non-null, the pattern to use for scan, including wildcards.
		 * @param	recursive		true to process recursively, false to process locally only, only
		 * 							applicable to folders and directories.
		 * @param 	data			If non-null, the user data passed to scan callback.
		 **/
		virtual void Scan(ScanCallback* callback, const String& pattern =
				StringUtils::Null, bool recursive = true) = 0;

		/**
		 * @brief	Gets a file list.
		 *
		 * @param [in,out]		the.
		 **/
		virtual void GetFileList(StringVector&, bool listOnlyFiles = false) = 0;

		/**
		 * @brief	Gets an attribute for an existing file.
		 *
		 * @author	Abhishek Dey
		 * @date	11/28/2009
		 *
		 * @param [in,out]	filename	If non-null, filename of the file.
		 * @param [in,out]	attributes	the attributes.
		 *
		 * @return	true if it succeeds, false if it fails.
		 **/
		virtual bool GetAttribute(const String& filename,
				FileAttribute& attributes) = 0;

		/**
		 * Query if file exists.
		 */
		virtual bool Exists(const String& filename) = 0;

	protected:
		String name;
	};

}

#endif //NEXTAR_MOUNTPOINT_H
