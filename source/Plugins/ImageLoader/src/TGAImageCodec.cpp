
#include <EngineHeaders.h>
#include <TGAImageCodec.h>
#include <Serializer.h>
#include <RenderManager.h>

namespace ImageLoader {

TGAImageCodec TGAImageCodec::codec;

struct TGAHeader
{
	uint8 header[12];         // File Header To Determine File Type
};

struct TGADesc
{
	uint16 width;               // Width Of Image              
	uint16 height;              // Height Of Image                 
	uint8 bpp;             // Number Of BITS Per Pixel (24 Or 32)
	uint8 pad;          // Holds The First 6 Useful Bytes Of The File
};

static constexpr TGAHeader s_uncoTGAHeader = { { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
static constexpr TGAHeader s_compTGAHeader = { { 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

TGAImageCodec::TGAImageCodec() {
}

TGAImageCodec::~TGAImageCodec() {
}

bool TGAImageCodec::TryLoad(InputStreamPtr& file,
	const ImageParams& params, ImageCodecMetaInfo& metaInfo) {
	TGAHeader h;
	if (file->Read(h.header, sizeof(h.header)) == sizeof(h.header)) {
		if ((std::memcmp(&h, &s_uncoTGAHeader, sizeof(TGAHeader)) == 0) ||
			(std::memcmp(&h, &s_compTGAHeader, sizeof(TGAHeader)) == 0))
			return true;
	}
	return false;
}

ImageData TGAImageCodec::Load(InputStreamPtr& file, const ImageParams& params,
							  ImageCodecMetaInfo& metaInfo) {
	TGAHeader h;
	if (file->Read(h.header, sizeof(h.header)) == sizeof(h.header)) {
		InputSerializer ser(file);
		if ((std::memcmp(&h, &s_uncoTGAHeader, sizeof(TGAHeader)) == 0))
			return ReadUncompressed(ser, params, metaInfo);
		else if ((std::memcmp(&h, &s_compTGAHeader, sizeof(TGAHeader)) == 0))
			return ReadCompressed(ser, params, metaInfo);
		else {
			Error("Unsupported tga format!");
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}
	} else {
		Error("Not a targa file!");
		NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	}
	ImageData d = { 0 };
	return d;
}

void TGAImageCodec::Spread(uint8* data, uint32 width, uint32 height) {

	uint8* source = data;
	uint32* dest = reinterpret_cast<uint32*>(data);
	Color32 prev, cur;
	prev.alpha = 255;
#define READ_SRC_PX(src, c) c.red = source[0]; c.green = source[1]; c.blue = source[2]; source+=3;
	READ_SRC_PX(src, prev);
	for (uint32 i = 1; i < width*height; ++i) {
		READ_SRC_PX(src, cur);
		dest[i-1] = prev.ToRgba();
		prev = cur;
	}
	dest[width*height - 1] = prev.ToRgba();
}

ImageData TGAImageCodec::ReadUncompressed(InputSerializer& ser, const ImageParams& params, ImageCodecMetaInfo& metaInfo) {
	TGADesc desc;
	ser >> desc.width
		>> desc.height
		>> desc.bpp
		>> desc.pad;
	if (((int32)desc.width <= 0) || ((int32)desc.height <= 0)) {
		Error("Invalid data in: " + params.name);
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}

	ImageData data = { 0 };
	data.width = desc.width;
	data.height = desc.height;
	data.depth = 1;
	data.numFaces = 1;
	data.numMipMaps = 1;
	uint32 destBytes = desc.width * desc.height;
	
	switch (desc.bpp) {
	case 24:
	case 32:
		// rgba
		data.format = PixelFormat::RGBA8;
		destBytes <<= 2;
		break;
	case 8:
		data.format = PixelFormat::R8;
		break;
	default:
	{
		Error("Invalid format: " + params.name);
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}
	}
	desc.pad = desc.bpp >> 3;
	size_t srcBytes = (desc.pad) * desc.width * desc.height;
	NEX_ASSERT(srcBytes <= destBytes);
	data.data = NEX_ALLOC(destBytes, MEMCAT_GENERAL);

	uint8* udata = static_cast<uint8*>(data.data);
	InputSerializer::UByteArray arr(udata, (uint32)srcBytes);
	ser >> arr;

	// swap
	if (desc.bpp == 24 || desc.bpp == 32) {
		for (uint32 i = 0; i < srcBytes; i += desc.pad) {
			std::swap(udata[i], udata[i + 2]);
		}
	}

	if (desc.bpp == 24) {
		Spread(udata, desc.width, desc.height);
	}
	// read into the src
	if (!metaInfo.metaInfoInited) {
		metaInfo.metaInfo.maxWidth = desc.width;
		metaInfo.metaInfo.maxHeight = desc.height;
		metaInfo.metaInfo.maxMipMapCount = 1;
		metaInfo.mipLevelsToRead = 0;
		metaInfo.metaInfoInited = true;
	}
	return data;
}

ImageData TGAImageCodec::ReadCompressed(InputSerializer& ser, const ImageParams& params, ImageCodecMetaInfo& metaInfo) {
	TGADesc desc;
	ser >> desc.width
		>> desc.height
		>> desc.bpp
		>> desc.pad;
	if (((int32)desc.width <= 0) || ((int32)desc.height <= 0)) {
		Error("Invalid data in: " + params.name);
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}

	ImageData data = { 0 };
	data.width = desc.width;
	data.height = desc.height;
	data.depth = 1;
	data.numFaces = 1;
	data.numMipMaps = 1;
	uint32 destBytes = desc.width * desc.height;
	switch (desc.bpp) {
	case 24:
	case 32:
		// rgba
		data.format = PixelFormat::RGBA8;
		destBytes <<= 2;
		break;
	case 8:
		data.format = PixelFormat::R8;
		break;
	default:
	{
		Error("Invalid format: " + params.name);
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}
	}
	desc.pad = desc.bpp >> 3;
	size_t srcBytes = (desc.pad) * desc.width * desc.height;
	NEX_ASSERT(srcBytes <= destBytes);
	data.data = NEX_ALLOC(destBytes, MEMCAT_GENERAL);
	uint8* udata = static_cast<uint8*>(data.data);

	uint8 pixel[4];
	InputSerializer::UByteArray r(pixel, desc.pad);
	uint32 pixelCount = desc.width * desc.height;
	uint32 pixelRead = 0;
	uint32 bytesRead = 0;
	uint8  head;
	do {
		ser >> head;
		if (head < 128) {
			head++;
			for (uint32 c = 0; c < head; ++c) {
				ser >> r;
				for (uint32 p = 0; p < desc.pad; ++p) {
					udata[bytesRead++] = pixel[p];
				}
				pixelRead++;
				if (pixelRead > pixelCount) {
					Error("Too many pixels in a run: " + params.name);
					NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
				}
			}
		} else {
			head -= 127;
			ser >> r;
			for (uint32 c = 0; c < head; ++c) {
				for (uint32 p = 0; p < desc.pad; ++p) {
					udata[bytesRead++] = pixel[p];
				}
				pixelRead++;
				if (pixelRead > pixelCount) {
					Error("Too many pixels in a run: " + params.name);
					NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
				}
			}
		}
	} while (pixelRead < pixelCount);

	// swap
	if (desc.bpp == 24 || desc.bpp == 32) {
		for (uint32 i = 0; i < srcBytes; i += desc.pad) {
			std::swap(udata[i], udata[i + 2]);
		}
	}

	if (desc.bpp == 24) {
		Spread(udata, desc.width, desc.height);
	}

	if (!metaInfo.metaInfoInited) {
		metaInfo.metaInfo.maxWidth = desc.width;
		metaInfo.metaInfo.maxHeight = desc.height;
		metaInfo.metaInfo.maxDepth = 1;
		metaInfo.metaInfo.maxMipMapCount = 1;
		metaInfo.mipLevelsToRead = 0;
		metaInfo.metaInfoInited = true;
	}
	return data;
}

void TGAImageCodec::Save(OutputStreamPtr& file, const ImageParams& params,
					 const ImageData& data)  {
	TGADesc desc;
	uint32 bytesPerPixel = PixelUtils::BytesPerPixel(data.format);
	if (bytesPerPixel != 8 && bytesPerPixel != 32) {
		Error("Cannot save in this format: " + params.name);
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}

	desc.width = data.width;
	desc.height = data.height;
	desc.bpp = bytesPerPixel * 4;
	desc.pad = 0;
	uint32 pixelCount = desc.width * desc.height;
	
	file->Write(s_uncoTGAHeader.header, sizeof(s_uncoTGAHeader.header));

	OutputSerializer ser(file);
	
	if (desc.bpp == 32) {
		uint8 pixel[4];
		uint8* udata = (uint8*)data.data;
		OutputSerializer::UByteArray r(pixel, 4);
		for (uint32 i = 0; i < pixelCount; i++) {
			pixel[i * 4 + 0] = udata[i * 4 + 2];
			pixel[i * 4 + 1] = udata[i * 4 + 1];
			pixel[i * 4 + 2] = udata[i * 4 + 0];
			pixel[i * 4 + 3] = udata[i * 4 + 3];
			ser << r;
		}
	} else {
		uint32 srcBytes = bytesPerPixel * pixelCount;
		OutputSerializer::UByteArray arr((uint8*)data.data, srcBytes);
		ser << arr;
	}
}

}