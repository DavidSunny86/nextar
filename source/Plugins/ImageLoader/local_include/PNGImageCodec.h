/* 
 * File:   PngImageLoader.h
 * Author: obhi
 *
 * Created on May 8, 2011, 4:57 AM
 */

#ifndef PNGIMAGELOADER_H
#define	PNGIMAGELOADER_H

#include <Image.h>

using namespace nextar;

namespace ImageLoader {
class PNGImageCodec: public ImageCodec {
	NEX_LOG_HELPER (PNGImageCodec);
public:

	static PNGImageCodec codec;

	PNGImageCodec();
	virtual ~PNGImageCodec();

	virtual bool TryLoad(InputStreamPtr& file, const ImageParams& params);
	virtual ImageData Load(InputStreamPtr& file, const ImageParams&);
	virtual void Save(OutputStreamPtr& file, const ImageParams& params,
			const ImageData& data);

private:

};

}

#endif	/* PNGIMAGELOADER_H */

