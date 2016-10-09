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
	public AllocGraphics {
public:
	enum Flags {
		PASS_ENABLED = 1 << 0,
		LAST_FLAG = 1 << 1,
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

		
	virtual void Commit(CommitContext& context) = 0;

protected:

	uint8 flags;

};

typedef vector<RenderPass*>::type RenderPassList;

} /* namespace nextar */
#endif /* RENDERPASS_H_ */
