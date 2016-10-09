/*
 * AssetTemplate.cpp
 *
 *  Created on: 19-Jul-2014
 *      Author: obhi
 */
#include <NexProjectModel.h>
#include <AssetTemplate.h>

namespace nextar {

AssetTemplate::AssetTemplate(const StringID name, const StringID factory) :
		Asset(name, factory),
		lastSavedModification(0),
		modification(-1) {
}

AssetTemplate::~AssetTemplate() {
}

URL AssetTemplate::GetProxyAssetLocator() const {
	const URL& url = GetAssetLocator();
	String relativePath = url.GetRelativePath();
	StringUtils::Replace(relativePath, "Scripts/", "");
	size_t p = relativePath.find_last_of('.');
	if (p != String::npos) {
		relativePath.replace(p + 1, String::npos, GetPoxyAssetExtension());
	} else {
		relativePath += '.';
		relativePath += GetPoxyAssetExtension();
	}
	return URL(url.GetArchiveName(), relativePath);
}

} /* namespace nextar */
