/*
 * AssetTemplate.cpp
 *
 *  Created on: 19-Jul-2014
 *      Author: obhi
 */
#include <NexProjectModel.h>
#include <AssetTemplate.h>

namespace nextar {

AssetTemplate::AssetTemplate(const StringID name, const StringID factory) : Asset(name, factory) {
}

AssetTemplate::~AssetTemplate() {
}

} /* namespace nextar */
