#ifndef PIXELCONVERSION_H
#define PIXELCONVERSION_H

#include <PixelFormat.h>

namespace nextar {

namespace PixelUtils {

struct ColorChannel {
	float value[4];
};

template<typename C>
class PixelConverter {
public:

	typedef typename C::SrcType SrcType;
	typedef typename C::DestType DestType;

	static void Transfer(PixelBox& dest, const PixelBox& src) {
		SrcType* srcData = reinterpret_cast<SrcType*>(src.Data())
				+ (src.front * src.slicePixelPitch)
				+ (src.top * src.rowPixelPitch) + src.left;
		DestType* destData = reinterpret_cast<DestType*>(dest.Data())
				+ (dest.front * dest.slicePixelPitch)
				+ (dest.top * dest.rowPixelPitch) + dest.left;

		uint32 srcRowSkip = src.rowPixelPitch - src.GetWidth();
		uint32 destRowSkip = dest.rowPixelPitch - dest.GetWidth();
		uint32 srcPlaneSkip = src.slicePixelPitch
				- (src.GetHeight() * src.GetWidth());
		uint32 destPlaneSkip = dest.slicePixelPitch
				- (dest.GetHeight() * dest.GetWidth());

		for (uint32 d = src.front; d < src.back; ++d) {

			for (uint32 h = src.top; h < src.bottom; ++h) {
				for (uint32 w = src.left; w < src.right; ++w) {
					destData[w] = C::MapPixel(srcData[w]);
				}
				destData += destRowSkip;
				srcData += srcRowSkip;
			}
			destData += destPlaneSkip;
			srcData += srcPlaneSkip;
		}
	}
};

template<typename S, typename D>
class PixMapperBase {
public:
	typedef S SrcType;
	typedef D DestType;
};

template<PixelFormat srcFmt, PixelFormat destFmt>
class PixelMapper {
public:
};

template<>
class PixelMapper<PixelFormat::RGBA8, PixelFormat::BGRA8> : public PixMapperBase<
		uint32, uint32> {
public:
	static inline uint32 MapPixel(uint32 p) {
#if (NEX_ENDIANNESS == NEX_ENDIAN_LITTLE)
		return (p & 0xff000000) | ((p & 0x00ff0000) >> 16) | (p & 0x0000ff00)
				| ((p & 0x000000ff) << 16);
#else
		return (p & 0x000000ff) | ((p & 0xff000000) >> 16) |
		(p & 0x00ff0000) | ((p & 0x0000ff00) << 16);
#endif
	}
};

template<>
class PixelMapper<PixelFormat::BGRA8, PixelFormat::RGBA8> : public PixelMapper<
		PixelFormat::RGBA8, PixelFormat::BGRA8> {
};

/* No format conversion is possible using this */
template<PixelFormat fmt>
class PixelPacker {
public:
	static inline void Pack(ColorChannel& c, const void* pixMem) {
	}
	static inline void Unpack(void* pixMem, const ColorChannel& c) {
	}
};

template<>
class PixelPacker<PixelFormat::R8> {
public:
	static inline void Pack(ColorChannel& c, const void* pixMem) {
		c.value[0] = (float) (*(const uint8*) pixMem) / 255.f;
	}

	static inline void Unpack(void* pixMem, const ColorChannel& c) {
		(*(uint8*) pixMem) = (uint8) (c.value[0] * 255.f);
	}
};

template<>
class PixelPacker<PixelFormat::BGRA8> {
public:
	static inline void Pack(ColorChannel& c, const void* pixMem) {
		const uint8* memPtr = (const uint8*) pixMem;
		c.value[0] = (float) (memPtr[0]) / 255.f;
		c.value[1] = (float) (memPtr[1]) / 255.f;
		c.value[2] = (float) (memPtr[2]) / 255.f;
		c.value[3] = (float) (memPtr[3]) / 255.f;
	}

	static inline void Unpack(void* pixMem, const ColorChannel& c) {
		uint8* memPtr = (uint8*) pixMem;
		memPtr[0] = (uint8) (c.value[0] * 255.f);
		memPtr[1] = (uint8) (c.value[1] * 255.f);
		memPtr[2] = (uint8) (c.value[2] * 255.f);
		memPtr[3] = (uint8) (c.value[3] * 255.f);
	}
};

template<>
class PixelPacker<PixelFormat::RGBA8> : public PixelPacker<PixelFormat::BGRA8> {
};
}
}

#endif //PIXELCONVERSION_H