/* 
 * File:   Image.cpp
 * Author: obhi
 * 
 * Created on May 5, 2011, 4:25 PM
 */
#include "NexHeaders.h"
#include "Image.h"

namespace nextar {

	/*****************************************************/
	/* Image			       							 */
	/*****************************************************/
	Image::Image()
			: width(0), height(0), depth(1), numFaces(0), totalMipMapCount(0),
			  numMipMaps(0), format(PixelFormat::UNKNOWN), dataBuffer(0) {
	}

	Image::~Image() {
		if (dataBuffer)
			NEX_FREE(dataBuffer, MEMCAT_GENERAL);
	}

	PixelBox Image::GetPixelBox(uint32 face, uint32 mipLevel) {
		PixelBox ret;

		NEX_ASSERT(face < GetNumFaces());
		NEX_ASSERT(mipLevel <= GetNumMipMaps());

		size_t offset = 0;
		size_t faceOffset = 0;
		uint32 w, h, d;
		w = width;
		h = height;
		d = depth;
		uint8* data = static_cast<uint8*>(dataBuffer);
		for (size_t mip = 0; mip < numMipMaps; ++mip) {
			if (mip == mipLevel) {
				faceOffset = offset;
				ret.right = w;
				ret.bottom = h;
				ret.back = d;
			}
			offset += PixelUtils::GetBufferSize(w, h, d, format);
			/// Half size in each dimension
			w /= 2;
			h /= 2;
			d /= 2;
		}
		// Advance pointer by number of full faces, plus mip offset into
		data += face * offset;
		data += faceOffset;
		ret.data = data;
		ret.format = GetFormat();
		ret.CalculatePitches();

		return ret;
	}

	void Image::Load(InputStreamPtr& stream, const ImageParams& params) {
		if (dataBuffer)
			NEX_FREE(dataBuffer, MEMCAT_GENERAL);
		Serializer l(params);
		ImageData data = l.Serialize(stream);
		if (data.data) {
			numFaces = data.numFaces;
			numMipMaps = data.numMipMaps;
			depth = data.depth;
			height = data.height;
			width = data.width;
			format = data.format;
			dataBuffer = data.data;
			totalMipMapCount = data.totalMipMapCount;
		} else {
			Error(String("Failed to load image ") + params.name);
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}
	}

	/*****************************************************/
	/* Image::Loader        							 */
	/*****************************************************/
	NEX_IMPLEMENT_FACTORY(Image::Serializer);
	ImageData Image::Serializer::Serialize(InputStreamPtr& stream) {
		String fakeLoaderName = params.codecName;
		if (!fakeLoaderName.length()) {
			const String& name = params.name;
			size_t pos = name.find_first_of('.');
			if (pos != String::npos) {
				fakeLoaderName = (name.substr(pos + 1));
				StringUtils::ToUpper(fakeLoaderName);
			}
		}

		ImageCodec* codec = GetImpl(fakeLoaderName);
		if (!codec) {
			Error(String("Couldn't find image codec for: ") + params.name);
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}

		return codec->Load(stream, params);
	}
}

