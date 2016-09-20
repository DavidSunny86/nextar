/*
 * ResourcePackerServiceImpl.h
 *
 *  Created on: Sep 18, 2016
 *      Author: obhi
 */

#ifndef PLUGINS_RESOURCEPACKER_LOCAL_INCLUDE_RESOURCEPACKERSERVICEIMPL_H_
#define PLUGINS_RESOURCEPACKER_LOCAL_INCLUDE_RESOURCEPACKERSERVICEIMPL_H_

#include <ResourcePacker.h>
#include <ResourcePackerService.h>

namespace ResourcePacker {

class ResourcePackerServiceImpl: public nextar::ResourcePackerService {
public:
	ResourcePackerServiceImpl();
	virtual ~ResourcePackerServiceImpl();

	inline void IncrementUsage() {
		refNumber++;
	}

	inline void DecrementUsage() {
		refNumber--;
	}

	inline bool IsInUse() const {
		return refNumber != 0;
	}

	virtual void BuildMaterials(ProjectContextPtr const &context);
	virtual void BuildRenderScripts(ProjectContextPtr const &context);
	virtual void BuildProject(ProjectContextPtr const &context);
	virtual void Destroy() override;

	class MaterialLoad : public Archive::ScanCallback {
	public:
		NEX_LOG_HELPER(MaterialLoad);
		/** @brief Callback, called when scanning dir. **/
		virtual void FoundFile(const FileAttribute& attribute, Archive*);
	};

	class RenderScriptLoad : public Archive::ScanCallback {
	public:
		NEX_LOG_HELPER(RenderScriptLoad);
		/** @brief Callback, called when scanning dir. **/
		virtual void FoundFile(const FileAttribute& attribute, Archive*);
	};

protected:

	int refNumber;
};

} /* namespace ResourcePacker */

#endif /* PLUGINS_RESOURCEPACKER_LOCAL_INCLUDE_RESOURCEPACKERSERVICEIMPL_H_ */
