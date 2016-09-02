/*
 * MaterialLoaderImplv10.cpp
 *
 *  Created on: 29-Jul-2014
 *      Author: obhi
 */

#include <MaterialLoaderImplv10.h>

namespace MaterialLoader {

enum MaterialFormatChunkHeaders : uint16 {
	MATERIAL_HEADER = 0x3a7e,
	MATERIAL_PARAMETERS = 0x3196,
};

struct MaterialHeader {
	VersionID version;
	ShaderAsset::ID shader;
	URL shaderLocation;
	String options;
	uint8 layerMask;
};

MaterialLoaderImplv1_0::MaterialLoaderImplv1_0() {

}

MaterialLoaderImplv1_0::~MaterialLoaderImplv1_0() {

}

void MaterialLoaderImplv1_0::Load(InputStreamPtr& stream,
		AssetLoader& loader) {

	MaterialAsset::MaterialLoadRequest* request = static_cast<MaterialAsset::MaterialLoadRequest*>(
			loader.GetRequestPtr());
	MaterialAsset* material = static_cast<MaterialAsset*>(
				request->GetStreamedObject());

	MaterialHeader header;
	ChunkInputStream ser(stream);
	// right now all buffers are shared by subMesh
	InputSerializer::Chunk chunk = ChunkInputStream::Invalid;
	InputSerializer::Chunk paramChunk = ChunkInputStream::Invalid;
	ser.ReadChunk(MATERIAL_HEADER, chunk);
	if (InputSerializer::IsValid(chunk)) {
		ser >> header.version;
		if (!TestVersion(NEX_MAKE_VERSION(1,0,0), header.version)) {
			Error("Material is of higher version: " + request->GetName());
			request->SetCompleted(false);
			return;
		}

		ser >> header.layerMask
			>> header.shader >> header.shaderLocation
			>> header.options;
		request->SetLayer((Layer)header.layerMask);
		request->SetShader(header.shader, header.shaderLocation);
	} else {
		Error("Material does not have a valid header: " +request->GetName());
		request->SetCompleted(false);
		return;
	}

	ser.ReadChunk(MATERIAL_PARAMETERS, paramChunk, chunk);
	if (InputSerializer::IsValid(paramChunk)) {
		ParameterBuffer buffer;
		buffer.AsyncLoad(ser, request);
		request->SetParameterBuffer(std::move(buffer));
	} else {
		Error("Material does not have a valid param buffer: " + request->GetName());
		request->SetCompleted(false);
		return;
	}

	request->SetCompleted(true);
	return;
}

} /* namespace MaterialLoader */
