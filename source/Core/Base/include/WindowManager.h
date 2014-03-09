/* 
 * File:   WinManager.h
 * Author: obhi
 *
 * Created on February 13, 2011, 2:03 AM
 */

#ifndef NEXTAR_WINMANAGER_H
#define	NEXTAR_WINMANAGER_H

#include <NexBase.h>

namespace nextar {

	class _NexBaseExport WindowManager: public Singleton<WindowManager>,
	public AllocGeneral {


	public:

		WindowManager();
		~WindowManager();

		void RegisterWindow(RenderWindow*);
		void UnregisterWindow(RenderWindow*);

		/* Returns false to quit */
		void ProcessMessages();
	protected:

		void _Unregister(RenderWindow*);

		bool processing;
		typedef list<RenderWindow*>::type WindowList;

		WindowList removedWindows;
		WindowList registeredWindows;
	};
}
#endif	/* WINMANAGER_H */

