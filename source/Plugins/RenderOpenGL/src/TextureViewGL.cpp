/*
 * TextureViewGL.cpp
 *
 *  Created on: 25-May-2014
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <TextureViewGL.h>
#include <RenderContext_Base_GL.h>

namespace RenderOpenGL {

TextureViewGL::TextureViewGL() :
texture(0), target(0) {
	pixelFormat.internalFormat = GL_NONE;
}

TextureViewGL::~TextureViewGL() {
}

void TextureViewGL::Destroy(nextar::RenderContext* rc) {
	auto gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->DestroyTexture(texture);
	texture = 0;
}

void TextureViewGL::ReadPixels(RenderContext_Base_GL* gl, TextureBase::ReadPixelUpdateParams& params) {
	if (params.lock.test_and_set(std::memory_order_release))
		return;
	PixelBox* pixels = params.box;
	gl->ActivateTexture(target, texture);
	Size dim = gl->GetTextureParams(target);

	pixels->data = static_cast<uint8*>(
		NEX_ALLOC(dim.dx * dim.dy * pixelFormat.pixelSize, MEMCAT_GENERAL));
	pixels->left = 0;
	pixels->right = dim.dx;
	pixels->top = 0;
	pixels->bottom = dim.dy;
	pixels->front = 0;
	pixels->back = 1;
	pixels->format = pixelFormat.textureFormat;
	pixels->deleteData = true;
	gl->GetTextureData(target, pixelFormat.internalFormat, pixelFormat.dataType, pixels->data);
	pixels->CalculatePitches();
}

void TextureViewGL::Update(nextar::RenderContext* rc, uint32 msg,
	ContextObject::ContextParamPtr params) {

	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	if (msg & TextureBase::MSG_TEX_READ) {
		ReadPixels(gl, *const_cast<TextureBase::ReadPixelUpdateParams*>(
			reinterpret_cast<const TextureBase::ReadPixelUpdateParams*>(params)));
		return;
	}

	// todo Incorrect implementation
	const TextureBase::UpdateParams& textureParams =
		*reinterpret_cast<const TextureBase::UpdateParams*>(params);

	PixelFormat imageFormat = textureParams.textureFormat;
	if (textureParams.image)
		imageFormat = textureParams.image->GetFormat();

	if (!IsCreated()) {
		pixelFormat = RenderContext_Base_GL::GetGlPixelFormat(imageFormat,
			textureParams.textureFormat);

		if (pixelFormat.internalFormat == GL_NONE) {
			Warn(
				"Currently image should be of compatible format with texture!");
			return;
		}

		texture = gl->CreateTexture();
		target = RenderContext_Base_GL::GetGlTextureType(textureParams.type);
		gl->ActivateTexture(target, texture);

		if (textureParams.textureFlags & TextureBase::PRE_ALLOCATE_STORAGE) {
			gl->AllocateTexture(target,
				(GLint)textureParams.desc.maxMipMapCount,
				pixelFormat.internalFormat, textureParams.desc.maxWidth,
				textureParams.desc.maxHeight, textureParams.desc.maxDepth);
		}
	} else
		gl->ActivateTexture(target, texture);

	if (textureParams.image && (msg & TextureBase::MSG_TEX_UPLOAD)) {
		Image& img = *textureParams.image;
		uint32 numMips = img.GetNumMipMaps();
		uint32 numFaces = img.GetNumFaces();
		NEX_ASSERT(numFaces == 1 || numFaces == 6);
		GLenum realTarget = target;
		if (target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_CUBE_MAP_ARRAY)
			realTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		for (uint32 f = 0; f < numFaces; ++f) {
			for (uint32 i = 0; i < numMips; ++i) {
				PixelBox box = img.GetPixelBox(f, i);
				if (!box.IsSimpleArray()) {
					Warn("Pixel box has pixel padding. Cannot copy.");
					continue;
				}
				if (textureParams.textureFlags & TextureBase::PRE_ALLOCATE_STORAGE)
					gl->WriteTextureLevel(realTarget + f,
					(GLint)(textureParams.baseMipLevel + i),
					pixelFormat, box.GetWidth(), box.GetHeight(),
					box.GetDepth(), box.data, box.GetDataSize());
				else
					gl->AllocateTextureLevel(realTarget + f,
					(GLint)(textureParams.baseMipLevel + i),
					pixelFormat, box.GetWidth(), box.GetHeight(),
					box.GetDepth(), box.data, box.GetDataSize());

			}
			gl->SetMipLevels(realTarget + f, textureParams.baseMipLevel, textureParams.lowestMipLevel);
		}
		// update number of mip levels
	}
}

} /* namespace RenderOpenGL */
