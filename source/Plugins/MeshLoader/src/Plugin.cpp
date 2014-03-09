/* 
 * File:   Plugin.cpp
 * Author: obhi
 * 
 * Created on May 8, 2011, 2:23 AM
 */

#include <BaseHeaders.h>
#include <Plugin.h>
#include <Image.h>

NEX_IMPLEMENT_PLUGIN(ImageLoader) {
	return &ImageLoader::Plugin::plugin;
}

namespace ImageLoader {

	Plugin Plugin::plugin;

	Plugin::Plugin() {
	}

	Plugin::~Plugin() {
	}

	void Plugin::Open() {
	}

	void Plugin::LicenseRenewed() {
		Image::Serializer::RegisterFactory("PNG", &PNGImageCodec::codec);
	    Image::Serializer::RegisterFactory("JPG", &JPEGImageCodec::codec);
	    Image::Serializer::RegisterFactory("DDS", &DDSImageCodec::codec);
	}

	bool Plugin::LicenseExpired() {
		Image::Serializer::UnregisterFactory("PNG");
	    Image::Serializer::UnregisterFactory("JPG");
	    Image::Serializer::UnregisterFactory("DDS");
	    return true;
	}

	void Plugin::Close() {
	}

	void Plugin::Dispose() {
	}
}
