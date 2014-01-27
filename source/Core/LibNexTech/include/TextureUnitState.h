/* 
 * File:   TextureUnit.h
 * Author: obhi
 *
 * Created on May 10, 2011, 5:12 PM
 */

#ifndef TEXTUREUNITSTATE_H
#define	TEXTUREUNITSTATE_H

#include "NexSys.h"
#include "StringUtils.h"
#include "Color.h"
#include "Texture.h"

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
		TF_MAG_NEAREST,
		TF_MAG_LINEAR
	};

	typedef uint8 TextureMagFilterType;

	enum TextureAddressMode {
		TAM_WRAP,
		TAM_MIRROR,
		TAM_CLAMP,
		TAM_BORDER,
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

	/* @todo Incorporate unit type field specifying what kind of texture this
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
		uint16 bindPoint; // @todo Might not be required
		Vector2 uvTile;
		Vector2 uvOffset;
		TextureBase* texture;

		inline TextureUnit()
				: bindPoint(-1), uvTile(1, 1), uvOffset(0, 0), texture(0) {
		}

		inline TextureUnit(int16 _bindPoint, TextureBase* _texture, const Vector2& tile,
				const Vector2& offset)
				: bindPoint(_bindPoint), texture(_texture), uvTile(tile), uvOffset(
						offset) {
			if (texture && texture->IsTextureAsset())
				static_cast<Texture*>(texture)->AddRef();
		}

		inline TextureUnit(int16 _bindPoint, TextureBase* _texture)
				: bindPoint(_bindPoint), texture(_texture), uvTile(1, 1), uvOffset(
						0, 0) {
			if (texture && texture->IsTextureAsset())
				static_cast<Texture*>(texture)->AddRef();
		}

		inline TextureUnit(const TextureUnit& tu) : bindPoint(tu.bindPoint), texture(tu.texture), 
			uvTile(tu.uvTile), uvOffset(tu.uvOffset) {
		}

		inline ~TextureUnit() {
			if (texture && texture->IsTextureAsset())
				static_cast<Texture*>(texture)->Release();
		}

		inline void SetTexture(TextureBase* _texture) {
			if (texture && texture->IsTextureAsset())
				static_cast<Texture*>(texture)->Release();
			texture = _texture;
			if (texture && texture->IsTextureAsset())
				static_cast<Texture*>(texture)->AddRef();
		}

		inline TextureUnit& operator = (TextureBase* _texture) {
			SetTexture(_texture);
			return *this;
		}

		inline TextureBase* operator -> () {
			return texture;
		}

		inline operator TextureBase* () {
			return texture;
		}

		inline const TextureBase* operator -> () const {
			return texture;
		}

		inline operator const TextureBase* () const {
			return texture;
		}
				
	};

	//typedef map<String, TextureUnit>::type TextureUnitMap;
}

#endif	/* TEXTUREUNITSTATE_H */

