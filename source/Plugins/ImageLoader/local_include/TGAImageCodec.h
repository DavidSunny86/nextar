/* 
 * File:   JpegImageLoader.h
 * Author: obhi
 *
 * Created on May 8, 2011, 2:29 AM
 */

#ifndef TGAIMAGELOADER_H
#define	TGAIMAGELOADER_H

#include <Image.h>

using namespace nextar;

namespace ImageLoader {
class TGAImageCodec: public ImageCodec {
	NEX_LOG_HELPER(TGAImageCodec);

public:

	static TGAImageCodec codec;

	TGAImageCodec();
	virtual ~TGAImageCodec();

	virtual bool TryLoad(InputStreamPtr& file, const ImageParams& params, ImageCodecMetaInfo& metaInfo) override;
	virtual ImageData Load(InputStreamPtr& file, const ImageParams& params, ImageCodecMetaInfo& metaInfo) override;
	virtual void Save(OutputStreamPtr& file, const ImageParams& params,
			const ImageData& data) override;

private:
	static void Spread(uint8* data, uint32 width, uint32 height);
	ImageData ReadCompressed(InputSerializer& ser, const ImageParams& params, ImageCodecMetaInfo& metaInfo);
	ImageData ReadUncompressed(InputSerializer& ser, const ImageParams& params, ImageCodecMetaInfo& metaInfo);
};
}

#endif	/* TGAIMAGELOADER_H */

