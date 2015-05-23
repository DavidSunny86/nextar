#include <BaseHeaders.h>
#include <RenderWindow.h>
#include <WindowManager.h>
#include <ApplicationContext.h>

namespace nextar {

RenderWindow::RenderWindow(Impl* _impl) :
		flags(WINDOW_CLOSED), impl(_impl) {
}

RenderWindow::~RenderWindow() {
}

void RenderWindow::Create(uint32 width, uint32 height, bool fullscreen,
		const NameValueMap* params) {
	if (flags & WINDOW_CREATED)
		return;
	impl->Create(width, height, fullscreen, params);
	WindowManager::Instance().RegisterWindow(this);
	WinEventListnerList::iterator it = listeners.begin();
	WinEventListnerList::iterator end = listeners.end();
	while (it != end) {
		(*it)->WindowCreated(this);
		++it;
	}
	flags |= WINDOW_CREATED;
	Trace("Window Created: " + windowTitle);
}

void RenderWindow::Destroy() {
	if (!(flags & WINDOW_CREATED))
		return;

	WinEventListnerList::iterator it = listeners.begin();
	WinEventListnerList::iterator end = listeners.end();

	while (it != end) {
		(*it)->WindowDestroying(this);
		++it;
	}

	if (IsMainWindow()) {
		// we will post quit message, so release all resources
		ApplicationContext::Instance().QuitApplication();
	}

	WindowManager::Instance().UnregisterWindow(this);
	impl->Destroy();
	
	flags &= ~WINDOW_CREATED;
	flags |= WINDOW_CLOSED;
	Trace("Window Destroyed: " + windowTitle);

}

void RenderWindow::AddListener(RenderWindow::EventListener* l) {
	listeners.push_back(l);
}

void RenderWindow::RemoveListener(RenderWindow::EventListener* l) {
	listeners.remove(l);
}

void RenderWindow::WindowConfigChanged() {
	WinEventListnerList::iterator it = listeners.begin();
	WinEventListnerList::iterator end = listeners.end();
	while (it != end) {
		(*it)->WindowConfigChanged(this);
		++it;
	}
	impl->ApplyChangedAttributes();
}

void RenderWindow::WindowFocusChanged() {
	WinEventListnerList::iterator it = listeners.begin();
	WinEventListnerList::iterator end = listeners.end();
	while (it != end) {
		(*it)->WindowFocusChanged(this);
		++it;
	}
	impl->ApplyChangedAttributes();
}

void RenderWindow::SetActive(bool act) {
	flags = act ? flags | WINDOW_ACTIVE : flags & ~WINDOW_ACTIVE;
}

void RenderWindow::SetVisible(bool val) {
	flags = val ? flags | WINDOW_VISIBLE : flags & ~WINDOW_VISIBLE;
}

void RenderWindow::SetToFullScreen(bool full) {
	impl->SetToFullScreen(full);
}
}
