/*
 * RenderSystemImpl.h
 *
 *  Created on: 06-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_RENDERENGINE_LOCAL_INCLUDE_RENDERSYSTEMIMPL_H_
#define CORE_RENDERENGINE_LOCAL_INCLUDE_RENDERSYSTEMIMPL_H_

#include <RenderSystem.h>

namespace nextar {

class RenderSystemImpl: public RenderSystem {
public:
	RenderSystemImpl();
	virtual ~RenderSystemImpl();

	virtual void RegisterTarget(StringID as, RenderTargetPtr target) override;
	virtual RenderTargetPtr GetTarget(StringID name) override;

	virtual void Load(InputStreamPtr& stream, const String& fileType);
	virtual void Save(OutputStreamPtr& stream, const String& fileType);

protected:

	typedef map<StringID, RenderTargetPtr>::type TargetMap;

	TargetMap targets;
};

} /* namespace nextar */

#endif /* CORE_RENDERENGINE_LOCAL_INCLUDE_RENDERSYSTEMIMPL_H_ */
