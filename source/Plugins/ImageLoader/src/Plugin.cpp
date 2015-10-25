/* 
 * File:   Plugin.cpp
 * Author: obhi
 * 
 * Created on May 8, 2011, 2:23 AM
 */

#include <BaseHeaders.h>
#include <Plugin.h>
#include <Image.h>

NEX_IMPLEMENT_PLUGIN(ImageLoader, ImageLoader::Plugin);

namespace ImageLoader {

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
	Image::Serializer::RegisterFactory("TGA", &TGAImageCodec::codec);
}

bool Plugin::LicenseExpired() {
	Image::Serializer::UnregisterFactory("PNG");
	Image::Serializer::UnregisterFactory("JPG");
	Image::Serializer::UnregisterFactory("DDS");
	Image::Serializer::UnregisterFactory("TGA");
	return true;
}

void Plugin::Close() {
}

void Plugin::Dispose() {
}
}
