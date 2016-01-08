/*FILE MARK
 DOC ORIGINAL NAME:	fontmain.cpp
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	1/14/2007		AT:		10:01:13 AM
END
******************************************************************************************************
*/

#include <windows.h>
#include <xfontfile_s.h>
#include <stdio.h>

typedef unsigned long u32;
typedef unsigned short u16;
typedef char format;

#define CHAR_COUNT	(128-21)
#define A1R5G5B5(a,r,g,b) ((u16) ( ((u16)(a) << 15) | ((u16)(r) << 10) | ((u16)(g) << 5) | ((u16)(b)) ))
#define A8R8G8B8(a,r,g,b) ( A8R8G8B8_A(((u32)a),((u32)r),((u32)g),((u32)b)) )
#define A8R8G8B8_A(a,r,g,b)  (u32)( ((a&0xff) <<24) | ((r&0xff)<<16) | ((g&0xff)<<8) | (b&0xff) )

struct fmCharInfo
{
	short	code;
	HFONT	mkFont;
	long	width, miHeight;
};

HFONT			fmFont = 0;
HFONT			fmSym1 = 0, fmSym2 = 0;
HDC				fmDC = 0, fmbpDC = 0;
fmCharInfo		arrChar[CHAR_COUNT] = {0};
int				iWidth, iHeight;
int				iFontConstHeight;
const int		iXStartOffset(1),iXEndOffset(1);
const int		iYStartOffset(1),iYEndOffset(1);
double			fRWidth,fRHeight;
int				iIndexCount(0);
bool			fmSymbolOut;
const  TCHAR StdSymbolFont1[] = "Wingdings 2";
const  TCHAR StdSymbolFont2[] = "Wingdings";

#ifdef _DEBUG
#	define D3DSAVE
#	define DSAVEASTXTFILE
#endif

/* file format */
/* -- Nextar File Header */
/* -- Nextar Font Header */
/* -- Character Info Array */
/* -- Pixel Buffer */

/* header */
file_header	version = {0};
font_header	fmHeader = {0};
/* char array */
char_info*	fmCharArray;
/* character buffer */
format*	fmBuffer = 0;
/* const char color */
const format iBackMask = (format)0;	/* high order bit */
/* has char smoothing */
bool bCharSmoothing = false;
/* forward declaration [in order of being written]*/
void fmInit();
long fmSurfTest( long witdh );
void fmSurface();
void fmFillSizeList();
void fmTextOut( int &x, int &y, int index );
void fmCreate( const TCHAR* szFile );
void fmCreateFont( HWND hWnd, LPCSTR mkFont, int fheight, bool bold, bool italic, bool symbol, const TCHAR* szFile );
void fmSave( const TCHAR* );

extern bool myCopyToClipboard();

/* fmInit() */
void fmInit()
{
	fmDC	= 0;
	fmFont = 0;
	fmSym1 = 0;
	fmSym2 = 0;
	fmBuffer = 0;
	iFontConstHeight = 0;
	iWidth = 0;
	iHeight = 0;
	iIndexCount = 0;
	bCharSmoothing = false;
	memset( &fmHeader, 0, sizeof(fmHeader) );
	memset( &fmCharArray, 0, sizeof(fmCharArray) );
}

/* fmSurfTest: returns the surface miHeight for a 
   given width
*/
long fmSurfTest( long width )
{
	long  posx = 0, posy = 0;

	int i = 0;
	for( ; i < iIndexCount; i++ )
	{
		int CurrWidth = iXStartOffset + arrChar[i].width + iXEndOffset;

		if( ( posx + CurrWidth ) > width )
		{
			posx = 0;
			posy += (iYStartOffset + arrChar[i].miHeight + iYEndOffset);
		}

		posx += CurrWidth;
	}

	posy += (iYStartOffset + arrChar[0].miHeight + iYEndOffset);

	long miHeight = 0x01;
	while( miHeight < posy )
		miHeight <<= 1;

	return miHeight > 2048 ? -1 : miHeight;
}

/* determination of surface attributes 
   once the array of characters are filled
*/
void fmSurface()
{
	long iLastWidth, iLastHeight;
	unsigned long iLastArea = -1; // max
	long arrWidths[] = { 256, 512, 128, 16, 32, 64, 1024, 2048, 0 }; // 0 = sentinel

	for( int i = 0; arrWidths[i] != 0; i++ )
	{
		int miHeight = fmSurfTest(arrWidths[i]);
		if( miHeight > 0 && (unsigned long)(arrWidths[i] * miHeight) < iLastArea )
		{
			iLastWidth  = arrWidths[i];
			iLastHeight = miHeight;
			iLastArea  = iLastWidth*iLastHeight;
		}
	}

	iHeight = iLastHeight;
	iWidth  = iLastWidth;
}

void fmGetTextDim( TCHAR x, int idx, HFONT f )	
{
	SIZE ls;
	TCHAR str[2] = {x,'\0'};
	arrChar[idx].code = (short)x;
	arrChar[idx].mkFont = f;
	GetTextExtentPoint32( fmDC, str, 1, &ls );
	arrChar[idx].miHeight = ls.cy;
	arrChar[idx].width = ls.cx;
	iFontConstHeight = ls.cy;
}

/* create the mkFont size list */
void fmFillSizeList()
{
	/* assummes : fmDC, fmFont, fmSym1, fmSym2 ; are valid */

	SelectObject( fmDC, fmFont );

	int i = 0;
	for( ; i < 128-32; i++ )
		fmGetTextDim( i+32, i, fmFont );

	if( fmSymbolOut )
	{

		SelectObject( fmDC, fmSym1 );

		fmGetTextDim('$',i,fmSym1); i++; // pen
		fmGetTextDim('%',i,fmSym1); i++;// scissor
		fmGetTextDim('2',i,fmSym1); i++;// clipboard
		fmGetTextDim('E',i,fmSym1); i++;// bullet :hand
		fmGetTextDim('O',i,fmSym1); i++;// cross
		fmGetTextDim('P',i,fmSym1); i++;// tick 
		fmGetTextDim('Q',i,fmSym1); i++;// box cross
		fmGetTextDim('R',i,fmSym1); i++;// box tick

		SelectObject( fmDC, fmSym2 );

		fmGetTextDim('0',i,fmSym2); i++;// foler
		fmGetTextDim('1',i,fmSym2); i++;// foler open
		fmGetTextDim('2',i,fmSym2); i++;// file
	}

	iIndexCount = i;
}

/* fmTextOut */
void fmTextOut( int &x, int &y, int index )
{
	SelectObject( fmDC, arrChar[index].mkFont );
	TCHAR str[2] = { (TCHAR)arrChar[index].code, 0 };

	int CurrWidth = iXStartOffset + arrChar[index].width + iXEndOffset;
	int CurrHeight = iYStartOffset + arrChar[index].miHeight + iYEndOffset;

	if( (x + CurrWidth) > iWidth )
	{	
		x = 0;
		y += CurrHeight;
	}

	int aXpos = x+iXStartOffset;
	int aYpos = y+iYStartOffset;

	RECT rc;
	rc.left = aXpos;
	rc.top = aYpos;
	rc.right = aXpos+arrChar[index].width;
	rc.bottom = aYpos+arrChar[index].miHeight;

	DrawTextEx( fmDC, str, 1, &rc, DT_CENTER|DT_VCENTER, 0 );

	fmCharArray[index].code = arrChar[index].code;
	fmCharArray[index].width = (short)arrChar[index].width;
	fmCharArray[index].u0 = (float)((double)(rc.left) * fRWidth);
	fmCharArray[index].v0 = (float)((double)(rc.top) * fRHeight);
	fmCharArray[index].u1 = (float)((double)(rc.right) * fRWidth);
	fmCharArray[index].v1 = (float)((double)(rc.bottom) * fRHeight);

	x += CurrWidth;
}

/* create a font map */
void fmCreate( const TCHAR* szFile)
{
	HBITMAP		fontbmp;
	HBRUSH		brush;
	HPEN		pen;
	
	HGDIOBJ		olfontbmp, olpen, olbrush;
	int			x(0),y(0);

	/* create a brush */
	LOGBRUSH lbrush;
	lbrush.lbColor = RGB(0,0,0);
	lbrush.lbHatch = 0;
	lbrush.lbStyle = BS_SOLID;
	brush = CreateBrushIndirect(&lbrush);
	/* create a pen */
	pen = CreatePen(PS_NULL, 0, 0);
	/* create the bitmap */
	fontbmp	= CreateCompatibleBitmap( fmbpDC, iWidth, iHeight );
	/* select the new objects */
	olfontbmp  = SelectObject( fmDC, fontbmp );
	olpen	   = SelectObject( fmDC, pen );
	olbrush    = SelectObject( fmDC, brush );
	/* set text attributes */
	SetBkMode( fmDC, TRANSPARENT );
	SetTextColor( fmDC, RGB(255,255,255) );

	/* create the buffers */
	fmCharArray = new char_info[iIndexCount];
	fmBuffer = new format[iWidth*iHeight];

	/* output the text */
	for( int i = 0; i < iIndexCount; i++ )
		fmTextOut( x, y, i );

	/* now copy pix value to buffer */
	for( y = 0; y < iHeight; y++ )
	{
		for( x = 0; x < iWidth; x++ )
		{	
			u32		uColSet = GetPixel( fmDC, x, y );
			format	uColSetByte = (format)GetRValue(uColSet);
			if( uColSetByte > 0 && uColSetByte < 255 ) 
				bCharSmoothing = true;
			fmBuffer[y*iWidth+x] = uColSetByte;
		}
	}

	if( myCopyToClipboard() )
	{
		/* copy data to clipboard */
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, fontbmp);
		CloseClipboard();
	}

	fmHeader.char_height = iFontConstHeight;
	fmHeader.has_smoothing = bCharSmoothing;

	/* save to file */
	fmSave(szFile);

#ifdef DSAVEASTXTFILE
	TCHAR buff[1024];
	wsprintf( buff, "%s%s", szFile, ".txt" );

	// we save this to a seperate file
	FILE* f = fopen(buff,"wb");
	for(int i=0;i<iWidth*iHeight;i++)
		fwrite(&fmBuffer[i],sizeof(format),1,f);
	fclose(f);
#endif

	delete [] fmBuffer;
	delete [] fmCharArray;

	SelectObject( fmDC, olfontbmp );
	SelectObject( fmDC, olpen );
	SelectObject( fmDC, olbrush );

	DeleteObject( fontbmp );
	DeleteObject( pen );
	DeleteObject( brush );
}

/* create mkFont */
void fmCreateFont( 
	HWND hWnd, LPCSTR mkFont, int fheight, 
	bool bold, bool italic, bool symbol, 
	const TCHAR* szFile )
{
	HGDIOBJ olFont;
	HDC		olDC;

	/* init globals */
	fmSymbolOut = symbol;
	/* create a DC for the mkFont */
	olDC	= GetDC( hWnd );
	fmbpDC	= olDC;
	fmDC	= CreateCompatibleDC( olDC );

#define FONTQUALITY		ANTIALIASED_QUALITY
	/* create the fonts */
	fheight = -MulDiv(fheight, GetDeviceCaps(fmDC, LOGPIXELSY), 72);
	fmFont = CreateFont( fheight, 0, 0, 0, bold ? FW_BOLD:FW_NORMAL, italic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, FONTQUALITY, VARIABLE_PITCH, mkFont );
	fmSym1 = CreateFont( fheight, 0, 0, 0, bold ? FW_BOLD:FW_NORMAL, italic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, FONTQUALITY, VARIABLE_PITCH, StdSymbolFont1 );
	fmSym2 = CreateFont( fheight, 0, 0, 0, bold ? FW_BOLD:FW_NORMAL, italic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, FONTQUALITY, VARIABLE_PITCH, StdSymbolFont2 );

#undef FONTQUALITY

	/* get the ol mkFont */
	olFont = SelectObject( fmDC, fmFont );
	/* create the mkFont attribute list */
	fmFillSizeList();
	/* determine the size of the surface */
	fmSurface();
	/* fill up the minimum information */
	version.version		= 1.0f; /* X_VERSION_REAL */
	version.code	= Magic_FontType;
	fmHeader.char_count	= iIndexCount;
	fmHeader.char_buffer_size		= sizeof(char_info)*iIndexCount;
	fmHeader.bits_per_pix		= sizeof(format)*8;
	fmHeader.texture_height	= iHeight;
	fmHeader.texture_width		= iWidth;
	fmHeader.texture_size		= iHeight*iWidth*sizeof(format);
	fmHeader.has_symbols	= symbol;
	fmHeader.has_smoothing = bCharSmoothing;
	fmHeader.char_height   =-fheight;
	fmHeader.is_italized		= italic;
	fmHeader.is_underlined	= false;
	fmHeader.is_strokeout	= false;
	fRWidth  = 1 / (double)iWidth;
	fRHeight = 1 / (double)iHeight;

	/* create and copy a text bitmap */
	fmCreate( szFile );

	/* delete objects */
	SelectObject( fmDC, olFont );

	DeleteObject( fmFont );
	DeleteObject( fmSym1 );
	DeleteObject( fmSym2 );
	ReleaseDC( hWnd, olDC );
	DeleteDC( fmDC );
}

/* fmSave */
#ifdef D3DSAVE

#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>

void fmSave( const TCHAR* szFile )
{
	IDirect3D9*			d3d = 0;
	IDirect3DDevice9*  device = 0;
	IDirect3DTexture9* pTexture = 0;

    if( NULL == ( d3d = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return;

    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    if( FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, GetActiveWindow(),
                                   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                   &d3dpp, &device ) ) )
    {
		d3d->Release();
        return;
    }


	device->CreateTexture( iWidth, iHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, 0 );
	if( !pTexture )
	{
		device->Release();
		d3d->Release();
		return;
	}

	D3DLOCKED_RECT rc;
	pTexture->LockRect( 0, &rc, 0, 0 );
	long *pBits = (long*)rc.pBits;
	/* now copy the mkFont fmBuffer */
	for( int i = 0; i < iHeight*iWidth; i++ )
		pBits[i] = (long)A8R8G8B8(fmBuffer[i],fmBuffer[i],fmBuffer[i],fmBuffer[i]);

	pTexture->UnlockRect(0);

	TCHAR buff[1024];
	wsprintf( buff, "%s%s", szFile, ".bmp" );

	D3DXSaveTextureToFile( buff, D3DXIFF_BMP, pTexture, 0 );

	pTexture->Release();
	device->Release();
	d3d->Release();

	FILE * pFile = fopen( szFile, "wb" );
	
	if( pFile )
	{
		fwrite( &version, sizeof(version), 1, pFile );
		fwrite( &fmHeader, sizeof(fmHeader), 1, pFile );
		fwrite( fmCharArray, fmHeader.char_buffer_size, 1, pFile );
		fwrite( fmBuffer, fmHeader.texture_size, 1, pFile );
		fclose( pFile );
	}
}

#else

#include <stdio.h>

void fmSave( const TCHAR* szFile )
{
	FILE* pFile = fopen( szFile, "wb" );
	if( pFile )
	{
		fwrite( &version, sizeof(version), 1, pFile );
		fwrite( &fmHeader, sizeof(fmHeader), 1, pFile );
		fwrite( fmCharArray, fmHeader.char_buffer_size, 1, pFile );
		fwrite( fmBuffer, fmHeader.texture_size, 1, pFile );
		fclose( pFile );
	}
}

#endif
