/* 
 * File:   TextureUnit.h
 * Author: obhi
 *
 * Created on May 10, 2011, 5:12 PM
 */

#ifndef TEXTUREUNITSTATE_H
#define	TEXTUREUNITSTATE_H

#include <NexBase.h>
#include <StringUtils.h>
#include <Color.h>
#include <TextureAsset.h>

namespace nextar {

enum TextureMinFilter {
	TF_MIN_NEAREST,
	TF_MIN_LINEAR,
	TF_MIN_NEAREST_MIPMAP_NEAREST,
	TF_MIN_LINEAR_MIPMAP_NEAREST,
	TF_MIN_NEAREST_MIPMAP_LINEAR,
	TF_MIN_LINEAR_MIPMAP_LINEAR,
};

typedef uint8 TextureMinFilterType;

enum TextureMagFilter {
	TF_MAG_NEAREST, TF_MAG_LINEAR
};

typedef uint8 TextureMagFilterType;

enum TextureAddressMode {
	TAM_WRAP, TAM_MIRROR, TAM_CLAMP, TAM_BORDER,
};

typedef uint8 TextureAddressModeType;

enum TextureComparisonMode {
	TEXCOMP_NEVER,
	TEXCOMP_LESS,
	TEXCOMP_EQUAL,
	TEXCOMP_LESS_EQUAL,
	TEXCOMP_GREATER,
	TEXCOMP_NOT_EQUAL,
	TEXCOMP_GREATER_EQUAL,
	TEXCOMP_ALWAYS,
	/* Texture comparison disabled */
	TEXCOMP_NONE,
};

typedef uint8 TextureComparisonModeType;

/* todo Incorporate unit type field specifying what kind of texture this
 * is going to use like:
 * a. Mesh Texture
 * b. Shadow Texture
 * c. Render Target
 * e. Selective Texture
 */
struct TextureUnitParams {
	TextureMinFilterType minFilter;
	TextureMagFilterType magFilter;
	TextureAddressModeType uAddress;
	TextureAddressModeType vAddress;
	TextureAddressModeType wAddress;
	TextureComparisonModeType comparisonFunc;
	uint16 flags;
	uint16 updateFrequency;
	uint32 maxAnisotropy;
	float lodBias;
	float minLod;
	float maxLod;
	Color borderColor;
};

/* A texture unit that can be submitted to the
 * context and bound to the shader at a specific bind point.
 */
class TextureUnit {
public:
	TextureBase* texture;

	inline TextureUnit() :
			texture(0) {
	}

	inline TextureUnit(TextureBase* _texture) :
			texture(_texture) {
		if (texture && texture->IsTextureAsset())
			static_cast<TextureAsset*>(texture)->AddRef();
	}

	inline TextureUnit(const TextureUnit& tu) :
			texture(tu.texture) {
	}

	inline ~TextureUnit() {
		if (texture && texture->IsTextureAsset())
			static_cast<TextureAsset*>(texture)->Release();
	}

	inline void SetTexture(TextureBase* _texture) {
		if (texture && texture->IsTextureAsset())
			static_cast<TextureAsset*>(texture)->Release();
		texture = _texture;
		if (texture && texture->IsTextureAsset())
			static_cast<TextureAsset*>(texture)->AddRef();
	}

	inline TextureUnit& operator =(TextureBase* _texture) {
		SetTexture(_texture);
		return *this;
	}

	inline TextureBase* operator ->() {
		return texture;
	}

	inline operator TextureBase*() {
		return texture;
	}

	inline const TextureBase* operator ->() const {
		return texture;
	}

	inline operator const TextureBase*() const {
		return texture;
	}
};

}

#endif	/* TEXTUREUNITSTATE_H */

