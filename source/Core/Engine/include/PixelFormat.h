/* 
 * File:   PixelFormat.h
 * Author: obhi
 *
 * Created on May 5, 2011, 5:02 PM
 */

#ifndef PIXELFORMAT_H
#define	PIXELFORMAT_H

#include <NexBase.h>

namespace nextar {

/**
 * Filtering options
 */
enum Filter {
	FILTER_NEAREST, FILTER_LINEAR, FILTER_ANISOTROPIC,
};

enum PixelFlags {
	/* Indicates compressed texture */
	PIXFLAG_COMPRESSED = 1 << 0,
	/* Indicates alpha channel present */
	PIXFLAG_HAS_ALPHA = 1 << 1,
	/* Indicates floating point format */
	PIXFLAG_FLOAT = 1 << 2,
	/* Signed type */
	PIXFLAG_SIGNED = 1 << 3,
	/* Normalized access to shader */
	PIXFLAG_NORMALIZED = 1 << 4,
	/* Unsigned type */
	PIXFLAG_UNSIGNED = 1 << 5,
	/* SRGB */
	PIXFLAG_SRGB = 1 << 6,
	/* Indicates mask and shifts can be used */
	PIXFLAG_INTEGRAL = 1 << 7,
	/* Indicates can be used as texture format */
	PIXFLAG_TEXTURE_FORMAT = 1 << 8,
	/* Indicates depth format */
	PIXFLAG_DEPTH = 1 << 9,
	/* Indicates stencil format */
	PIXFLAG_STENCIL = 1 << 10,

};

/**
 * @brief
 * Basic pixel formats
 */
enum class PixelFormat
	: int16 {
		/* Red only (encoding alpha), unsigned, normalized 8 bits per pixel */
		R8 = 0,
		/* RRGGBBAA, unsigned, normalized 32 bits per pixel. */
		RGBA8,
		/* BBGGRRAA, unsigned, normalized 32 bits per pixel. */
		BGRA8,
		/* R16G16, unsigned, normalized 32 bits per pixel. */
		RG16,
		/* R16G16, signed, normalized 32 bits per pixel. */
		RG16_SNORM,
		/* depth format, pixel size 16 bits per pixel */
		D16,
		/* depth format, pixel size 24 bits per pixel */
		D24,
		/* depth stencil format, pixel size 32 bits per pixel */
		D24S8,
		/* depth format, pixel size 32 bits per pixel */
		D32,
		/* floating point textures */
		RGBA16F,
		/* Not supported */
		UNKNOWN,

		COUNT = UNKNOWN
};

class _NexEngineAPI PixelBox {

	public:
				
		uint32 left, right, top, bottom, front, back;
		/** Amount of pixels to skip for the next row,
		 * this is not in bytes, rather in pixels.*/
		uint32 rowPixelPitch;
		/** Amount of pixels to skip for the next plane,
		 * this is not in bytes, rather in pixels  */
		uint32 slicePixelPitch;
		PixelFormat format;
		// data
		uint8* data;

		bool deleteData;

		PixelBox()
				: data(nullptr), left(0), right(0), top(0), bottom(0), front(0), back(
						0), rowPixelPitch(0), slicePixelPitch(0), format(
						PixelFormat::UNKNOWN), deleteData(false) {
		}

		PixelBox(uint32 width, uint32 height, uint32 depth, PixelFormat f, void* dat = 0, bool _deleteData = false)
				: data(reinterpret_cast<uint8*>(dat)), left(0), right(width), top(0), bottom(height), front(
						0), back(depth), format(f), deleteData(_deleteData) {
			CalculatePitches();
		}

		~PixelBox() {
			if (deleteData && data)
				NEX_FREE(data, MEMCAT_GENERAL);
		}

		void* Data() const {
			return data;
		}

		uint8*& Data() {
			return data;
		}

		uint32 GetWidth() const {
			return right - left;
		}
		uint32 GetHeight() const {
			return bottom - top;
		}
		uint32 GetDepth() const {
			return back - front;
		}

		uint32 GetNumPixel() const {
			return GetWidth() * GetHeight() * GetDepth();
		}

		bool IsSimpleArray() const {
			return ((GetWidth() == rowPixelPitch)
					&& (GetHeight() * GetWidth() == slicePixelPitch));
		}

		void CalculatePitches() {
			rowPixelPitch = GetWidth();
			slicePixelPitch = GetHeight() * GetWidth();
		}

		Size GetSize() const {
			return Size(GetWidth(), GetHeight());
		}

		size_t GetDataSize() const;
};

namespace PixelUtils {

/* Get pixel format from string */
_NexEngineAPI PixelFormat GetFormatFromString(const String& name);

/* Get pixel format string */
_NexEngineAPI String GetStringFromFormat(PixelFormat);

/* Get buffer size */
_NexEngineAPI size_t GetBufferSize(uint32 width, uint32 height, uint32 depth,
		PixelFormat fmt);

/* Get bytes per pixel */
_NexEngineAPI uint32 BytesPerPixel(PixelFormat);

/**
 * Returns true if the pixel volume is same.
 */
_NexInline bool IsPixelVolumeSame(const PixelBox& box1, const PixelBox& box2) {
return (box1.GetWidth() == box2.GetWidth()
	&& box1.GetHeight() == box2.GetHeight()
	&& box1.GetDepth() == box2.GetDepth()) != 0;
}

/**
 * Returns true if this is a valid depth format.
 */
_NexEngineAPI bool IsDepthFormat(PixelFormat);

/** Returns true if this is a valid stencil format. */
_NexEngineAPI bool IsStencilFormat(PixelFormat);

/** Returns true if this is a valid depth and stencil format. */
_NexEngineAPI bool IsDepthStencilFormat(PixelFormat);

/**
 * All pixel formats are not acceptable texture formats, those which are,
 * can be found from this call. This call will query the render system
 * for validation, and thus the render system must be initialized beforehand.
 * @param 
 * The pixel format to query.
 * @return 
 * true if this pixel format is supported.
 */
_NexEngineAPI bool IsValidTextureFormat(PixelFormat);

/**
 * Return a pixel format that closely resembles the source format
 * @param Source format
 * @return The texture pixel format
 */
_NexEngineAPI PixelFormat GetNearestTextureFormat(PixelFormat);

/**
 * Perform a pixel transfer operation, on the software. In cases where the
 * pixel transfer requires a resize, operation might require filtering.
 * @param src
 * The source pixel box.
 * @param dest
 * The destination pixel box.
 * @param
 * The filtering to perform.
 */
_NexEngineAPI void PixelTransfer(PixelBox& dest, const PixelBox& src, Filter);

/**
 * This function assumes that the src and dest are in the same format,
 * throws error if not. This function will perform a specific pixel 
 * transfer of resizing the src to destination format, if both have
 * same size, it returns without any conversion.
 * @param src
 * @param dest
 * @param 
 */
_NexEngineAPI void ResizeBox(PixelBox& dest, const PixelBox& src, Filter);

}

inline size_t PixelBox::GetDataSize() const {
return PixelUtils::GetBufferSize(GetWidth(), GetHeight(), GetDepth(), format);
}
}

#endif	/* PIXELFORMAT_H */

