/*
 * DDSImageLoader.cpp
 *
 *  Created on: 03-Nov-2013
 *      Author: obhi
 */
#include <EngineHeaders.h>
#include <DDSImageCodec.h>
#include <DXGIFormat.h>
#include <Serializer.h>
#include <RenderManager.h>

namespace ImageLoader {

DDSImageCodec DDSImageCodec::codec;
/*****************************/
DDSImageCodec::DDSImageCodec() {
}

DDSImageCodec::~DDSImageCodec() {
}

bool DDSImageCodec::TryLoad(InputStreamPtr& file, const ImageParams& params, ImageCodecMetaInfo& metaInfo) {
	uint32 mn;
	file->Read(&mn, 4);
#if NEX_NEED_ENDIAN_SWAPPING
	mn = Endian::Swap32(mn);
#endif
	return (mn == DDS_MAGIC);
}

ImageData DDSImageCodec::Load(InputStreamPtr& file, const ImageParams& params, ImageCodecMetaInfo& metaInfo) {
	// read uint32 + sizeof(DDS_HEADER) bytes of data
	uint32 magic;
	InputSerializer ser(file);

	ser >> magic;
	if (magic != DDS_MAGIC) {
		Error("Incorrect file format: " + params.name);
		NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	}

	DDS_HEADER header;
	InputSerializer::UIntArray arr((uint32*) &header, sizeof(header) / 4);
	ser >> arr;
	// Verify header to validate DDS file
	if (header.size != sizeof(DDS_HEADER)
			|| header.ddspf.size != sizeof(DDS_PIXELFORMAT)) {
		Error("Invalid header: " + params.name);
		NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	}

	bool bDXT10Header = false;
	if ((header.ddspf.flags & DDS_FOURCC)
			&& (MAKEFOURCC('D', 'X', '1', '0') == header.ddspf.fourCC)) {
		// Must be long enough for both headers and magic value
		bDXT10Header = true;
	}

	ptrdiff_t offset = sizeof(uint32) + sizeof(DDS_HEADER)
			+ (bDXT10Header ? sizeof(DDS_HEADER_DXT10) : 0);
	return CreateImageData(ser, &header, offset, params, metaInfo);
}

PixelFormat DDSImageCodec::GetPixelFormat(uint32 format) {
	switch (format) {
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return PixelFormat::RGBA8;
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return PixelFormat::BGRA8;
	case DXGI_FORMAT_A8_UNORM:
		return PixelFormat::R8;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return PixelFormat::RGBA16F;
	}

	return PixelFormat::UNKNOWN;
}

PixelFormat DDSImageCodec::GetPixelFormat(DDS_PIXELFORMAT& fmt) {
	return GetPixelFormat((uint32) GetDXGIFormat(fmt));
}

ImageData DDSImageCodec::CreateImageData(InputSerializer& ser,
		DDS_HEADER* header, size_t offset, const ImageParams& params, ImageCodecMetaInfo& metaInfo) {
	if (!metaInfo.metaInfoInited) {
		metaInfo.metaInfo.maxWidth = (uint16) header->width;
		metaInfo.metaInfo.maxHeight = (uint16) header->height;
		metaInfo.metaInfo.maxDepth = (uint16) header->depth;
		metaInfo.metaInfo.maxMipMapCount = (uint16) header->mipMapCount;
		metaInfo.mipLevelsToRead = (uint16) header->mipMapCount;
		metaInfo.metaInfoInited = true;
		if (0 == metaInfo.metaInfo.maxMipMapCount) {
			metaInfo.metaInfo.maxMipMapCount = 1;
		}
		// Bound sizes (for security purposes we don't trust DDS file metadata larger than the D3D 11.x hardware requirements)
		if (metaInfo.metaInfo.maxMipMapCount > (uint16)RenderConstants::MAX_MIP_LEVELS) {
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}
	}

	uint32 resDim = -1;
	size_t arraySize = 1;
	PixelFormat format = PixelFormat::UNKNOWN;
	bool isCubeMap = false;

	if ((header->ddspf.flags & DDS_FOURCC)
			&& (MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC)) {
		DDS_HEADER_DXT10 d3d10ext;
		InputSerializer::UIntArray arr((uint32*) &d3d10ext,
				sizeof(d3d10ext) / 4);
		ser >> arr;
		arraySize = d3d10ext.arraySize;
		if (arraySize == 0) {
			Error("Array size is zero: " + params.name);
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}

		format = GetPixelFormat((uint32) d3d10ext.dxgiFormat);

		if (PixelUtils::BytesPerPixel(format) == 0) {
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}

		switch (d3d10ext.resourceDimension) {
		case DDS_DIMENSION_TEXTURE1D:
			// D3DX writes 1D textures with a fixed Height of 1
			if ((header->flags & DDS_HEIGHT) && metaInfo.metaInfo.maxHeight != 1) {
				NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
			}
			metaInfo.metaInfo.maxHeight = metaInfo.metaInfo.maxDepth = 1;
			break;

		case DDS_DIMENSION_TEXTURE2D:
			if (d3d10ext.miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE) {
				arraySize *= 6;
				isCubeMap = true;
			}
			metaInfo.metaInfo.maxDepth = 1;
			break;

		case DDS_DIMENSION_TEXTURE3D:
			if (!(header->flags & DDS_HEADER_FLAGS_VOLUME)) {
				NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
			}
			if (arraySize > 1) {
				NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
			}
			break;

		default:
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}

		resDim = d3d10ext.resourceDimension;
	} else {
		format = GetPixelFormat(header->ddspf);

		if (format == PixelFormat::UNKNOWN) {
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}

		if (header->flags & DDS_HEADER_FLAGS_VOLUME) {
			resDim = DDS_DIMENSION_TEXTURE3D;
		} else {
			if (header->caps2 & DDS_CUBEMAP) {
				// We require all six faces to be defined
				if ((header->caps2 & DDS_CUBEMAP_ALLFACES)
						!= DDS_CUBEMAP_ALLFACES) {
					NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
				}
				arraySize = 6;
				isCubeMap = true;
			}

			metaInfo.metaInfo.maxDepth = 1;
			resDim = DDS_DIMENSION_TEXTURE2D;
			// Note there's no way for a legacy Direct3D 9 DDS to express a '1D' texture
		}

		//assert(BitsPerPixel(format) != 0);
	}

	switch (resDim) {
	case DDS_DIMENSION_TEXTURE1D:
		if (((uint32)arraySize > RenderConstants::MAX_TEXTURE_LAYER)
				|| ((uint32)metaInfo.metaInfo.maxWidth > RenderConstants::MAX_TEXTURE_DIM)) {
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}
		break;

	case DDS_DIMENSION_TEXTURE2D:
		if (isCubeMap) {
			// This is the right bound because we set arraySize to (NumCubes*6) above
			if (((uint32)arraySize > RenderConstants::MAX_TEXTURE_LAYER)
					|| ((uint32)metaInfo.metaInfo.maxWidth > RenderConstants::MAX_TEXTURE_DIM)
					|| ((uint32)metaInfo.metaInfo.maxHeight > RenderConstants::MAX_TEXTURE_DIM)) {
				NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
			}
		} else if (((uint32)arraySize > RenderConstants::MAX_TEXTURE_LAYER)
				|| ((uint32)metaInfo.metaInfo.maxWidth > RenderConstants::MAX_TEXTURE_DIM)
				|| ((uint32)metaInfo.metaInfo.maxHeight > RenderConstants::MAX_TEXTURE_DIM)) {
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}
		break;

	case DDS_DIMENSION_TEXTURE3D:
		if ((arraySize > 1)
				|| ((uint32)metaInfo.metaInfo.maxWidth > RenderConstants::MAX_TEXTURE_DIM)
				|| ((uint32)metaInfo.metaInfo.maxWidth > RenderConstants::MAX_TEXTURE_DIM)
				|| ((uint32)metaInfo.metaInfo.maxDepth > RenderConstants::MAX_TEXTURE_DIM)) {
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}
		break;
	}

	return FillInitData(ser, arraySize, format, params, metaInfo);
}

void DDSImageCodec::GetSurfaceInfo(size_t width, size_t height, PixelFormat fmt,
		size_t* outNumBytes, size_t* outRowBytes, size_t* outNumRows) {
	size_t numBytes = 0;
	size_t rowBytes = 0;
	size_t numRows = 0;

	bool bc = false;
	bool packed = false;
	size_t bcnumBytesPerBlock = 0;

	/*switch (fmt) {
	 case DXGI_FORMAT_BC1_TYPELESS:
	 case DXGI_FORMAT_BC1_UNORM:
	 case DXGI_FORMAT_BC1_UNORM_SRGB:
	 case DXGI_FORMAT_BC4_TYPELESS:
	 case DXGI_FORMAT_BC4_UNORM:
	 case DXGI_FORMAT_BC4_SNORM:
	 bc = true;
	 bcnumBytesPerBlock = 8;
	 break;

	 case DXGI_FORMAT_BC2_TYPELESS:
	 case DXGI_FORMAT_BC2_UNORM:
	 case DXGI_FORMAT_BC2_UNORM_SRGB:
	 case DXGI_FORMAT_BC3_TYPELESS:
	 case DXGI_FORMAT_BC3_UNORM:
	 case DXGI_FORMAT_BC3_UNORM_SRGB:
	 case DXGI_FORMAT_BC5_TYPELESS:
	 case DXGI_FORMAT_BC5_UNORM:
	 case DXGI_FORMAT_BC5_SNORM:
	 case DXGI_FORMAT_BC6H_TYPELESS:
	 case DXGI_FORMAT_BC6H_UF16:
	 case DXGI_FORMAT_BC6H_SF16:
	 case DXGI_FORMAT_BC7_TYPELESS:
	 case DXGI_FORMAT_BC7_UNORM:
	 case DXGI_FORMAT_BC7_UNORM_SRGB:
	 bc = true;
	 bcnumBytesPerBlock = 16;
	 break;

	 case DXGI_FORMAT_R8G8_B8G8_UNORM:
	 case DXGI_FORMAT_G8R8_G8B8_UNORM:
	 packed = true;
	 break;
	 }*/

	if (bc) {
		size_t numBlocksWide = 0;
		if (width > 0) {
			numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
		}
		size_t numBlocksHigh = 0;
		if (height > 0) {
			numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
		}
		rowBytes = numBlocksWide * bcnumBytesPerBlock;
		numRows = numBlocksHigh;
	} else if (packed) {
		rowBytes = ((width + 1) >> 1) * 4;
		numRows = height;
	} else {
		size_t bpp = PixelUtils::BytesPerPixel(fmt) * 4;
		rowBytes = (width * bpp + 7) / 8; // round up to nearest byte
		numRows = height;
	}

	numBytes = rowBytes * numRows;
	if (outNumBytes) {
		*outNumBytes = numBytes;
	}
	if (outRowBytes) {
		*outRowBytes = rowBytes;
	}
	if (outNumRows) {
		*outNumRows = numRows;
	}
}

ImageData DDSImageCodec::FillInitData(InputSerializer& ser, size_t arraySize,
		PixelFormat format, const ImageParams& params, ImageCodecMetaInfo& metaInfo) {

	ImageData ret = { 0 };

	size_t numBytes = 0;
	size_t rowBytes = 0;
	size_t numRows = 0;

	uint32 baseMipLevel = params.baseMipLevel;
	uint32 numMipLevelToLoad = params.numMipLevelToLoad;

	if (params.numMipLevelToLoad == Image::IMAGE_ALL_MIP_LEVELS) {
		numMipLevelToLoad = metaInfo.metaInfo.maxMipMapCount;
	}

	if (baseMipLevel == Image::IMAGE_HIGHEST_MIP_LEVEL) {
		baseMipLevel = 0;
	} else if (baseMipLevel == Image::IMAGE_LOWEST_MIP_LEVEL) {
		baseMipLevel = metaInfo.metaInfo.maxMipMapCount - numMipLevelToLoad;
	}

	if (baseMipLevel + numMipLevelToLoad > metaInfo.metaInfo.maxMipMapCount)
		numMipLevelToLoad = metaInfo.metaInfo.maxMipMapCount - baseMipLevel;

	ret.format = format;
	ret.numFaces = (uint8)arraySize;
	ret.numMipMaps = numMipLevelToLoad;

	size_t highestMipLevel = baseMipLevel + numMipLevelToLoad;
	size_t totalBufferSize = 0;
	// misleading but the math works out like this
	metaInfo.mipLevelsToRead = baseMipLevel;
	// calculate total buffer size
	for (uint16 j = 0; j < (uint16)arraySize; j++) {
		uint16 w = metaInfo.metaInfo.maxWidth;
		uint16 h = metaInfo.metaInfo.maxHeight;
		uint16 d = metaInfo.metaInfo.maxDepth;

		for (uint16 i = 0; i < metaInfo.metaInfo.maxMipMapCount; i++) {
			GetSurfaceInfo(w, h, format, &numBytes, &rowBytes, &numRows);
			// mip levels loaded are
			// baseMipLevel <= i < baseMipLevel + numMipLevelsToLoad
			if (baseMipLevel <= i && i < highestMipLevel) {
				if (!ret.width) {
					ret.width = w;
					ret.height = h;
					ret.depth = d;
				}
				totalBufferSize += (numBytes * d);
			}

			w = w >> 1;
			h = h >> 1;
			d = d >> 1;
			if (w == 0) {
				w = 1;
			}
			if (h == 0) {
				h = 1;
			}
			if (d == 0) {
				d = 1;
			}
		}
	}

	ret.data = NEX_ALLOC(totalBufferSize, MEMCAT_GENERAL);
	uint32 bpp = PixelUtils::BytesPerPixel(format);

	uint8* byteArr = (uint8*) ret.data;
	for (uint16 j = 0; j < (uint16)arraySize; j++) {
		uint16 w = metaInfo.metaInfo.maxWidth;
		uint16 h = metaInfo.metaInfo.maxHeight;
		uint16 d = metaInfo.metaInfo.maxDepth;

		for (uint16 i = 0; i < metaInfo.metaInfo.maxMipMapCount; i++) {
			GetSurfaceInfo(w, h, format, &numBytes, &rowBytes, &numRows);
			// mip levels loaded are
			// baseMipLevel <= i < baseMipLevel + numMipLevelsToLoad
			if (baseMipLevel <= i && i < highestMipLevel) {
				if (!ret.width) {
					ret.width = w;
					ret.height = h;
					ret.depth = d;
				}
#if NEX_NEED_ENDIAN_SWAPPING
				switch(bpp) {
					case 1: {
						InputSerializer::UByteArray arr(byteArr, numBytes*d);
						ser >> arr;
					}
					break;
					case 2: {
						InputSerializer::UShortArray arr((uint16*)byteArr, (numBytes*d)>>1);
						ser >> arr;
					}
					break;
					case 4: {
						InputSerializer::UIntArray arr((uint32*)byteArr, (numBytes*d)>>2);
						ser >> arr;
					}
					break;
					case 8: {
						InputSerializer::UInt64Array arr((uint64*)byteArr, (numBytes*d)>>3);
						ser >> arr;
					}
					break;
					case 16: {
						InputSerializer::UInt64Array arr((uint64*)byteArr, (numBytes*d)>>3);
						ser >> arr;
						size_t c = arr.second/2;
						for (uint32 i = 0; i < c; ++i) {
							uint64 t = arr.first[i*2 + 0];
							arr.first[i*2 + 0] = arr.first[i*2 + 1];
							arr.first[i*2 + 1] = t;
						}
					}
					break;
					default:
					NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
					//
				}
#else
				InputSerializer::UByteArray arr(byteArr, numBytes * d);
				ser >> arr;
#endif
			} else {
				ser.Seek(numBytes * d, std::ios_base::cur);
			}

			w = w >> 1;
			h = h >> 1;
			d = d >> 1;
			if (w == 0) {
				w = 1;
			}
			if (h == 0) {
				h = 1;
			}
			if (d == 0) {
				d = 1;
			}
		}
	}
	return ret;
}

void DDSImageCodec::Save(OutputStreamPtr& file, const ImageParams& params,
		const ImageData& data) {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}

} /* namespace nextar */
