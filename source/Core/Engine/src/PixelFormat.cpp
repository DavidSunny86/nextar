/* 
 * File:   PixelFormat.cpp
 * Author: obhi
 * 
 * Created on May 5, 2011, 5:02 PM
 */
#include "NexEngine.h"
#include "PixelFormat.h"
#include "PixelConversion.hpp"

#define PixelConversionTypeId(from,to) (((uint32)from << 16) | (uint32)to)
namespace nextar {

	namespace PixelUtils {

		/* All sizes are in bytes */
		struct PixelFormatDesc {
			char* name;
			uint8 pixelSize;
			uint8 numChannels;
			uint16 flags;
		};

// descriptor table
		static PixelFormatDesc formatDescTable[PixelFormat::COUNT] = { {
// name
				"A8",
				// pixelSize, numChannels, flags
				1, 1, PIXFLAG_INTEGRAL | PIXFLAG_HAS_ALPHA
						| PIXFLAG_TEXTURE_FORMAT | PIXFLAG_UNSIGNED
						| PIXFLAG_NORMALIZED },

		{
// name
				"RGBA8",
				// pixelSize, numChannels, flags
				4, 4, PIXFLAG_INTEGRAL | PIXFLAG_HAS_ALPHA
						| PIXFLAG_TEXTURE_FORMAT | PIXFLAG_UNSIGNED
						| PIXFLAG_NORMALIZED },

		{
// name
				"BGRA8",
				// pixelSize, numChannels, flags
				4, 4, PIXFLAG_INTEGRAL | PIXFLAG_HAS_ALPHA
						| PIXFLAG_TEXTURE_FORMAT | PIXFLAG_UNSIGNED
						| PIXFLAG_NORMALIZED },

		{ "D16",
// pixelSize, numChannels, flags
				2, 1, PIXFLAG_FLOAT | PIXFLAG_TEXTURE_FORMAT | PIXFLAG_UNSIGNED
						| PIXFLAG_NORMALIZED | PIXFLAG_DEPTH },

		{ "D24",
// pixelSize, numChannels, flags
				3, 1, PIXFLAG_FLOAT | PIXFLAG_TEXTURE_FORMAT | PIXFLAG_UNSIGNED
						| PIXFLAG_NORMALIZED | PIXFLAG_DEPTH },

				{ "D24S8",
// pixelSize, numChannels, flags
						4, 2, PIXFLAG_FLOAT | PIXFLAG_TEXTURE_FORMAT
								| PIXFLAG_UNSIGNED | PIXFLAG_NORMALIZED
								| PIXFLAG_DEPTH | PIXFLAG_STENCIL },

				{ "D32",
// pixelSize, numChannels, flags
						4, 1, PIXFLAG_FLOAT | PIXFLAG_TEXTURE_FORMAT
								| PIXFLAG_UNSIGNED | PIXFLAG_NORMALIZED
								| PIXFLAG_DEPTH },

				{ "RGBA16F",
// pixelSize, numChannels, flags
						8, 1, PIXFLAG_FLOAT | PIXFLAG_TEXTURE_FORMAT
								| PIXFLAG_UNSIGNED | PIXFLAG_NORMALIZED
								| PIXFLAG_DEPTH } };

		PixelFormat GetFormatFromString(const String& name) {
			for (uint32 i = 0; i < PixelFormat::COUNT; ++i)
				if (!name.compare(formatDescTable[i].name))
					return (PixelFormat) i;
			return PixelFormat::UNKNOWN;
		}

		bool IsDepthFormat(PixelFormat pf) {
			const int index = (const int) pf;
			if (index >= 0 && index <= PixelFormat::COUNT) {
				uint32 flags = formatDescTable[index].flags;
				return (flags & PIXFLAG_DEPTH) != 0;
			}
			return false;
		}

		bool IsStencilFormat(PixelFormat pf) {
			const int index = (const int) pf;
			if (index >= 0 && index <= PixelFormat::COUNT) {
				uint32 flags = formatDescTable[index].flags;
				return (flags & PIXFLAG_STENCIL) != 0;
			}
			return false;
		}

		bool IsDepthStencilFormat(PixelFormat pf) {
			const int index = (const int) pf;
			if (index >= 0 && index <= PixelFormat::COUNT) {
				uint32 flags = formatDescTable[index].flags;
				return (flags & PIXFLAG_DEPTH) && (flags & PIXFLAG_STENCIL);
			}
			return false;
		}

		String GetStringFromFormat(PixelFormat pf) {
			const int index = (const int) pf;
			if (index >= 0 && index <= PixelFormat::COUNT)
				return formatDescTable[index].name;
			return StringUtils::Null;
		}

		inline bool IsCompressed(PixelFormat pf) {
			const int index = (const int) pf;
			NEX_ASSERT(index >= 0 && index <= PixelFormat::COUNT);
			return (formatDescTable[index].flags & PIXFLAG_COMPRESSED) != 0;
		}

		bool IsValidTextureFormat(PixelFormat pf) {
			return (formatDescTable[pf].flags & PIXFLAG_TEXTURE_FORMAT) != 0;
		}

		PixelFormat GetNearestTextureFormat(PixelFormat fmt) {
			switch (fmt) {
			case PixelFormat::BGRA8:
			case PixelFormat::RGBA8:
			case PixelFormat::A8:
			case PixelFormat::D16:
			case PixelFormat::D24S8:
			case PixelFormat::D32:
			case PixelFormat::RGBA16F:
				return fmt;
			}
			return PixelFormat::UNKNOWN;
		}

// include conversion routines

		uint32 BytesPerPixel(PixelFormat pf) {
			const int index = (const int) pf;
			if(index >= 0 && index <= PixelFormat::COUNT)
				return formatDescTable[index].pixelSize;
			return 0;
		}

// get buffer size

		size_t GetBufferSize(uint32 width, uint32 height, uint32 depth,
				PixelFormat fmt) {
			if (IsCompressed(fmt)) {
				/* todo for compressed formats */
			}
			return BytesPerPixel(fmt) * width * height * depth;
		}

		static void _PixelTransferImpl(PixelBox& dest, const PixelBox& src) {
#define PIXCONV_ROUTINE(from,to)  case PixelConversionTypeId(from,to):  \
        PixelConverter< PixelMapper< from, to > >::Transfer(dest, src); \
        return;

			// the dimensions are same
			NEX_ASSERT(
					src.GetWidth() == dest.GetWidth() && src.GetHeight() == dest.GetHeight() && src.GetDepth() == dest.GetDepth() && src.format != dest.format);

			// check if we have a converter
			switch (PixelConversionTypeId(src.format, dest.format)) {
			PIXCONV_ROUTINE(PixelFormat::RGBA8, PixelFormat::BGRA8)
			PIXCONV_ROUTINE(PixelFormat::BGRA8, PixelFormat::RGBA8)
			}
#undef PIXCONV_ROUTINE
			// do a floating point convert
			LogManager::LogMsg(LOG_ERROR, "PixelTransfer",
					"Cannot do pixel format convertion.");
		}

		static void _PackPixel(ColorChannel& c, const void *data,
				PixelFormat fmt) {
#define PIXPACK_ROUTINE(format) case format: PixelPacker<format>::Pack(c, data); break;
			switch (fmt) {
			PIXPACK_ROUTINE(PixelFormat::A8)
			PIXPACK_ROUTINE(PixelFormat::RGBA8)
			PIXPACK_ROUTINE(PixelFormat::BGRA8)
			}
#undef PIXPACK_ROUTINE
		}

		static void _UnpackPixel(ColorChannel& c, void *data, PixelFormat fmt) {
#define PIXUNPACK_ROUTINE(format) case format: PixelPacker<format>::Unpack(data, c); break;
			switch (fmt) {
			PIXUNPACK_ROUTINE(PixelFormat::A8)
			PIXUNPACK_ROUTINE(PixelFormat::RGBA8)
			PIXUNPACK_ROUTINE(PixelFormat::BGRA8)
			}
#undef PIXUNPACK_ROUTINE
		}

		/* Pixel transfer */
		void PixelTransfer(PixelBox& dest, const PixelBox& src, Filter filter) {
			/* do some validation*/
			/* end of validation */
			bool needsResize = !IsPixelVolumeSame(dest, src);
			bool freeProxySrc = false;
			PixelBox proxySrc = src;
			/* check if src and dest have different volumes, in which case
			 * pixel transfer has to take place on seperate buffers */
			if (src.format != dest.format) {
				if (needsResize) {
					freeProxySrc = true;
					proxySrc.data =
							NEX_ALLOC(
									GetBufferSize(proxySrc.GetWidth(),
											proxySrc.GetHeight(),
											proxySrc.GetDepth(), dest.format), MEMCAT_GENERAL); // retrieve size
					/* todo this may be incorrectly set, the rowSkip and sliceSkip,
					 check this for compressed formats */
					proxySrc.format = dest.format;
					proxySrc.rowPixelPitch = proxySrc.GetWidth();
					proxySrc.slicePixelPitch = proxySrc.GetWidth()
							* proxySrc.GetHeight();
					_PixelTransferImpl(proxySrc, src);
				} else
					_PixelTransferImpl(dest, src);
			} else if (!needsResize) {
				// just do a blunt copy
				uint32 bytesPerPix = BytesPerPixel(src.format);
				if (src.IsSimpleArray() && dest.IsSimpleArray()) {
					void* srcData = reinterpret_cast<uint8*>(src.data)
							+ src.front * src.slicePixelPitch * bytesPerPix;
					void* destData = reinterpret_cast<uint8*>(dest.data)
							+ dest.front * dest.slicePixelPitch * bytesPerPix;
					std::memcpy(
							destData,
							srcData,
							GetBufferSize(src.GetWidth(), src.GetHeight(),
									src.GetDepth(), src.format));
				} else {
					uint32 width = src.GetWidth();
					for (uint32 d = src.front; d < src.back; ++d) {
						void* srcPlane = reinterpret_cast<uint8*>(src.data)
								+ d * src.slicePixelPitch * bytesPerPix;
						void* destPlane = reinterpret_cast<uint8*>(dest.data)
								+ d * dest.slicePixelPitch * bytesPerPix;
						for (uint32 h = src.top; h < src.bottom; ++h) {
							void* srcRow = reinterpret_cast<uint8*>(srcPlane)
									+ h * src.rowPixelPitch * bytesPerPix;
							void* destRow = reinterpret_cast<uint8*>(destPlane)
									+ h * dest.rowPixelPitch * bytesPerPix;
							std::memcpy(destRow, srcRow,
									width * bytesPerPix);
						}
					}
				}
			}

			if (needsResize) {
				ResizeBox(proxySrc, dest, filter);
			}
			if (freeProxySrc) {
				NEX_FREE(proxySrc.data, MEMCAT_GENERAL);
			}
		}

// Linear interpolation

		void LinearSampler(PixelBox& dest, const PixelBox& src) {

			NEX_ASSERT(src.format == dest.format);
			size_t bytesPerPix = BytesPerPixel(src.format);
			size_t numChannels = formatDescTable[src.format].numChannels;
			NEX_ASSERT(bytesPerPix == BytesPerPixel(dest.format));
			float dstep = ((float) src.GetDepth() / (float) dest.GetDepth()) * .5f;
			float hstep = ((float) src.GetHeight() / (float) dest.GetHeight()) * .5f;
			float wstep = ((float) src.GetWidth() / (float) dest.GetWidth()) * .5f;

			for (uint32 d = dest.front; d < dest.back; ++d) {
				float fdoff1 = (float) (dstep * (d * 2 + 1));
				size_t doff1 = (size_t) Math::Floor(fdoff1);
				size_t doff2 = std::min(doff1 + 1, (size_t) (src.back - 1));
				float dweight = fdoff1 - (float) doff1;
				float dweightOpp = 1 - dweight;

				NEX_ASSERT(doff1 >= src.front && doff1 < src.back);
				void* destPlane = reinterpret_cast<uint8*>(dest.data)
						+ d * dest.slicePixelPitch * bytesPerPix;
				for (uint32 h = dest.top; h < dest.bottom; ++h) {

					float fhoff1 = (float) ((2 * h + 1) * hstep);
					size_t hoff1 = (size_t) Math::Floor(fdoff1);
					size_t hoff2 = std::min(hoff1 + 1,
							(size_t) (src.bottom - 1));
					float hweight = fhoff1 - (float) hoff1;
					float hweightOpp = 1 - hweight;

					NEX_ASSERT(hoff1 >= src.top && hoff1 < src.bottom);
					uint8* destRow = reinterpret_cast<uint8*>(destPlane)
							+ h * dest.rowPixelPitch * bytesPerPix;
					for (uint32 w = dest.left; w < dest.right; ++w) {
						float fwoff1 = (2 * w + 1) * wstep;
						size_t woff1 = (size_t) Math::Floor(fwoff1);
						size_t woff2 = std::min(woff1 + 1,
								(size_t) (src.right - 1));
						float wweight = fwoff1 - (float) woff1;
						float wweightOpp = 1 - wweight;
						NEX_ASSERT(woff1 >= src.left && woff1 < src.right);
						// 
						ColorChannel d1h1w1, d1h1w2, d2h1w1, d2h1w2, d2h2w2,
								d2h2w1, d1h2w2, d1h2w1, accum;
						// extract

#define PIXVAL_EXTRACT(to, x,y,z)  \
_PackPixel( to, reinterpret_cast<uint8*>(src.data) + \
(x*src.slicePixelPitch + y*src.rowPixelPitch + z)*bytesPerPix, src.format);

						PIXVAL_EXTRACT(d1h1w1, doff1, hoff1, woff1);
						PIXVAL_EXTRACT(d1h1w2, doff1, hoff1, woff2);
						PIXVAL_EXTRACT(d2h1w1, doff2, hoff1, woff1);
						PIXVAL_EXTRACT(d2h1w2, doff2, hoff1, woff2);
						PIXVAL_EXTRACT(d2h2w2, doff2, hoff2, woff2);
						PIXVAL_EXTRACT(d2h2w1, doff2, hoff2, woff1);
						PIXVAL_EXTRACT(d1h2w2, doff1, hoff2, woff2);
						PIXVAL_EXTRACT(d1h2w1, doff1, hoff2, woff1);

						// src offset
						for (uint32 i = 0; i < numChannels; ++i) {
							accum.value[i] =
									d1h1w1.value[i]
											* (dweightOpp * hweightOpp
													* wweightOpp)
											+ d1h1w2.value[i]
													* (dweightOpp * hweightOpp
															* wweight)
											+ d2h1w1.value[i]
													* (dweight * hweightOpp
															* wweightOpp)
											+ d2h1w2.value[i]
													* (dweight * hweightOpp
															* wweight)
											+ d2h2w2.value[i]
													* (dweight * hweight
															* wweight)
											+ d2h2w1.value[i]
													* (dweight * hweight
															* wweightOpp)
											+ d1h2w2.value[i]
													* (dweight * hweightOpp
															* wweightOpp)
											+ d1h2w1.value[i]
													* (dweight * hweightOpp
															* wweight);
						}

						uint8* destData = destRow + w * bytesPerPix;
						_UnpackPixel(accum, destData, dest.format);
					}
				}
			}
		}

// Piecewise constant interpolation

		void NearestSampler(PixelBox& dest, const PixelBox& src) {
			size_t bytesPerPix = BytesPerPixel(src.format);
			NEX_ASSERT(bytesPerPix == BytesPerPixel(dest.format));

			float dstep = ((float) src.GetDepth() / (float) dest.GetDepth()) * .5f;
			float hstep = ((float) src.GetHeight() / (float) dest.GetHeight()) * .5f;
			float wstep = ((float) src.GetWidth() / (float) dest.GetWidth()) * .5f;

			for (uint32 d = dest.front; d < dest.back; ++d) {
				size_t doff = (uint32) (dstep * (src.slicePixelPitch * d * 2 + 1));
				NEX_ASSERT(doff >= 0 && doff <= src.slicePixelPitch * d);
				void* destPlane = reinterpret_cast<uint8*>(dest.data)
						+ d * dest.slicePixelPitch * bytesPerPix;
				for (uint32 h = dest.top; h < dest.bottom; ++h) {

					size_t hoff = (uint32) ((2 * h * src.rowPixelPitch + 1)
							* hstep);
					NEX_ASSERT(hoff >= 0 && hoff <= src.rowPixelPitch * h);
					uint8* destRow = reinterpret_cast<uint8*>(destPlane)
							+ h * dest.rowPixelPitch * bytesPerPix;
					for (uint32 w = dest.left; w < dest.right; ++w) {
						size_t woff = (size_t) ((2 * w + 1) * wstep);
						NEX_ASSERT(woff >= 0 && woff <= w);
						uint8* srcData = reinterpret_cast<uint8*>(src.data)
								+ (doff + hoff + woff) * bytesPerPix;
						// src offset
						uint8* destData = (destRow + w * bytesPerPix);
						for (uint32 j = 0; j < bytesPerPix; ++j)
							destData[j] = srcData[j];
					}
				}
			}
		}

		void ResizeBox(PixelBox& dest, const PixelBox& src, Filter filter) {
			switch (filter) {
			case FILTER_ANISOTROPIC:
			case FILTER_LINEAR:
				LinearSampler(dest, src);
				break;
			case FILTER_NEAREST:
			default:
				NearestSampler(dest, src);
				break;
			}
		}
	}
}
