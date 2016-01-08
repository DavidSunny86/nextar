/* 
 * File:   WinManager.h
 * Author: obhi
 *
 * Created on February 13, 2011, 2:03 AM
 */

#ifndef NEXTAR_WINMANAGER_H
#define	NEXTAR_WINMANAGER_H

#include <NexBase.h>
#include <EventDispatcher.h>

namespace nextar {

class _NexBaseAPI WindowManager: public Singleton<WindowManager>,
	public EventDispatcher,
	public AllocGeneral {

public:
	enum EventID {
		EVENT_ACTIVE_WINDOW_CHANGED,
	};

	WindowManager();
	~WindowManager();

	void RegisterWindow(RenderWindow*);
	void UnregisterWindow(RenderWindow*);
	
	RenderWindow* GetActiveWindow() {
		return activeWindow;
	}

	void ActivateWindow(RenderWindow* rw);
	void DeactivateWindow(RenderWindow* rw);
	/* Returns false to quit */
	void ProcessMessages();
	void Quit();
protected:

	void _Unregister(RenderWindow*);
	void _ProcessRemovedItems();

	bool processing;
	typedef list<RenderWindow*>::type WindowList;

	RenderWindow* activeWindow;
	WindowList removedWindows;
	WindowList registeredWindows;
};

}
#endif	/* WINMANAGER_H */

