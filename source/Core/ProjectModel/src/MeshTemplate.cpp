/*
 * MeshTemplate.cpp
 *
 *  Created on: 03-Oct-2014
 *      Author: obhi
 */
#include <NexProjectModel.h>
#include <MeshTemplate.h>

namespace nextar {

MeshTemplate::MeshTemplate(const StringID name, const StringID factory) : AssetTemplate(name, factory),
		sharedBuffer(nullptr) {
}

MeshTemplate::~MeshTemplate() {
}

StreamNotification MeshTemplate::NotifyAssetLoadedImpl(
		StreamRequest* request) {
	MeshBuilder* builder = static_cast<StreamRequest*>(request);

	sharedMaterial = builder->sharedMaterial;
	sharedBuffer = builder->sharedBuffer;
	bounds = builder->bounds;
	primitives = std::move(builder->primitives);

	return StreamNotification::NOTIFY_COMPLETED_AND_READY;
}

}



