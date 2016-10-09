/*
 * MeshTemplate.cpp
 *
 *  Created on: 03-Oct-2014
 *      Author: obhi
 */
#include <NexProjectModel.h>
#include <MeshTemplate.h>
#include <MeshAsset.h>

namespace nextar {

MeshTemplate::MeshTemplate(const StringID name, const StringID factory) : AssetTemplate(name, factory),
		sharedBuffer(nullptr) {
}

MeshTemplate::~MeshTemplate() {
	Unload();
}

StreamNotification MeshTemplate::NotifyAssetLoadedImpl(
		StreamRequest* request) {
	MeshBuilder* builder = static_cast<MeshBuilder*>(request);

	sharedMaterial = builder->sharedMaterial;
	std::swap(sharedBuffer, builder->sharedBuffer);
	bounds = builder->bounds;
	primitives = std::move(builder->primitives);

	return StreamNotification::NOTIFY_COMPLETED_AND_READY;
}

uint32 MeshTemplate::GetClassID() const {
	return CLASS_ID;
}

uint32 MeshTemplate::GetProxyID() const {
	return MeshAsset::CLASS_ID;
}

String MeshTemplate::GetPoxyAssetExtension() const {
	return "nexmesh";
}

StreamRequest* MeshTemplate::CreateStreamRequestImpl(bool load) {
	if (load)
		return NEX_NEW(MeshBuilder(this));
	else
		return NEX_NEW(AssetStreamRequest(this, StreamRequest::REQUEST_SAVE|
				StreamRequest::AUTO_DELETE_REQUEST));
}

void MeshTemplate::UnloadImpl() {
	if (sharedBuffer)
		NEX_DELETE(sharedBuffer);

	sharedMaterial.Clear();
	primitives.clear();
}

/**************************************
 * PrimitiveGroup
 **************************************/

MeshTemplate::PrimitiveGroup::~PrimitiveGroup() {
	if (buffer)
		NEX_DELETE(buffer);
}

}



