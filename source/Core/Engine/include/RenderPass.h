/*
 * RenderPass.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef RENDERPASS_H_
#define RENDERPASS_H_

#include <NexEngine.h>
#include <VisibilitySet.h>

namespace nextar {

class _NexEngineAPI RenderPass: 
	public NamedObject,
	public AllocGeneral {
public:
	enum Flags {
		PASS_ENABLED = 1 << 0,
		PASS_OVERRIDES_MATERIAL = 1 << 1,
		LAST_FLAG = 1 << 2,
	};

	struct Info {
		uint32 flags;
		// a render pass might have a tag, that will be checked with effects
		// to see if an effect should be compiled with it.
		hash_t tag;
		const StringUtils::WordList* _options;
	};
	typedef RenderPass* (*CreateInstance) ();

	RenderPass();
	virtual ~RenderPass();


	bool IsEnabled() const {
		return (flags & PASS_ENABLED) != 0;
	}

	void SetEnabled(bool b) {
		if (b)
			flags |= PASS_ENABLED;
		else
			flags &= ~PASS_ENABLED;
	}

	virtual Info GetPassInfo() = 0;
	virtual void Commit(CommitContext& context) = 0;
	
protected:

	uint32 flags;

};

typedef vector<RenderPass*>::type RenderPassList;

} /* namespace nextar */
#endif /* RENDERPASS_H_ */
