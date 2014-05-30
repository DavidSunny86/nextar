/*
 * MultiRenderTarget.h
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */

#ifndef MULTIRENDERTARGET_H_
#define MULTIRENDERTARGET_H_

#include <ContextObject.h>
#include <RenderTarget.h>

namespace nextar {

	class _NexEngineAPI MultiRenderTarget: public nextar::RenderTarget,
		public nextar::ContextObject {
	public:

		enum {
			CO_TYPE = TYPE_MULTI_RENDER_TARGET,
			MAX_TARGET = 4
		};

		enum UpdateMessage {
			MSG_RT_CREATE = 1 << 1,
			MSG_RT_RESIZE = 1 << 2,
		};

		typedef array<RenderTarget*,MAX_TARGET>::type ColorAttachmentArray;

		struct TargetParam {
			bool useAsTexture;
			PixelFormat format;
			RenderTarget* useTarget;
		};

		typedef array<TargetParam,MAX_TARGET>::type TargetParamArray;

		struct CreateParam {
			bool useDepth;
			uint16 numColorTargets;
			Size dimensions;
			TargetParamArray targets;
			TargetParam depth;
		};

		MultiRenderTarget();
		virtual ~MultiRenderTarget();

		bool IsDepthBufferValid() const {
			return depth != nullptr;
		}

		/* throws INVALID_CALL */
		virtual PixelFormat GetPixelFormat() const;
		/* throws INVALID_CALL */
		virtual void Capture(RenderContext* rc, PixelBox& image, FrameBuffer);
		virtual Size GetDimensions() const;

		virtual void Create(const CreateParam& params);

		RenderTarget* GetAttachment(uint16 index);
		RenderTarget* GetDepthAttachment();

	protected:

		RenderTarget* CreateColorTexture(const TargetParam& tp);

		uint16 flags;
		uint16 numColorTargets;
		Size dimensions;
		ColorAttachmentArray color;
		RenderTarget* depth;
	};

} /* namespace nextar */
#endif /* MULTIRENDERTARGET_H_ */
