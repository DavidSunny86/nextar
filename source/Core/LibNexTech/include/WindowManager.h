/* 
 * File:   WinManager.h
 * Author: obhi
 *
 * Created on February 13, 2011, 2:03 AM
 */

#ifndef NEXTAR_WINMANAGER_H
#define	NEXTAR_WINMANAGER_H

#include "NexSys.h"

namespace nextar {

	class _NexExport WindowManager: public Singleton<WindowManager>,
	public AllocGeneral {


	public:

		WindowManager();
		~WindowManager();

		void RegisterWindow(RenderWindowPtr&);
		void UnregisterWindow(RenderWindowPtr&);

		/* Returns false to quit */
		void ProcessMessages();
	protected:

		void _Unregister(RenderWindowPtr&);

		bool processing;
		typedef list<RenderWindowPtr>::type WindowList;

		WindowList removedWindows;
		WindowList registeredWindows;
	};
}
#endif	/* WINMANAGER_H */

