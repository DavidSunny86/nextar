/*
 * DDSImageLoader.h
 *
 *  Created on: 03-Nov-2013
 *      Author: obhi
 */

#ifndef DDSIMAGELOADER_H_
#define DDSIMAGELOADER_H_

#include <Image.h>

using namespace nextar;

namespace ImageLoader {

	struct DDS_HEADER;
	class DDS_PIXELFORMAT;
	class DDSImageCodec: public nextar::ImageCodec {
		NEX_LOG_HELPER(DDSImageCodec);

	public:

		static DDSImageCodec codec;

		DDSImageCodec();
		virtual ~DDSImageCodec();

		virtual bool TryLoad(InputStreamPtr& file, const ImageParams& params);
		virtual ImageData Load(InputStreamPtr& file, const ImageParams&);
		virtual void Save(OutputStreamPtr& file, const ImageParams& params, const ImageData& data);

	protected:

		static PixelFormat GetPixelFormat(DDS_PIXELFORMAT&);
		static PixelFormat GetPixelFormat(uint32 dxgiFormat);

		static void GetSurfaceInfo(size_t width, size_t height, PixelFormat fmt,
					size_t* outNumBytes, size_t* outRowBytes, size_t* outNumRows);
		ImageData FillInitData(InputSerializer& file, ImageData& initialData, size_t arraySize, PixelFormat, const ImageParams& params);
		ImageData CreateImageData(InputSerializer& file, DDS_HEADER* header, size_t offset, const ImageParams& params);
	};

} /* namespace nextar */
#endif /* DDSIMAGELOADER_H_ */
