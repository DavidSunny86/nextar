#ifndef NEXTAR_VIEWPORT_H
#define NEXTAR_VIEWPORT_H

#include <NexEngine.h>
#include <Color.h>
#include <CommitContext.h>
#include <SceneTraversal.h>
#include <VisibilitySet.h>
#include <RenderPass.h>
#include <LightSystem.h>

namespace nextar {
class ViewportCallback;
class Viewport;
typedef list<Viewport*>::type ViewportList;

/**
 * @class ViewportCallback
 *
 * @remarks Interface to receive viewport messages.
 **/
class _NexNoVtable ViewportCallback {
public:
	virtual void PreUpdate(Viewport*) = 0;
	virtual void PostUpdate(Viewport*) = 0;
protected:
	~ViewportCallback() {}
};

/**
 * @class Viewport
 *
 * @remarks Viewport is the portion of a canvas where a camera can draw.
 *        A viewport is created inside a canvas.
 **/
class Viewport: 
	public NamedObject,
	public AllocGeneral {
public:

	enum ClearMask {
		CLEAR_COLOR_BUFFER = 1 << 0,
		CLEAR_DEPTH_BUFFER = 1 << 1,
		CLEAR_STENCIL_BUFFER = 1 << 2,
		CLEAR_ALL = 0xff
	};
	// flags
	enum {
		ACTIVE = 1 << 0, DIRTY = 1 << 1, LASTFLAG = 1 << 2
	};
	/** todo We can use a single callback and multiplex if necessary */
	typedef vector<ViewportCallback*>::type ViewportCallbackList;

	class Iterator {
		Viewport* viewPort;

	public:
		Iterator() :
				viewPort(0) {

		}

		Iterator(Viewport* vp) :
				viewPort(vp) {

		}

		Viewport* operator *() {
			return viewPort;
		}

		inline operator bool() {
			return viewPort != 0;
		}

		Iterator operator ++() {
			viewPort = viewPort->_GetNext();
			return *this;
		}

		friend inline bool operator ==(const Iterator it1, const Iterator it2) {
			return (it1.viewPort == it2.viewPort) != 0;
		}
	};

	// Compare
	inline static int32 Compare(const Viewport *vp1, const Viewport *vp2) {
		return vp1->priority - vp2->priority;
	}

	Viewport(Camera *cam, RenderTarget *rt, const String& renderSystem = StringUtils::Default,
		float x = 0.f, float y = 0.f,
		float width = 1.f, float height = 1.f, int32 priority = 0,
		uint32 flags = ACTIVE, StringID optName = StringUtils::NullID,
		Viewport* next = 0);

	virtual ~Viewport();

	inline int32 GetPriority() const {
		return priority;
	}

	inline float GetTopLeftX() const {
		return topleftX;
	}

	inline float GetTopLeftY() const {
		return topleftY;
	}

	inline float GetWidth() const {
		return width;
	}

	inline float GetHeight() const {
		return height;
	}

	inline RenderTarget *GetRenderTarget() const {
		return renderTarget;
	}

	/** @brief	Gets the camera.  */
	inline Camera *GetCamera() {
		return camera;
	}

	/** @brief	Gets the camera.  */
	inline const Camera *GetCamera() const {
		return camera;
	}

	/* @remarks returns if this viewport is enabled. */
	inline bool IsEnabled() const {
		return (flags & ACTIVE) != 0;
	}

	void SetCamera(Camera* cam) {
		camera = cam;
	}

	void SetClearDepth(float clearDepth) {
		this->clearDepth = clearDepth;
	}

	float GetClearDepth() const {
		return clearDepth;
	}

	void SetClearColor(Color clearColor) {
		this->clearColor = clearColor;
	}

	Color GetClearColor() const {
		return clearColor;
	}

	void SetClearMask(uint8 clearMask) {
		this->clearMask = clearMask;
	}

	uint8 GetClearMask() const {
		return clearMask;
	}

	Size GetViewportPixelDimensions() const {
		return viewportSizeInPixels;
	}

	void SetRenderSystem(RenderSystemPtr rsys) {
		renderSystem = rsys;
	}

	RenderSystemPtr GetRenderSystem() const {
		return renderSystem;
	}

	/* Update viewport */
	virtual void Render(RenderContext* renderCtx, const FrameTimer&);

	/* @remarks Add new callback */
	virtual void AddCallback(ViewportCallback*);
	/* @remarks Remove callback */
	virtual void RemoveCallback(ViewportCallback*);
	/* @remarks Fires preupdate call for callbacks */
	virtual void FirePreupdate();
	/* @remarks Fires postupdate call for callbacks */
	virtual void FirePostupdate();

	/* */
	virtual void PushPrimitives(const FrameTimer& frameTimer);
	virtual void CommitPrimitives(RenderContext* renderCtx, const FrameTimer& frameNumber);

protected:

	void _SetNext(Viewport *next);
	inline Viewport *_GetNext() const {
		return next;
	}

	LightSystemPtr lightSystem;
	SceneTraversal traversal;

	Size viewportSizeInPixels;
	Size lastTargetSize;

	float topleftX;
	float topleftY;
	float width;
	float height;

	/* clear flags */
	uint8 clearMask;
	Color clearColor;
	float clearDepth;
	uint32 clearStencil;

	/* the more this value, the quicker it is displayed, and hence
	 beneath lesser valued viewports */
	int32 priority;
	uint32 flags;
	String name;
	Camera *camera;
	RenderTarget *renderTarget;

	CommitContext commitContext;
	ViewportList offscreen;

	VisibilitySet visibleSet;
	ViewportCallbackList callbacks;
	RenderSystemPtr renderSystem;
private:

	/* next viewport in the list */
	Viewport *next;

};

typedef Viewport *ViewportPtr;
}

#endif //NEXTAR_VIEWPORT_H