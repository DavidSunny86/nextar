/* 
 * File:   JpegImageLoader.h
 * Author: obhi
 *
 * Created on May 8, 2011, 2:29 AM
 */

#ifndef JPEGIMAGELOADER_H
#define	JPEGIMAGELOADER_H

#include <Image.h>

using namespace nextar;

namespace ImageLoader {
	class JPEGImageCodec : public ImageCodec {
	    NEX_LOG_HELPER(JPEGImageCodec);

	public:

	    static JPEGImageCodec codec;

	    JPEGImageCodec();
	    virtual ~JPEGImageCodec();

	    virtual bool TryLoad(InputStreamPtr& file, const ImageParams& params);
	    virtual ImageData Load(InputStreamPtr& file, const ImageParams& params);
	    virtual void Save(OutputStreamPtr& file, const ImageParams& params, const ImageData& data);

	private:
	};
}

#endif	/* JPEGIMAGELOADER_H */

