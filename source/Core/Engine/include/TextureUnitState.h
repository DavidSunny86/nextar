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
#include <ShaderParamEnums.h>

namespace nextar {

enum TextureMinFilter : uint8 {
	TF_MIN_NEAREST,
	TF_MIN_LINEAR,
	TF_MIN_NEAREST_MIPMAP_NEAREST,
	TF_MIN_LINEAR_MIPMAP_NEAREST,
	TF_MIN_NEAREST_MIPMAP_LINEAR,
	TF_MIN_LINEAR_MIPMAP_LINEAR,
};

enum TextureMagFilter : uint8 {
	TF_MAG_NEAREST,
	TF_MAG_LINEAR
};

enum TextureAddressMode : uint8 {
	TAM_WRAP,
	TAM_MIRROR,
	TAM_CLAMP,
	TAM_BORDER,
};


enum TextureComparisonMode : uint8 {
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

enum TextureUnitType : uint8 {
	MESH_TEXTURE,
	SHADOW_TEXTURE,
	RENDER_TEXTURE,
	SELECTIVE_TEXTURE
};

/* todo Incorporate unit type field specifying what kind of texture this
 * is going to use like:
 * a. Mesh Texture
 * b. Shadow Texture
 * c. Render Target
 * e. Selective Texture
 */
struct TextureUnitParams {
	TextureMinFilter minFilter;
	TextureMagFilter magFilter;
	TextureAddressMode uAddress;
	TextureAddressMode vAddress;
	TextureAddressMode wAddress;
	TextureComparisonMode comparisonFunc;
	ParameterContext context;
	TextureUnitType unitType;
	uint16 flags;
	uint32 maxAnisotropy;
	float lodBias;
	float minLod;
	float maxLod;
	Color borderColor;

	TextureUnitParams() : minFilter(TextureMinFilter::TF_MIN_NEAREST),
		magFilter(TextureMagFilter::TF_MAG_NEAREST), uAddress(TextureAddressMode::TAM_CLAMP),
		vAddress(TextureAddressMode::TAM_CLAMP), wAddress(TextureAddressMode::TAM_CLAMP),
		comparisonFunc(TextureComparisonMode::TEXCOMP_NONE), context(ParameterContext::CTX_UNKNOWN),
		unitType(MESH_TEXTURE), flags(0), maxAnisotropy(0), lodBias(0), minLod(0), maxLod(1000) {
	}
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

