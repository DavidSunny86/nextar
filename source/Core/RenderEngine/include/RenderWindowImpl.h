/*
 * RenderWindowImpl.h
 *
 *  Created on: 24-Mar-2014
 *      Author: obhi
 */

#ifndef RENDERWINDOWIMPL_H_
#define RENDERWINDOWIMPL_H_

#include <NexRenderEngine.h>
#include <RenderWindow.h>
#include <RenderTarget.h>

namespace nextar {

/*
 *
 */
class _NexRenderAPI RenderWindowImpl: public RenderTarget,
		public RenderWindow::Impl {
public:
	RenderWindowImpl();
	virtual ~RenderWindowImpl();

	// RenderWindow::Impl
	virtual void SetToFullScreen(bool fullscreen) override = 0;
	virtual void Destroy() override = 0;
	virtual void Create(uint32 width, uint32 height, bool fullscreen,
			const NameValueMap* params) override = 0;
	virtual void ApplyChangedAttributes() override = 0;
	virtual void FocusChanged() override = 0;
	virtual Size GetCurrentDimensions() const override;

	// RenderTarget
	virtual Size GetDimensions() const;
	virtual PixelFormat GetPixelFormat() const override = 0;
	virtual void Capture(RenderContext* rc, PixelBox& image, FrameBuffer) = 0;
	virtual void Present(RenderContext* rc) = 0;

protected:
	Size dimensions;
};

} /* namespace nextar */

#endif /* RENDERWINDOWIMPL_H_ */
