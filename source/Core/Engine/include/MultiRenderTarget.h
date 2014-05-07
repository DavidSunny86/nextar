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

	class MultiRenderTarget: public nextar::RenderTarget,
		public nextar::ContextObject,
		public Referenced<MultiRenderTarget, AllocGraphics>{
	public:

		enum {
			MAX_TARGET = 4
		};

		typedef array<RenderTarget*,MAX_TARGET>::type ColorAttachmentArray;

		struct TargetParam {
			bool useAsTexture;
			PixelFormat format;
			RenderTarget* useTarget;
		};

		typedef array<TargetParam,MAX_TARGET>::type TargetParamArray;

		struct UpdateParam {
			enum {
				UPDATE_DIMENSIONS = 1 << 0,
				UPDATE_ALL = 1 << 1,
				INCLUDE_DEPTH = 1 << 2,
			};
			uint16 flags;
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

		virtual void Reset();
		/* throws INVALID_CALL */
		virtual PixelFormat GetPixelFormat() const;
		/* throws INVALID_CALL */
		virtual void Capture(RenderContext* rc, PixelBox& image, FrameBuffer);

		virtual Size GetDimensions() const;
		virtual void NotifyUpdated(ContextObject::UpdateParamPtr) override;

		virtual void Update(nextar::RenderContext*, ContextObject::UpdateParamPtr);

		RenderTarget* GetAttachment(uint16 index);
		RenderTarget* GetDepthAttachment();

	protected:

		virtual void UpdateImpl(RenderContext*, UpdateParam*) = 0;

		uint16 flags;
		uint16 numColorTargets;
		Size dimensions;
		ColorAttachmentArray color;
		RenderTarget* depth;
	};

} /* namespace nextar */
#endif /* MULTIRENDERTARGET_H_ */
