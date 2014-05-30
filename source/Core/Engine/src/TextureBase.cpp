/*
 * TextureBase.cpp
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */

#include <TextureBase.h>

namespace nextar {

	TextureBase::TextureBase(uint16 flags, ContextObject::Type type) :
		ContextObject(type),
		type(TEXTURE_TYPE_UNKNOWN),
		format(PixelFormat::UNKNOWN),
		width(0), height(0), depth(0), faces(0), numMipMaps(0),
		currentMaxMipLevel(0), textureFlags(flags) {

	}

	TextureBase::~TextureBase() {
	}

} /* namespace nextar */
