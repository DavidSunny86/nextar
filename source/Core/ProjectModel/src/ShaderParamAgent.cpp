/*
 * ShaderParamAgent.cpp
 *
 *  Created on: 27-Jul-2014
 *      Author: obhi
 */

#include <NexProjectModel.h>
#include <ShaderParamAgent.h>

namespace nextar {

ShaderParamScalar ShaderParamScalar::agent;
ShaderParamVector ShaderParamVector::agent;
ShaderParamTexture ShaderParamTexture::agent;
ShaderParamAgent::ShaderParamAgentMap ShaderParamAgent::agents;

ShaderParamAgent* ShaderParamAgent::GetAgent(ParamDataType type) {
	if (!agents.size()) {
		agents[ParamDataType::PDT_BOOL] = &ShaderParamScalar::agent;
		agents[ParamDataType::PDT_UINT] = &ShaderParamScalar::agent;
		agents[ParamDataType::PDT_INT] = &ShaderParamScalar::agent;
		agents[ParamDataType::PDT_FLOAT] = &ShaderParamScalar::agent;
		agents[ParamDataType::PDT_TEXTURE] = &ShaderParamTexture::agent;
		agents[ParamDataType::PDT_VEC2] = &ShaderParamVector::agent;
		agents[ParamDataType::PDT_VEC3] = &ShaderParamVector::agent;
		agents[ParamDataType::PDT_VEC4] = &ShaderParamVector::agent;
		agents[ParamDataType::PDT_IVEC2] = &ShaderParamVector::agent;
		agents[ParamDataType::PDT_IVEC3] = &ShaderParamVector::agent;
		agents[ParamDataType::PDT_IVEC4] = &ShaderParamVector::agent;
		agents[ParamDataType::PDT_MAT4x4] = &ShaderParamVector::agent;
		agents[ParamDataType::PDT_MAT3x4] = &ShaderParamVector::agent;
	}

	auto it = agents.find(type);
	if (it != agents.end()) {
		return (*it).second;
	}
	return nullptr;
}

void ShaderParamAgent::SetParamValue(size_t offset,
		MaterialAsset::MaterialLoadRequest* request, const ParamEntry& pe,
		const String& value) {
	size_t start = 0;
	bool stop = false;
	size_t stride = pe.maxSize / pe.arrayCount;
	for (uint32 i = 0; i < pe.arrayCount && !stop; i++) {
		size_t n = value.find_first_of(',', start);
		if (n == String::npos) {
			n = value.length();
			stop = true;
		}
		String v = value.substr(start, n-start);
		if (v.length()) {
			StringUtils::Trim(v);
			SetParamValueImpl(offset, request, pe, v);
			offset += stride;
		}
	}
}

void ShaderParamScalar::SetParamValueImpl(size_t offset,
		MaterialAsset::MaterialLoadRequest* request, const ParamEntry& pe,
		const String& value) {
	switch(pe.type) {
	case ParamDataType::PDT_BOOL:
	{
		bool c_value = Convert::ToBool(value);
		request->SetParamValue(offset, &c_value, 1);
	}
	break;
	case ParamDataType::PDT_UINT:
	{
		int32 c_value = Convert::ToLong(value);
		request->SetParamValue(offset, &c_value, 4);
	}
	break;
	case ParamDataType::PDT_INT:
	{
		uint32 c_value = Convert::ToULong(value);
		request->SetParamValue(offset, &c_value, 4);
	}
	break;
	case ParamDataType::PDT_FLOAT:
	{
		float c_value = Convert::ToFloat(value);
		request->SetParamValue(offset, &c_value, 4);
	}
	break;
	default:
		NEX_ASSERT(0);
	}
}

void ShaderParamVector::SetParamValueImpl(size_t offset,
		MaterialAsset::MaterialLoadRequest* request, const ParamEntry& pe,
		const String& value) {
	switch(pe.type) {
	case ParamDataType::PDT_VEC2:
	{
		Vector2 c_value = Convert::ToVector2(value);
		request->SetParamValue(offset, &c_value, 4*2);
	}
	break;
	case ParamDataType::PDT_VEC3:
	{
		Vector3 c_value = Convert::ToVector3(value);
		request->SetParamValue(offset, &c_value, 4*3);
	}
	break;
	case ParamDataType::PDT_VEC4:
	{
		Vector4 c_value = Convert::ToVector4(value);
		request->SetParamValue(offset, &c_value, 4*4);
	}
	break;
	case ParamDataType::PDT_IVEC2:
	{
		IVector2 c_value = Convert::ToIVector2(value);
		request->SetParamValue(offset, c_value.data(), 4*2);
	}
	break;
	case ParamDataType::PDT_IVEC3:
	{
		IVector3 c_value = Convert::ToIVector3(value);
		request->SetParamValue(offset, c_value.data(), 4*3);
	}
	break;
	case ParamDataType::PDT_IVEC4:
	{
		IVector4 c_value = Convert::ToIVector4(value);
		request->SetParamValue(offset, c_value.data(), 4*4);
	}
	break;
	default:
		NEX_ASSERT(0);
	}
}

void ShaderParamMatrix::SetParamValueImpl(size_t offset,
		MaterialAsset::MaterialLoadRequest* request, const ParamEntry& pe,
		const String& value) {
	switch(pe.type) {
	case ParamDataType::PDT_MAT4x4:
	{
		Matrix4x4 c_value = Convert::ToMat4x4(value);
		request->SetParamValue(offset, &c_value, 4*16);
	}
	break;
	case ParamDataType::PDT_MAT3x4:
	{
		Matrix3x4 c_value = Convert::ToMat3x4(value);
		request->SetParamValue(offset, &c_value, 4*12);
	}
	break;
	default:
		NEX_ASSERT(0);
	}
}

void ShaderParamTexture::SetParamValueImpl(size_t offset,
		MaterialAsset::MaterialLoadRequest* request, const ParamEntry& pe,
		const String& value) {
	URL locator;
	Asset::ID objectId;
	StringUtils::TokenIterator it = 0;
	{
	String url;
	it = StringUtils::NextWord(value, url, it);
	if (url.length() <= 0)
		return;
	locator = URL(url);
	}
	if (it != String::npos) {
		String id;
		it = StringUtils::NextWord(value, id, it);
		objectId = SharedComponent::ToID(id);
	} else
		objectId = Asset::ToID(locator);
	TextureAssetPtr texture = TextureAsset::Traits::Instance(objectId, locator);
	TextureUnit unit(texture);
	request->SetTextureValue(offset, &unit);
}

} /* namespace MaterialCompiler */
