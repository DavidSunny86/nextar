
//@ Abhishek Dey
//@ September 01 2007

#ifndef ISYSTEM_H_NEXTAR_1_01
#define ISYSTEM_H_NEXTAR_1_01

#include <iobject.h>
#include <cstr.h>
#include <clog.h>

namespace nextar
{

class X_Api iSystem : public iObject
{
	SharedSingleInterface(iSystem);

public:
		
	//@ returns the current version of the engine
	_n_inline const cStr& getVersion() const { return mkVersion; } 
	//@ returns the home directory of the exe
	_n_inline const cStr& getHomeDir() const { return mkHomeDir; } 
	//@ returns the os name
	_n_inline const cStr& getOsName()  const { return mkOs; }		 
	//@ "/" for UNIX and "\" for Windows
	_n_inline const s32	getFileSeparator() const { return miFileSeparator; } 
	//@ ":" for UNIX and "\" for Windows
	_n_inline const s32	getPathSeparator() const { return miPathSeparator; } 
	//@ get the logger instance
	_n_inline cLog& getLogger() { return mkLogger; }
	//@ return the actual path of the file from nextar format
	void getRealFilePath(tchar* inout) const;
	//@ return the file path in nextar format from actual one
	void getNextarFilePath(tchar* inout) const;
	//@ added for console handling
	virtual void cmd(const tchar* cmdStr) = 0; 
	//@ displays a msg box
	virtual s32 msgBox(const tchar* msg,const tchar* caption) = 0; 
	//@ copies text to clipboard
	virtual void copyToClipboard(const tchar* msg,s32 count=-1) = 0; 
	//@ returns the clipboard content
	virtual const tchar* pasteFromClipboard() const = 0; 
	//@ returns true if the window is active
	virtual bool isCurrentlyActiveWnd() const = 0;
	//@ sets the window caption
	virtual void setWndCaption(const tchar*) = 0;
	//@ run returns true if no exit cmd was issued or no error occured
	virtual bool run() = 0;
	//@ call the frame loop
	virtual void mainLoop() = 0;
	//@ close the device
	virtual void exit() = 0;
	//@ create the system
	static iSystem* newSystem(const tchar* config);
	//@ ctor
	iSystem();
	
protected:
	
	virtual ~iSystem();

protected:
	cLog	mkLogger;
	cStr	mkVersion;
	cStr	mkHomeDir;
	cStr	mkOs;
	s32		miFileSeparator;
	s32		miPathSeparator;
};

#define iSystemLog X_SharedPtr(iSystem)->getLogger().print

}

#endif //ISYSTEM_H_NEXTAR_1_01
