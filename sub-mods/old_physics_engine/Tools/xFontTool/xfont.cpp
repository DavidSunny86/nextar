/*FILE MARK
 DOC ORIGINAL NAME:	fontcrt.cpp
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	1/11/2007		AT:		6:37:49 PM
END
******************************************************************************************************
*/

	/* const s16	 iStartPixel		= pData[0]; /* normally 0x8000 
	const s16	 iEndPixel			= pData[kDimn.dx]; /* normally 0x4000 
	const s16	 iTextColor			= pData[kDimn.dx*2]; /* normally 0x2000 
	const s16	 iShadowColor		= pData[kDimn.dx*3]; /* normally 0x1000 
	pData[kDimn.dx] = pData[kDimn.dx*2] = pData[kDimn.dx*3] = 0; */

#include <windows.h>
#define TOTAL_CHARS			128-21
typedef long format;
const int bitsperpix = sizeof(format)*8;
/* font header */
struct sTexRect
{
	long left,top,right,bottom;
};

struct sFontHeader
{
	float		fVersion;
	int			iWidth, iHeight;
	int			iBitsPerPix;	/* 8 */
	int			iTotalInfoBytes;
	int			iTextMask; /* mask */
	int			iShadowMask; /* */
	bool		bShadow;
	bool		bBold;		/* if the font is bold type */
	bool		bItalic;
	sTexRect	arrkCoords[TOTAL_CHARS];	/* array to texture coordinates */
};
/* some global data */
int     width = 0, height = 0, spacing = 0;
int     posx = 0, posy = 0;
SIZE    ls = {0};
bool    shadow = 0;
int     shthk = 0;
format* fontbits = 0;
format* shadowbits = 0;
HDC	    fontdc = 0;
HDC	    shadowdc = 0;
HFONT   myFont = 0, mySymbol1 = 0, mySymbol2 = 0;
const   TCHAR StdSymbolFont2[] = "Wingdings 2";
const   TCHAR StdSymbolFont1[] = "Wingdings";

/* true font info */
char*		 bits = 0;
sFontHeader	 fontheader = {0};
const char textc  = (char)0x2;
const char shadoc = (char)0x4;

/* forward declaration */
void myWriteToFile( const TCHAR* szFileName );

/* doInit */
void myInit()
{
	width = 0, height = 0, spacing = 0;
	posx = 0, posy = 0;
	shadow = 0;
	shthk = 0;
	fontbits = 0;
	shadowbits = 0;
	fontdc = 0;
	shadowdc = 0;
	myFont = 0;
	mySymbol1 = 0;
	mySymbol2 = 0;
	if( bits ) delete [] bits;
	bits = 0;
}

/* combine */
void myCombine()
{
	bits = new char[ width*height ];
	memset( bits, 0, width*height );

	for( int i = 0; i < width*height; i++ )
		if( fontbits[i] ) bits[i] = textc;

	if( !shadow ) return;

	for( i = 0; i < width*height; i++ )
		if( shadowbits[i] ) bits[i] |= shadoc;
}

void myInitBits( format* ptr )
{	ZeroMemory( ptr, width*height*sizeof(format) ); }

/* text out */
sTexRect myTextOut( TCHAR x )
{
	TCHAR str[] = {x,0};
	
	GetTextExtentPoint32( fontdc, str, 1, &ls );

	if( shadow )
	{
		/* add up the thickness value */
		ls.cx+=shthk;
		ls.cy+=shthk;
	}

	if( ( posx + ls.cx + spacing ) > width )
	{	
		posx = 0;
		posy += ( ls.cy + spacing );
	}
	/* rect{ x,y, x+ls.cx,y+ls.cy }*/
	sTexRect ret = {posx,posy, posx+ls.cx,posy+ls.cy};
	/* print the last dot at  */
	/* print the text */
	SetTextColor( fontdc, textc );
	//ExtTextOut( fontdc, posx, posy, ETO_OPAQUE, NULL, str, 1, NULL );
	SetBkMode( fontdc, ETO_OPAQUE );
	TextOut( fontdc, posx, posy, str, 1 );
	/* if we have shadow lets get it */
	if( shadow && shadowbits )
	{				
		SetTextColor( shadowdc, shadoc );
		ExtTextOut( shadowdc, shthk+posx, shthk+posy, ETO_OPAQUE, NULL, str, 1, NULL );
	}

	posx += ( ls.cx + spacing );
	return ret;
}

/* determine dimension */
void myFindDimns(int fheight)
{
	if( fheight > 35 )
	{	width = 512; height = 512;spacing = 1; shthk = 2; }
	else if( fheight > 16 )
    {	width = height = 256;spacing = 1; shthk = 1; }
	else if( fheight > 9 )
	{	width = 256;height = 128;spacing = 2; shthk = 1; }
	else if( fheight > 6 )
	{	width = 256;height = 64;spacing = 2; shthk = 1; }
	else
	{	width = 256; height = 8;spacing = 2; shthk = 1; }
}

void myCreate( HWND hWnd, LPCSTR font, int fheight, bool bold, bool italic, bool bshadow, const TCHAR* szFile )
{
	HBITMAP		fontbmp, shadowbmp;
	HGDIOBJ		oldfontbmp, oldshadowbmp, oldfont, oldshadowfont;
	BITMAPINFO	Bmi1, Bmi2;

	myInit();

	/* Create a DC for the font */
    fontdc			= CreateCompatibleDC( NULL );
	/* create the fonts */
	fheight = -MulDiv(fheight, GetDeviceCaps(fontdc, LOGPIXELSY), 72),
	myFont	  = CreateFont( fheight, 0, 0, 0, bold ? FW_BOLD:FW_NORMAL, italic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, font );
	mySymbol1 = CreateFont( fheight+1, 0, 0, 0, bold ? FW_BOLD:FW_NORMAL, italic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, StdSymbolFont1 );
	mySymbol2 = CreateFont( fheight+1, 0, 0, 0, bold ? FW_BOLD:FW_NORMAL, italic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, StdSymbolFont2 );
	/* determine and initialize attributes */
	myFindDimns(fheight);
	shadow = bshadow;
	/* initialize bitmap header */
	ZeroMemory( &Bmi1.bmiHeader, sizeof(BITMAPINFOHEADER) );
	ZeroMemory( &Bmi2.bmiHeader, sizeof(BITMAPINFOHEADER) );
    Bmi1.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    Bmi1.bmiHeader.biWidth       =  width;
    Bmi1.bmiHeader.biHeight      = -height;
    Bmi1.bmiHeader.biPlanes      = 1;
    Bmi1.bmiHeader.biCompression = BI_RGB;
    Bmi1.bmiHeader.biBitCount    = bitsperpix;
	CopyMemory( &Bmi2, &Bmi1, sizeof(Bmi1) );
	/* a bitmap for the font */
	fontbmp			= CreateDIBSection( fontdc, &Bmi1, DIB_RGB_COLORS, (void**)&fontbits, NULL, 0 );
    SetMapMode( fontdc, MM_TEXT );
	myInitBits(fontbits);
	/* select the font bitmap and set text alignment */
	oldfontbmp  = SelectObject( fontdc, (HGDIOBJ)fontbmp );
	SetTextAlign( fontdc, TA_TOP );
	SetBkColor( fontdc, 0 );
	/* Create a DC and a bitmap for the shadow if its set */
	if( shadow )
	{
		shadowdc  = CreateCompatibleDC( NULL );
		shadowbmp = CreateDIBSection( shadowdc, &Bmi2, DIB_RGB_COLORS, (void**)&shadowbits, NULL, 0 );
		myInitBits(shadowbits);
		/* select the shadow bmp and set text alignment */
		oldshadowbmp = SelectObject( shadowdc, (HGDIOBJ)shadowbmp );
		SetTextAlign( shadowdc, TA_TOP );
		SetBkColor( shadowdc, 0 );
	}
	
#define SelectFont( f ) SelectObject( fontdc, (HGDIOBJ)f ); if( shadow ) SelectObject( shadowdc, (HGDIOBJ)f );

	/* start printing */

	/* now go for letters */
	oldfont = SelectFont( myFont );
	if( shadow ) oldshadowfont = SelectObject( shadowdc, (HGDIOBJ)myFont );

	for( int c = 0; c < 128-32; c++ )
		myTextOut( (TCHAR)(c+32) );

	/* symbols */

	/* combine bits */
	myCombine();

	/* save to file */
	myWriteToFile( szFile );
	
	/* now deinitialize */
	SelectObject( fontdc, oldfontbmp );
    SelectObject( fontdc, oldfont );

	if( shadow )
	{
		SelectObject( shadowdc, oldshadowbmp );
	    SelectObject( shadowdc, oldshadowfont );
	}

    DeleteObject( (HGDIOBJ)myFont );
	DeleteObject( (HGDIOBJ)mySymbol1 );
	DeleteObject( (HGDIOBJ)mySymbol2 );

	DeleteObject( fontbmp );
	DeleteDC( fontdc );

	if( shadow )
	{
		DeleteObject( (HGDIOBJ)shadowbmp );
		DeleteDC( shadowdc );
	}

	myInit();
}

#include <d3dx9.h>

/* write the bitmap to file */
void myWriteToFile( const TCHAR* szFileName )
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


	device->CreateTexture( width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, 0 );
	if( !pTexture )
	{
		device->Release();
		d3d->Release();
		return;
	}

	D3DLOCKED_RECT rc;
	pTexture->LockRect( 0, &rc, 0, 0 );
	long *pBits = (long*)rc.pBits;
	/* now copy the font bits */
	for( int i = 0; i < height; i++ )
	{
		for( int j = 0; j < width; j++ )
		{
			int k = i*width + j;
			if( bits[k] & textc )
				pBits[k] = 0xffeef466;
			else if( bits[k] & shadoc )
				pBits[k] = 0x88090909;
			else
				pBits[k] = 0;
		}
	}

	pTexture->UnlockRect(0);

	D3DXSaveTextureToFile( szFileName, D3DXIFF_TGA, pTexture, 0 );

	pTexture->Release();
	device->Release();
	d3d->Release();
}