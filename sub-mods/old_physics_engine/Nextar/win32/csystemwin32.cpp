//@ Abhishek Dey
//@ September 03 2007
//@ September 17 2007

#include <csystemwin32.h>
#define WIN32_LEAN_AND_MEAN
#include <windowsx.h>

#include <ixmldocument.h>
#include <cfilesystem.h>
#include <cinputsystem.h>
#include "win32/directx/cdirectx9inputdevice.h"
#include "win32/directx/cdirectx9renderer.h"
#include "win32/directx/cdirectx9materialfactory.h"
#include <loaders/cTextureFactory.h>
#include <ctimer.h>
#include <cengine.h>
#include <cxuimanager.h>
#include "physics/csimulator.h"

#define NEXTAR_W32_ICON_INFORMATION 0

using namespace nextar;

const tchar NEXTAR_WND_CLASS[] = TEXT("Nextar");
const tchar NEXTAR_WND_TITLE[] = TEXT("Nextar v"X_VERSION_STRING);
#define NEXTAR_W32_ICON_INFORMATION 0

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//@ internal function to display the memory status on a console
void _DisplayMemoryStatus()
{
	MEMORYSTATUS MemStatus;
	GlobalMemoryStatus(&MemStatus);
	_tprintf(TEXT("\nCurrent System Memory Info {\nPercent Usage = %u\nPhysical Memory Satatus = %u\nAvailable Physical Memory Satatus = %u\nTotal Bytes of Page File = %u\nAvailable Bytes of Page File = %u\nTotal Virtual Memory = %u\nAvailable Virtual Memory = %u\n}\n"),
			MemStatus.dwMemoryLoad,
			MemStatus.dwTotalPhys,
			MemStatus.dwAvailPhys,
			MemStatus.dwTotalPageFile,
			MemStatus.dwAvailPageFile,
			MemStatus.dwTotalVirtual,
			MemStatus.dwAvailVirtual 
			);
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// WndProc
LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	X_Assert( X_SharedPtr(iSystem) );
    switch( msg )
    {
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
			}
			return 0;
		case WM_ERASEBKGND:
			return 0;
		case WM_ACTIVATE:		
			// Add pause code here.
			if( WA_INACTIVE == LOWORD(wParam) )
				((cSystemWin32*)X_SharedPtr(cSystemWin32))->pauseSystem();
			else
				((cSystemWin32*)X_SharedPtr(cSystemWin32))->resumeSystem();
			break;
        case WM_DESTROY:
			// Add cleanup code here.
            PostQuitMessage( 0 );
            return 0;
		case WM_SIZE:
			// Ask Engine to Resize
			((cSystemWin32*)X_SharedPtr(cSystemWin32))->resize();
			break;
		case WM_SYSCOMMAND:
			// prevent screensaver or monitor powersave mode from starting
			if ( wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER )
				return 0;
			break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//@ ctor
cSystemWin32::cSystemWin32(const tchar* sConfig) : muFlags(0)
{
	RefObjectInit(cSystemWin32);
	_DisplayMemoryStatus();
	_getOsVersion();
	_getCurrentDirectory(mkHomeDir);
	miFileSeparator = '\\';
	miPathSeparator = '\\';
	mkConfigFile = TEXT("config.xml");
	// now to read the config file and create
	// a default file system.
	bool bLoad = false;
	if(sConfig)
	{
		mkConfigFile = sConfig;
		getRealFilePath(mkConfigFile);
	
		iXMLDocument* pkConf = iXMLDocument::create(mkConfigFile);
		if(pkConf)
		{
			if(pkConf->load() == X_SUCCESS && pkConf->isValidNextarDocument())
				if(_loadConfiguration(pkConf))
					bLoad = true;
			pkConf->release();
		}
	}

	if(!bLoad)
	{
		// this must be the resource dir
		mkResourceDir = sConfig;
		_defaultConfiguration();
	}

	cTimer::skAsynchTimer.start();
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// exit
cSystemWin32::~cSystemWin32(void)
{
	exit();
	_DisplayMemoryStatus();
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// exit
void cSystemWin32::exit()
{
#define doSafeRelease(x) if(X_SharedPtr(x)) { X_SharedPtr(x)->release(); }

	doSafeRelease(cSimulator);
	doSafeRelease(cEngine);
	doSafeRelease(cXuiManager);
	doSafeRelease(cInputSystem);
	doSafeRelease(iMaterialFactory);
	doSafeRelease(iTextureFactory);
	doSafeRelease(iRenderer);
	doSafeRelease(cFileSystem);

	if(mkWnd) 
	{ 
		DestroyWindow(mkWnd); 
		mkWnd=0; 
		UnregisterClass( NEXTAR_WND_CLASS, GetModuleHandle(0) ); 
	}

#undef doSafeRelease
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// pause system
void cSystemWin32::pauseSystem()
{
	muFlags |= WPaused;
	if(X_SharedPtr(cInputSystem))
		X_SharedPtr(cInputSystem)->deactivate();
	cTimer::skAsynchTimer.stop();
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// unpause system
void cSystemWin32::resumeSystem()
{
	muFlags &=~WPaused;
	if(X_SharedPtr(cInputSystem))
		X_SharedPtr(cInputSystem)->activate();
	cTimer::skAsynchTimer.start();
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// resize
void cSystemWin32::resize()
{
	if(muFlags & WPaused || muFlags & WFullScreen) return;
	RECT kR = {0};
	GetClientRect(mkWnd,&kR);
	if(kR.right <= 1 ||  kR.bottom <= 1)
		mkLogger.print(LL_INFO,"Ignoring resize operation!");
	else
	{
		mkLogger.print(LL_INFO,TEXT("Resizing window"));
		if(kR.right%2) kR.right++;
		if(kR.bottom%2) kR.bottom++;
		if(X_SharedPtr(iRenderer))
			X_SharedPtr(iRenderer)->setScreenSize(size2_x((i2d)(kR.right-kR.left),(i2d)(kR.bottom-kR.top)));
	}
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// cmd line 
void cSystemWin32::cmd(const tchar* cmdStr)
{
	// do nothing until console is implemented
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// msg box
s32 cSystemWin32::msgBox(const tchar* msg,const tchar* caption)
{
	if(IDOK==MessageBox(GetDesktopWindow(),msg,caption,MB_OKCANCEL))
		return 0;
	return -1;
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// cpy to clipbrd
void cSystemWin32::copyToClipboard(const tchar* str,s32 c)
{
	if(!str) return;
	if(!*str) return;
	if(!OpenClipboard(0)) return;
	EmptyClipboard();

	HGLOBAL clipb;
	tchar*	buffer;

	if(c<=0) c = lstrlen(str)+1;
	else c++;
	clipb = GlobalAlloc(GMEM_DDESHARE, sizeof(tchar)*(c));
	buffer = (tchar*)GlobalLock(clipb);

	lstrcpy(buffer, str);

	GlobalUnlock(clipb);
	SetClipboardData(CF_TEXT, clipb);
	CloseClipboard();
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// paste
const tchar* cSystemWin32::pasteFromClipboard() const
{
	if (!OpenClipboard(0))
		return 0;
	
	tchar * buffer = 0;
	HANDLE h = GetClipboardData( CF_TEXT );
	if(!h) return 0;

	buffer = (tchar*)GlobalLock( h );
	
	GlobalUnlock( h );
	CloseClipboard();

	return buffer;
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// currently active wnd
bool cSystemWin32::isCurrentlyActiveWnd() const
{
	return !(muFlags & WPaused);
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// set caption
void cSystemWin32::setWndCaption(const tchar* cap)
{
	SetWindowText(mkWnd,cap);
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// run
bool cSystemWin32::run()
{
	static MSG msg;

	bool quit = false;

	while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			quit = true;
	}

	return !quit;
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// run
void cSystemWin32::mainLoop()
{
	X_Assert(X_SharedPtr(cInputSystem));
	X_Assert(X_SharedPtr(iRenderer));
	if(muFlags & WPaused) return;
	cTimer::skAsynchTimer.tick();
	// we do not do a thing more
	X_SharedPtr(cEngine)->wUpdate();
	X_SharedPtr(iRenderer)->beginScene();
	X_SharedPtr(cEngine)->wDraw();
	X_SharedPtr(iRenderer)->endScene();
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// get curr directory
void cSystemWin32::_getCurrentDirectory(cStr& kStr)
{
	tchar sBuffer[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH,sBuffer);
	getNextarFilePath(sBuffer);
	kStr = sBuffer;
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// load up params
void cSystemWin32::_getOsVersion()
{
	OSVERSIONINFO kVersionInfo = {0};
	mkOs = TEXT("Windows (X)");
	
	kVersionInfo.dwOSVersionInfoSize = sizeof(kVersionInfo);

	if( GetVersionEx(&kVersionInfo) )
	{
		if(kVersionInfo.dwPlatformId==VER_PLATFORM_WIN32_NT)
		{
			switch(kVersionInfo.dwMinorVersion)
			{
			case 0:
				mkOs = TEXT("Windows 2000");
				break;
			case 1:
				mkOs = TEXT("Windows XP");
				break;
			case 2:
				mkOs = TEXT("Windows Server 2003 family");
				break;
			}
		}
	}
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// default config
void cSystemWin32::_defaultConfiguration()
{
	_createFileSystem();
	_createRenderer(RD_DIRECT3D9,
		X_DEFAULT_WIN_WIDTH,X_DEFAULT_WIN_HEIGHT,
		X_DEFAULT_BITDEPTH,X_DEFAULT_RENDER_FLAGS,
		NEXTAR_WND_TITLE,X_FULLSCRENN);
	_createLoaders();
	_createInputSystem(ID_DIRECTINPUT8);
	_createManagers();
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// load config
bool cSystemWin32::_loadConfiguration(iXMLDocument* )
{
	return false;
}
//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// save config
void cSystemWin32::_saveConfiguration(const tchar* sFile)
{
	// create the default xml
	/*
	if(sFile)
		mkConfigFile = sFile;
	iXMLDocument* pkDoc = iXMLDocument::create(mkConfigFile);
	// we will serially add elements
	iXMLElement*  pkE = 0;
	iXMLAttribute*pkA = 0;

	// file system config
	pkE = (iXMLElement*)pkDoc->createNode(iXMLNode::XML_ELEMENT);
	pkE->setValue(cStr(X_FS_EL_NAME));
	pkDoc->addChildLast(pkE);

	// window config
	pkE = (iXMLElement*)pkDoc->createNode(iXMLNode::XML_ELEMENT);
	pkE->setValue(cStr(X_WIN_EL_NAME));
	pkDoc->addChildLast(pkE);
	pkE->addAttr(X_WIN_EL_ATT1)->setAsSignedInt(X_DEFAULT_WIN_WIDTH);//TODO: Change these according to
	pkE->addAttr(X_WIN_EL_ATT2)->setAsSignedInt(X_DEFAULT_WIN_HEIGHT);//Renderer config
	pkE->addAttr(X_WIN_EL_ATT3)->setAsSignedInt(X_FULLSCRENN);

	// save
	pkDoc->save();
	pkDoc->release();
	*/
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// create the file system, and sub file systems (the zipped or archive file system)
void cSystemWin32::_createFileSystem()
{
	new cFileSystem(mkHomeDir,false);
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// create the window and renderer
void cSystemWin32::_createRenderer(RENDERDEVICE,s32 iWidth,s32 iHeight,u32 uB,u32 uF,const tchar* sCap,bool bFullScreen)
{
	// register the class
	HINSTANCE hModule = GetModuleHandle(0);
	// create a wnd class
	// Register the class first;
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_OWNDC;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hModule;
	wcex.hIcon			= NEXTAR_W32_ICON_INFORMATION;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= NEXTAR_WND_CLASS;
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);

	// get the client rect
	RECT	ClientRect	= { 0, 0, iWidth, iHeight };	
	DWORD	WinStyle	= WS_POPUP;
	
	if (!bFullScreen)
			WinStyle = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		
	AdjustWindowRect( &ClientRect, WinStyle, FALSE );

	s32 Width  = ClientRect.right - ClientRect.left;
	s32 Height = ClientRect.bottom - ClientRect.top;

	s32 WindowLeft = (GetSystemMetrics(SM_CXSCREEN) - Width) / 2;
	s32 WindowTop  = (GetSystemMetrics(SM_CYSCREEN) - Height) / 2;

	muFlags |= WFullScreen;
	if (bFullScreen)
	{
		WindowLeft = 0;
		WindowTop = 0;
	}

	// create window
	mkWnd = CreateWindow( NEXTAR_WND_CLASS, NEXTAR_WND_TITLE, WinStyle, WindowLeft, WindowTop, 
						 Width, Height, GetDesktopWindow(), 0, hModule, 0 );

	if(!mkWnd)
	{
		ReportBug("iSystem := Failed to create window. Closing App.","_createWindow");
		exit();
	}

	new cDirectX9Renderer(mkWnd,size2_x((i2d)iWidth,(i2d)iHeight),uB,uF);

	::ShowWindow( mkWnd , SW_SHOW );
	UpdateWindow(mkWnd);
	MoveWindow( mkWnd, WindowLeft, WindowTop, Width, Height, TRUE );

}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// _create the input system
void cSystemWin32::_createInputSystem(INPUTDEVICES)
{
	new cInputSystem();
	// we deal wit one interface currently
	iInputDevice* kPtr = new cDirectX9InputDevice((u32)mkWnd);
	X_SharedPtr(cInputSystem)->attachDeivice(kPtr);
	kPtr->release();
}

//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// create loaders
void cSystemWin32::_createLoaders()
{
	// texture loader
	new cTextureFactory();
	new cDirectX9MaterialFactory();
}
//@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// create scene and ui manager
void cSystemWin32::_createManagers()
{	
	// try to load the default skin
	X_SharedPtr(iFileSystem)->setDirectory(mkResourceDir,false);
	X_SharedPtr(iFileSystem)->setDirectory("skins",false);
	new cXuiManager(TEXT("default-ui-skin.xml"));
	// create the scene
	new cEngine();
	// create the simulator
	new cSimulator();
}
