/*
 * MaterialSaverImplv10.cpp
 *
 *  Created on: 28-Jul-2014
 *      Author: obhi
 */
#include <NexProjectModel.h>
#include <MaterialSaverImplv10.h>
#include <MaterialFormatv10.h>
#include <MaterialTemplate.h>

namespace nextar {

MaterialSaverImplv1_0 MaterialSaverImplv1_0::saver;

MaterialSaverImplv1_0::MaterialSaverImplv1_0() {
}

MaterialSaverImplv1_0::~MaterialSaverImplv1_0() {
}

void MaterialSaverImplv1_0::Save(OutputStreamPtr& out, AssetSaver& saver) {
	MaterialTemplate* material = static_cast<MaterialTemplate*>(
			saver.GetRequestPtr()->GetStreamedObject());
	MaterialAsset* asset = material->GetMaterial();
	MaterialHeader header;

	header.version = NEX_MAKE_VERSION(1, 0, 0);
	header.layerMask = (uint8)material->GetLayer();
	header.options = material->GetOptions();
	asset->GetShader()->GetID(header.shader);
	header.shaderLocation = asset->GetShader()->GetAssetLocator();

	ChunkOutputStream stream(out);

	{
	OutputSerializer& ser = stream.BeginChunk(MATERIAL_HEADER);
	ser << header.version
		<< header.layerMask << header.shader
		<< header.shaderLocation << header.options;
	stream.EndChunk();
	}

	{
	OutputSerializer& ser = stream.BeginChunk(MATERIAL_PARAMETERS);
	asset->GetParameters()->Save(ser);
	stream.EndChunk();
	}
}

} /* namespace nextar */
