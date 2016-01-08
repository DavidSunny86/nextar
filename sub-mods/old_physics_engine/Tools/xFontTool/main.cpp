
/***********************************************************************************************
/*									xFontTool					
/* This tool can be used to create Nextar compatible font files for win32
***********************************************************************************************/

#include <stdio.h>
#include <windows.h>
#include "resource.h"

#pragma warning( disable : 4800 )

/* global data */
HWND hWnd;
/* forward declrations */
BOOL CALLBACK myDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
int CALLBACK myEnumFontsCallback(CONST LOGFONT *lplf, CONST TEXTMETRIC *lptm, DWORD FontType, LPARAM lpData);
void		myRegisterNewFont(CONST LOGFONT *lplf);
void		myCreateDlgBox(HINSTANCE hInstance);
void		myCreateFontList();
void		myUpdatePreview();
const char* myGetSelFontName();
int			myGetSelFontHeight();
bool		myBold();
bool		myItalic();
bool		myOutSymbol();
bool		myCopyToClipboard();
void		myInitDlgBox(HINSTANCE);
void		myOpenFileSaveDlg();

extern void fmCreateFont( HWND hWnd, LPCSTR font, int fheight, bool bold, bool italic, bool symbol, const TCHAR* szFile );

/* myOpenDlg */
void myOpenFileSaveDlg()
{
	TCHAR FileName[1024] = {0};
	const char* name = myGetSelFontName();
	wsprintf( FileName, "%s.nfx", name );
	OPENFILENAME OpenFileParams = 
	{
		sizeof(OPENFILENAME),
		hWnd,
		0,
		0,
		0,
		0,
		0,
		FileName,
		1024,
		0,
		0,
		0,
		0,
		OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
		0,
		0,
		0,
		0,
		0,
		0
	};
	
	if( GetSaveFileName( &OpenFileParams ) )
		fmCreateFont( GetDlgItem( hWnd, IDC_PREVIEW ), myGetSelFontName(), 
			myGetSelFontHeight(), myBold(), myItalic(), myOutSymbol(), OpenFileParams.lpstrFile );
}

/* myInitDlgBox() */
void myInitDlgBox(HINSTANCE th)
{
	myCreateDlgBox(th);
	myCreateFontList();
	SendDlgItemMessage( hWnd, IDC_FONTHEIGHT, LB_SETCURSEL, 4, 0 );
	SendDlgItemMessage( hWnd, IDC_FONT, LB_SETCURSEL, 0, 0 );
	myUpdatePreview();
}

/* myBold() */
bool myBold()
{
	return (bool)IsDlgButtonChecked( hWnd, IDC_BOLD );
}

/* myItalic */
bool myItalic()
{
	return (bool)IsDlgButtonChecked( hWnd, IDC_ITALIC );
}

/* output symbol */
bool myOutSymbol()
{
	return (bool)IsDlgButtonChecked( hWnd, IDC_OUTSYM );
}

/* output to clipboard */
bool myCopyToClipboard()
{
	return (bool)IsDlgButtonChecked( hWnd, IDC_OUTCLIPBOARD );
}

/* myCreateDlgBox */
void myCreateDlgBox(HINSTANCE hInstance)
{
	hWnd = CreateDialogParam( hInstance, MAKEINTRESOURCE(IDD_FONTDLG), 0, &myDialogProc, 0 );
	ShowWindow( hWnd , SW_SHOW );
}

/* create font list */
void myCreateFontList()
{
	/* get the system font list */
	HDC dc = GetDC(hWnd);
	EnumFonts( dc, 0, &myEnumFontsCallback, 0 );
	ReleaseDC(hWnd,dc);

	/* create the height list */
	int  FontHeight[] = {4,6,8,9,10,11,12,14,16,18,20,22,24,26,28,36,48,56,68,72,0};
	int  i = 0;
	char str[14] = {0};

	while( FontHeight[i] != 0 )
	{
		sprintf(str, "%d", FontHeight[i] );
		SendDlgItemMessage( hWnd, IDC_FONTHEIGHT, LB_ADDSTRING, 0, (LPARAM)str );
		i++;
	}
}

/* get selected font */
const char* myGetSelFontName()
{
	static char str[512];
	int font = SendDlgItemMessage( hWnd, IDC_FONT, LB_GETCURSEL, 0, 0 );
	if( font == LB_ERR ) font = 0;
	SendDlgItemMessage( hWnd, IDC_FONT, LB_GETTEXT, (WPARAM)font, (LPARAM)str );
	return str;
}

/* get font height */
int	myGetSelFontHeight()
{
	static char str[10];
	int isize = SendDlgItemMessage( hWnd, IDC_FONTHEIGHT, LB_GETCURSEL, 0, 0 );
	if( isize == LB_ERR ) isize = 0;
	SendDlgItemMessage( hWnd, IDC_FONTHEIGHT, LB_GETTEXT, (WPARAM)isize, (LPARAM)str );
	return atoi(str);
}

/* update preview */
void myUpdatePreview()
{
	int height = myGetSelFontHeight();
	
	HDC dc = GetDC(hWnd);
	HFONT font = CreateFont(
		-MulDiv(height, GetDeviceCaps(dc, LOGPIXELSY), 72),
		0,0,0, myBold() ? FW_BOLD : 0,
		myItalic() ? TRUE : FALSE, 0,0,0,0,0,0,0, myGetSelFontName()
		);

	ReleaseDC(hWnd,dc);

	HWND hWndStat = GetDlgItem( hWnd, IDC_PREVIEW );
	const char* text = "The quick brown fox jumps over the lazy dog. 0123456789";

	dc = GetDC(hWndStat);

	RECT r;
	GetClientRect(hWndStat, &r);

	LOGBRUSH lbrush;
	lbrush.lbColor = GetSysColor(COLOR_3DFACE);
	lbrush.lbHatch = 0;
	lbrush.lbStyle = BS_SOLID;

	HBRUSH brush = CreateBrushIndirect(&lbrush);
	HPEN pen = CreatePen(PS_NULL, 0, 0);
	HGDIOBJ oldpen = SelectObject(dc, pen);
	HGDIOBJ oldbrush = SelectObject(dc, brush);
	HGDIOBJ oldfont = SelectObject(dc, font);

	SetBkMode(dc, OPAQUE);
	SetBkColor(dc, GetSysColor(COLOR_3DFACE));
	Rectangle(dc, 0,0, r.right, r.bottom);

	SetBkMode(dc, TRANSPARENT);
	SelectObject(dc, font);
	SetTextColor(dc,RGB(0,0,0));
	DrawText(dc, text, strlen(text), &r, DT_VCENTER | DT_CENTER | DT_SINGLELINE );

	SelectObject(dc, oldfont);
	SelectObject(dc, oldpen);
	SelectObject(dc, oldbrush);

	ReleaseDC(hWndStat, dc);

	DeleteObject(font);
	DeleteObject(brush);
	DeleteObject(pen);
}

/* register new font */
void myRegisterNewFont(CONST LOGFONT *lplf) 
{
	SendDlgItemMessage( hWnd, IDC_FONT,  LB_ADDSTRING, 0, (LPARAM)lplf->lfFaceName );
}

/* dilalog procedure */
BOOL CALLBACK myDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_PAINT:
		myUpdatePreview();
		break;
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:

		switch(LOWORD(wParam))
		{
		case IDC_BOLD:
		case IDC_ITALIC:
		case IDC_FONT:
		case IDC_FONTHEIGHT:
			myUpdatePreview();
			break;
		case IDC_SAVE:
			myOpenFileSaveDlg();
			break;
		}
		break;

	case WM_CLOSE:
		PostMessage( hWnd, WM_QUIT, 0, 0 );
		break;
	}
	return FALSE;
}

/* font enumeration */
int CALLBACK myEnumFontsCallback(
	CONST LOGFONT *lplf,
	CONST TEXTMETRIC *lptm,
	DWORD FontType,
	LPARAM lpData
	) 
{
	myRegisterNewFont(lplf);
	return TRUE;
}

/* winmain */
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
	myInitDlgBox(hInstance);
	
	MSG msg;
	do
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	while (msg.message != WM_QUIT);

	return 0;
}
