/*
 * PassScriptStreamerImpl.h
 *
 *  Created on: 07-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_PASSSCRIPTSTREAMER_LOCAL_INCLUDE_RENDERSCRIPTSTREAMER_H_
#define PLUGINS_PASSSCRIPTSTREAMER_LOCAL_INCLUDE_RENDERSCRIPTSTREAMER_H_

#include <RenderSystemScript.h>
#include <RenderScriptRoot.h>

namespace RenderSystemScript {

using namespace nextar;

class RenderScriptStreamer :
		public nextar::RenderSystem::Streamer {
	NEX_LOG_HELPER(RenderScriptStreamer);
public:

	RenderScriptStreamer();
	virtual ~RenderScriptStreamer();

	void RegisterDictionary();
	void UnregisterDictionary();

	virtual bool Load(RenderSystem& s, InputStreamPtr& stream);
	virtual bool Save(RenderSystem& s, OutputStreamPtr& stream);

protected:
	RenderScriptRoot _root;

};

} /* namespace PassScriptStreamer */

#endif /* PLUGINS_PASSSCRIPTSTREAMER_LOCAL_INCLUDE_RENDERSCRIPTSTREAMER_H_ */
