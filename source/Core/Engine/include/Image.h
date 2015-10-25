/* 
 * File:   Image.h
 * Author: obhi
 *
 * Created on May 5, 2011, 4:25 PM
 */

#ifndef IMAGE_H
#define	IMAGE_H

#include <NexBase.h>
#include <PixelFormat.h>

namespace nextar {

struct ImageMetaInfo {
	uint16 maxDepth;
	uint16 maxHeight;
	uint16 maxWidth;
	uint16 maxMipMapCount;
};

struct ImageCodecMetaInfo {
	bool metaInfoInited;
	uint16 mipLevelsToRead;
	ImageMetaInfo metaInfo;
};

struct ImageParams {
	bool progressiveLoad;
	String name;
	String codecName;
	uint16 baseMipLevel;
	uint16 numMipLevelToLoad;
};

struct ImageData {
	uint8 numFaces;
	uint8 numMipMaps;
	uint16 depth;
	uint16 height;
	uint16 width;
	PixelFormat format;
	// always allocated on MEMCAT_GENERAL
	void* data;
};

class ImageCodec {
public:
	/* Must return true, if this codec can load the file specified */
	virtual bool TryLoad(InputStreamPtr& file, const ImageParams& params,
			ImageCodecMetaInfo& metaInfo) = 0;

	virtual ImageData Load(InputStreamPtr& file, const ImageParams& params,
			ImageCodecMetaInfo& metaInfo) = 0;
	virtual void Save(OutputStreamPtr& file, const ImageParams& params,
			const ImageData& data) = 0;

protected:
	~ImageCodec() {
	}
};

/* One Dimension array of bytes, accessed as per requirements.
 * Can be layered or 3D, etc, image is laid out as such:
 * (face0,mip0) imagedata(x,y,z)
 * (face0,mip1) imagedata(x,y,z)
 * (face1,mip0) imagedata(x,y,z)
 * (face1,mip1) imagedata(x,y,z)
 */
class _NexEngineAPI Image : public AllocGraphics {
	NEX_LOG_HELPER(Image);
public:

	enum {
		CLASS_ID = 0xffba, // arbitrary number?
	};

	class _NexEngineAPI Serializer : public AllocGeneral {
		NEX_DECLARE_FACTORY(ImageCodec);

		const ImageParams& params;
	public:

		inline Serializer(const ImageParams& imgparams) : params(imgparams) {}
		ImageData Serialize(InputStreamPtr& stream, ImageCodecMetaInfo& metaInfo);
		void Serialize(OutputStreamPtr& stream, const ImageData& data);
	};

	enum {
		IMAGE_LOWEST_MIP_LEVEL = 255,
		IMAGE_HIGHEST_MIP_LEVEL = 0,
		IMAGE_ALL_MIP_LEVELS = 0,
	};

	Image();
	Image(PixelBox&& pb);
	virtual ~Image();

	PixelBox GetPixelBox(uint32 face, uint32 mipLevel);

	inline void* GetDataBuffer() const {
		return dataBuffer;
	}

	inline PixelFormat GetFormat() const {
		return format;
	}

	inline uint16 GetWidth() const {
		return width;
	}

	inline uint16 GetHeight() const {
		return height;
	}

	inline uint16 GetDepth() const {
		return depth;
	}

	inline uint16 GetNumMipMaps() const {
		return numMipMaps;
	}

	inline uint16 GetNumFaces() const {
		return numFaces;
	}

	void Load(InputStreamPtr& input, const ImageParams& params, ImageCodecMetaInfo& metaInfo);
	void Save(OutputStreamPtr& input, const ImageParams& params);

private:

	/* total mip map count in this image file,
	 * is equal to numMipMaps if the whole file was loaded
	 * or the maximum levels present in th file  */
	uint8 numFaces;
	uint8 numMipMaps;
	uint16 depth;
	uint16 height;
	uint16 width;
	PixelFormat format;

	/* temporary data buffer */
	void* dataBuffer;
};
}

#endif	/* IMAGE_H */

