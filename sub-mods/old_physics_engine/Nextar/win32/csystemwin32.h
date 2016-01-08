//@ Abhishek Dey
//@ September 03 2007
//@ September 17 2007

#ifndef WIN32_SYSTEM_NEXTAR_1_01
#define WIN32_SYSTEM_NEXTAR_1_01

#include <isystem.h>
#include <irenderer.h>
#include <cinputsystem.h>

namespace nextar
{
class iXMLDocument;
class cSystemWin32 : public iSystem
{
	//@ members
	enum Flags { WFullScreen = 1, WPaused = 2 };
	u32		muFlags;
	HWND	mkWnd;
	cStr	mkConfigFile;
	cStr	mkResourceDir;

private:

	void					_getOsVersion();
	void					_getCurrentDirectory(cStr&);
	void					_defaultConfiguration();
	bool					_loadConfiguration(iXMLDocument*);
	void					_saveConfiguration(const tchar* sNexFile);
	void					_createFileSystem();
	void					_createRenderer(RENDERDEVICE,s32,s32,u32,u32,const tchar*,bool);
	void					_createManagers();
	void					_createLoaders();
	void					_createInputSystem(INPUTDEVICES);

public:

	//@ internal
	void	pauseSystem();
	void	resumeSystem();
	void	resize();

	//@ overrides
	virtual void			cmd(const tchar* cmdStr); 
	virtual s32				msgBox(const tchar* msg,const tchar* caption); 
	virtual void			copyToClipboard(const tchar* msg,s32); 
	virtual const tchar*	pasteFromClipboard() const; 
	virtual bool			isCurrentlyActiveWnd() const;
	virtual void			setWndCaption(const tchar*);
	virtual bool			run();
	virtual void			mainLoop();
	virtual void			exit();

	cSystemWin32(const tchar*);
	virtual ~cSystemWin32(void);
};

}

#endif