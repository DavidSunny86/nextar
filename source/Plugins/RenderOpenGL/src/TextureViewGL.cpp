/*
 * TextureViewGL.cpp
 *
 *  Created on: 25-May-2014
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <TextureViewGL.h>
#include <RenderContextGL.h>

namespace RenderOpenGL {

	TextureViewGL::TextureViewGL() :
			texture(0), target(0) {
	}

	TextureViewGL::~TextureViewGL() {
	}

	void TextureViewGL::Update(nextar::RenderContext* rc,
			uint32 msg,
			ContextObject::ContextParamPtr params) {
		// todo Incorrect implementation
		RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
		const TextureBase::UpdateParams& textureParams =
				*reinterpret_cast<const TextureBase::UpdateParams*>(params);

		PixelFormat imageFormat = textureParams.textureFormat;
		if(textureParams.image)
			imageFormat = textureParams.image->GetFormat();

		auto pixelFormat = RenderContextGL::GetGlPixelFormat(
				imageFormat,
				textureParams.textureFormat);

		if (pixelFormat.internalFormat == GL_NONE) {
			Warn("Currently image should be of compatible format with texture!");
			return;
		}

		if (!IsCreated()) {
			texture = gl->CreateTexture();
			target = RenderContextGL::GetGlTextureType(textureParams.type);
			gl->ActivateTexture(target, texture);
			if (textureParams.textureFlags
					& TextureBase::PRE_ALLOCATE_STORAGE) {
				gl->AllocateTexture(target, (GLint) textureParams.desc.maxMipMapCount,
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
			if (target == GL_TEXTURE_CUBE_MAP
					|| target == GL_TEXTURE_CUBE_MAP_ARRAY)
				realTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			for (uint32 f = 0; f < numFaces; ++f) {
				for (uint32 i = 0; i < numMips; ++i) {
					PixelBox box = img.GetPixelBox(f, i);
					if (!box.IsSimpleArray()) {
						Warn("Pixel box has pixel padding. Cannot copy.");
						continue;
					}
					if (textureParams.textureFlags
							& TextureBase::PRE_ALLOCATE_STORAGE) {
						gl->AllocateTextureLevel(realTarget + f,
								(GLint) (textureParams.baseMipLevel + i),
								pixelFormat,
								box.GetWidth(),
								box.GetHeight(),
								box.GetDepth(),
								box.data,
								box.GetDataSize());
					} else {
						gl->WriteTextureLevel(realTarget + f,
								(GLint) (textureParams.baseMipLevel + i),
								pixelFormat,
								box.GetWidth(),
								box.GetHeight(),
								box.GetDepth(),
								box.data,
								box.GetDataSize());
					}
				}
			}
		}
	}
} /* namespace RenderOpenGL */
