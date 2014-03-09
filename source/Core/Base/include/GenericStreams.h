#ifndef NEXTAR_GENERALSTREAMS_H
#define NEXTAR_GENERALSTREAMS_H

#include <DataStream.h>

namespace nextar {

	class MemoryInputStream;
	class MemoryOutputStream;
	class GenericInputStream;
	class GenericOutputStream;
	class FileInputStream;
	class FileOutputStream;

	typedef RefPtr<MemoryInputStream> MemoryInputStreamPtr;
	typedef RefPtr<MemoryOutputStream> MemoryOutputStreamPtr;
	typedef RefPtr<GenericInputStream> GenericInputStreamPtr;
	typedef RefPtr<GenericOutputStream> GenericOutputStreamPtr;
	typedef RefPtr<FileInputStream> FileInputStreamPtr;
	typedef RefPtr<FileOutputStream> FileOutputStreamPtr;

	/**
	 * @class	MemoryInputStream
	 *
	 * @brief	Memory input stream.
	 *
	 * @author	Abhishek Dey
	 * @date	11/29/2009
	 **/
	class _NexBaseExport MemoryInputStream: public InputStream {
		uint8* data;
		size_t readLeft;bool freedata;
	public:

		//! Construct blank stream
		MemoryInputStream();
		//! Construct stream from memory
		MemoryInputStream(void* data, size_t size, bool freedataptr = false);
		//! Construct from another stream
		MemoryInputStream(InputStream* cpy);
		//! Destroy
		~MemoryInputStream();

		/**
		 * @brief	Clears this object to its blank/initial state.
		 *
		 * @author	Abhishek Dey
		 * @date	11/29/2009
		 **/
		void Clear();
		/**
		 *
		 * @brief	Loads data from memory, only applicable for blank streams.
		 *
		 * @author	Abhishek Dey
		 * @date	11/29/2009
		 *
		 * @param [in,out]	data	If non-null, the data.
		 * @param	size			The size.
		 **/
		void LoadMemory(void* data, size_t size, bool freedataptr = false);

		//! @copydoc InputStream::Read(void*,size_t)
		virtual size_t Read(void* buffer, size_t size);
		//! @copydoc InputStream::Seek(int32,std::ios_base::seekdir)
		virtual bool Seek(std::streamoff offset, std::ios_base::seekdir dir =
				std::ios_base::cur);
		//! @copydoc InputStream::Tell()
		virtual std::streamoff Tell();
		//! @copydoc InputStream::IsEndOfStream()
		virtual bool IsEndOfStream() const;
		//! @copydoc InputStream::AcquireBuffer()
		virtual size_t AcquireBuffer(const void*& readOnlyData);
		//! @copydoc InputStream::ReleaseBuffer()
		virtual void ReleaseBuffer(const void* readOnlyData);
	};

	/**
	 * @class	MemoryOutputStream
	 *
	 * @brief	Memory output stream.
	 *
	 * @author	Abhishek Dey
	 * @date	11/29/2009
	 **/
	class _NexBaseExport MemoryOutputStream: public OutputStream {

		ByteStream buffer;
		size_t pos;
	public:
		MemoryOutputStream();
		~MemoryOutputStream();
		/**
		 * @brief	Gets the raw data stored.
		 *
		 * @author	Abhishek Dey
		 * @date	11/29/2009
		 *
		 * @return	null if it fails, else the data.
		 **/
		const uint8* GetData() const;

		//! @copydoc OutputStream::Write(void*,size_t)
		virtual void Write(const void* buffer, size_t size);
		//! @copydoc OutputStream::Seek(int32,std::ios_base::seekdir)
		virtual bool Seek(std::streamoff offset, std::ios_base::seekdir dir =
				std::ios_base::cur);
		//! @copydoc OutputStream::Tell()
		virtual std::streamoff Tell();
	};

	class _NexBaseExport GenericInputStream: public InputStream {
		std::istream* streamptr;bool freedata;
	public:

		GenericInputStream(std::istream* strm, size_t size = CONST_INVALID_SIZE,
				bool bfreeStream = false);
		~GenericInputStream();

		//! @copydoc InputStream::Read(void*,size_t)
		virtual size_t Read(void* buffer, size_t size);
		//! @copydoc InputStream::Seek(int32,std::ios_base::seekdir)
		virtual bool Seek(std::streamoff offset, std::ios_base::seekdir dir =
				std::ios_base::cur);
		//! @copydoc InputStream::Tell()
		virtual std::streamoff Tell();
		//! @copydoc InputStream::IsEndOfStream()
		virtual bool IsEndOfStream() const;
	};

	class _NexBaseExport GenericOutputStream: public OutputStream {
		std::ostream* streamptr;bool freedata;
	public:
		GenericOutputStream(std::ostream* strm,
				size_t size = CONST_INVALID_SIZE, bool bfreeStream = false);
		~GenericOutputStream();

		//! @copydoc OutputStream::Write(void*,size_t)
		virtual void Write(const void* buffer, size_t size);
		//! @copydoc OutputStream::Seek(int32,std::ios_base::seekdir)
		virtual bool Seek(std::streamoff offset, std::ios_base::seekdir dir =
				std::ios_base::cur);
		//! @copydoc OutputStream::Tell()
		virtual std::streamoff Tell();
	};

	class _NexBaseExport FileInputStream: public InputStream {
		std::FILE* streamptr;
	public:

		FileInputStream(std::FILE* strm, size_t size = CONST_INVALID_SIZE);
		~FileInputStream();

		//! @copydoc InputStream::Read(void*,size_t)
		virtual size_t Read(void* buffer, size_t size);
		//! @copydoc InputStream::Seek(int32,std::ios_base::seekdir)
		virtual bool Seek(std::streamoff offset, std::ios_base::seekdir dir =
				std::ios_base::cur);
		//! @copydoc InputStream::Tell()
		virtual std::streamoff Tell();
		//! @copydoc InputStream::IsEndOfStream()
		virtual bool IsEndOfStream() const;
	};

	class _NexBaseExport FileOutputStream: public OutputStream {
		std::FILE* streamptr;
	public:
		FileOutputStream(std::FILE* streamptr,
				size_t size = CONST_INVALID_SIZE);
		~FileOutputStream();

		//! @copydoc OutputStream::Write(void*,size_t)
		virtual void Write(const void* buffer, size_t size);
		//! @copydoc OutputStream::Seek(int32,std::ios_base::seekdir)
		virtual bool Seek(std::streamoff offset, std::ios_base::seekdir dir =
				std::ios_base::cur);
		//! @copydoc OutputStream::Tell()
		virtual std::streamoff Tell();
	};

}

#endif //NEXTAR_GENERALSTREAMS_H
