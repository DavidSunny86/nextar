//@ Abhishek Dey
//@ Jan 8 2007
//@ Sep 17 07
//@ Oct 27 07
#include <xmemory.h>
#include <isystem.h>
#include <win32/directx/cdirectx9font.h>
#include <win32/directx/cdirectx9renderer.h>

namespace nextar
{

#define ERROR1 X_Text("iXuiFont::iXuiFont := File not found")
#define ERROR2 X_Text("iXuiFont::iXuiFont := Unable to read from file")

#define DUMP_BUFFER(iCount) \
	X_BreakOutIf(iCount>N3D2DQuadPrecache);	\
	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->drawIndexedTriangleList( gk2DVertices, gu2DIndices, iCount*4, iCount*2, sizeof(cDxVertex2D) );

/*      draw method		*/
/*		1---------3		*/
/*		|		  |		*/
/*		|		  |		*/
/*		|		  |		*/
/*		0---------2		*/
/*		vertex index	*/
/*	  {(0,1,2) (1,3,2)} */
cDirectX9Font::cDirectX9Font( ) : mpkTexture(0)
{
	;
}

cDirectX9Font::~cDirectX9Font()
{
	/* destroy the texture */
	if( mpkTexture )
		mpkTexture->release();
}

void cDirectX9Font::setTexture(iTexture* t)
{
	X_Assert(t);
	mpkTexture = (cDirectX9Texture*)t;
	mpkTexture->addRef();
}

void cDirectX9Font::drawSingleLineText(const s32 x,const s32 y,const s32 iColors,const tchar* sText,s32 iCount)
{
	X_Assert(sText);

	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->setVertexShader(VERTEX_2D);
	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->setTexture(0,mpkTexture);
	s32	iDump = 0;
	f32 fStartY = (f32)y;
	f32 fStopY = y+(f32)(char_height);
	s32 iStartX = x;
	const tchar* sEnd = &sText[iCount];
	// assuming 2d mode 
	while(sText != sEnd)
	{
		if( *sText == '\t' )
		{
			iStartX += TabCount*mpkChars[0].width;
			sText++;
			continue;
		}

		register s32 idx;
		idx = (*sText)-32;
		if( idx < 0 || idx >= miMaxIndex ) idx = WRONG_CHAR;
		cFontChar& kChar = mpkChars[idx];
		
		gk2DVertices[iDump*4 + 0].mkPosition.x = (f32)iStartX;
		gk2DVertices[iDump*4 + 0].mkPosition.y = fStopY;
		gk2DVertices[iDump*4 + 0].mkTexCoord.x = kChar.mfu1;
		gk2DVertices[iDump*4 + 0].mkTexCoord.y = kChar.mfv2;
		gk2DVertices[iDump*4 + 0].miColor	   = iColors;

		gk2DVertices[iDump*4 + 1].mkPosition.x = (f32)iStartX;
		gk2DVertices[iDump*4 + 1].mkPosition.y = fStartY;
		gk2DVertices[iDump*4 + 1].mkTexCoord.x = kChar.mfu1;
		gk2DVertices[iDump*4 + 1].mkTexCoord.y = kChar.mfv1;
		gk2DVertices[iDump*4 + 1].miColor	   = iColors;

		iStartX += kChar.width;
		
		gk2DVertices[iDump*4 + 2].mkPosition.x = (f32)iStartX;
		gk2DVertices[iDump*4 + 2].mkPosition.y = fStopY;
		gk2DVertices[iDump*4 + 2].mkTexCoord.x = kChar.mfu2;
		gk2DVertices[iDump*4 + 2].mkTexCoord.y = kChar.mfv2;
		gk2DVertices[iDump*4 + 2].miColor	   = iColors;

		gk2DVertices[iDump*4 + 3].mkPosition.x = (f32)iStartX;
		gk2DVertices[iDump*4 + 3].mkPosition.y = fStartY;
		gk2DVertices[iDump*4 + 3].mkTexCoord.x = kChar.mfu2;
		gk2DVertices[iDump*4 + 3].mkTexCoord.y = kChar.mfv1;
		gk2DVertices[iDump*4 + 3].miColor	   = iColors;

		++iDump;
		sText++;
	
		if(iDump >= N3D2DQuadPrecache)
		{
			DUMP_BUFFER(iDump);
			iDump = 0;
		}
	}

	if(iDump)
		DUMP_BUFFER(iDump);
}

void cDirectX9Font::drawSingleLineText(const rect2_x& kClient,u32 uDrawFlags,const s32 iColors,const tchar* sText)
{
	X_Assert(sText);

	X_SharedPtr(iRenderer)->setVertexShader(VERTEX_2D);
	X_SharedPtr(iRenderer)->setTexture(0,mpkTexture);
	s32			 iDump = 0;
	s32			 iStartX;
	f32			 fStartY,fStopY;

	if(uDrawFlags & DrawYCentered)
	{
		fStartY = (f32)(kClient.top + ((kClient.height() - char_height) >> 1));
		fStopY = fStartY + (f32)(char_height);
	}
	else
	{
		fStartY = (f32)kClient.top;
		fStopY = fStartY + (f32)(char_height);
	}

	if(uDrawFlags & DrawXCentered)
		iStartX = kClient.left + ((kClient.width() - getTextExtents( sText, 0, -1 )) >> 1);
	else
		iStartX = kClient.left;

	if(iStartX < 0) iStartX = 0;

	// assuming 2d mode 
	while(*sText)
	{
		if( *sText == '\t' )
		{
			iStartX += TabCount*mpkChars[0].width;
			sText++;
			continue;
		}

		register s32 idx;
		idx = (*sText)-32;
		if( idx < 0 || idx >= miMaxIndex ) idx = WRONG_CHAR;
		cFontChar& kChar = mpkChars[idx];
		
		f32 fTmp = (f32)iStartX;
		iStartX += kChar.width;
		if(iStartX >= kClient.right) break;

		gk2DVertices[iDump*4 + 0].mkPosition.x = fTmp;
		gk2DVertices[iDump*4 + 0].mkPosition.y = fStopY;
		gk2DVertices[iDump*4 + 0].mkTexCoord.x = kChar.mfu1;
		gk2DVertices[iDump*4 + 0].mkTexCoord.y = kChar.mfv2;
		gk2DVertices[iDump*4 + 0].miColor	   = iColors;

		gk2DVertices[iDump*4 + 1].mkPosition.x = fTmp;
		gk2DVertices[iDump*4 + 1].mkPosition.y = fStartY;
		gk2DVertices[iDump*4 + 1].mkTexCoord.x = kChar.mfu1;
		gk2DVertices[iDump*4 + 1].mkTexCoord.y = kChar.mfv1;
		gk2DVertices[iDump*4 + 1].miColor	   = iColors;
	
		fTmp = (f32)iStartX;

		gk2DVertices[iDump*4 + 2].mkPosition.x = fTmp;
		gk2DVertices[iDump*4 + 2].mkPosition.y = fStopY;
		gk2DVertices[iDump*4 + 2].mkTexCoord.x = kChar.mfu2;
		gk2DVertices[iDump*4 + 2].mkTexCoord.y = kChar.mfv2;
		gk2DVertices[iDump*4 + 2].miColor	   = iColors;

		gk2DVertices[iDump*4 + 3].mkPosition.x = fTmp;
		gk2DVertices[iDump*4 + 3].mkPosition.y = fStartY;
		gk2DVertices[iDump*4 + 3].mkTexCoord.x = kChar.mfu2;
		gk2DVertices[iDump*4 + 3].mkTexCoord.y = kChar.mfv1;
		gk2DVertices[iDump*4 + 3].miColor	   = iColors;

		++iDump;
		sText++;
	
		if(iDump >= N3D2DQuadPrecache)
		{
			DUMP_BUFFER(iDump);
			iDump = 0;
		}
	}

	if(iDump)
		DUMP_BUFFER(iDump);
}

// X_Api drawMultilineText 
void cDirectX9Font::drawMultilineText(const rect2_x& kClient, u32 uDrawFlags,const s32 iColors, const tchar* sText)
{
	////////////
	//------------------------------------
	// INCORRECT CODE (REWRITE)
	register f32 fStopY;
	s32  iDump = 0;
	f32  fStartX, fStartY,fStartXPos;
	
	//@ assuming 2d mode 
	X_SharedPtr(iRenderer)->setVertexShader(VERTEX_2D);
	X_SharedPtr(iRenderer)->setTexture(0,mpkTexture);

	fStartX = (f32)kClient.left;
	fStartY = (f32)kClient.top;
	

//@	if( bCentered )
//@	{
//@		s32	 iLine,iD;
//@		iD = (s32)kClient.width() - getTextExtents( sText, 0, -1, iLine );
//@		if( iD > 0 ) fStartX +=  (f32)( iD >> 1 );
//@		iD = (s32)kClient.height() - (char_height*iLine);
//@		if( iD > 0 ) fStartY +=  (f32)( iD  >> 1 );
//@	}

	fStopY = fStartY + (f32)char_height;
	fStartXPos = fStartX;

	while(*sText)
	{

		register s32	idx;
		if( *sText == '\t' )
		{
			fStartX += TabCount*mpkChars[0].width;
			sText++;
			continue;
		}
		else if( *sText == '\n' )
		{
			sText++;
			goto nextline;
		}

		idx = (*sText)-32;
		if( idx < 0 || idx >= miMaxIndex ) idx = WRONG_CHAR;
		cFontChar& kChar = mpkChars[idx];
		
		gk2DVertices[iDump*4 + 0].mkPosition.x = fStartX;
		gk2DVertices[iDump*4 + 0].mkPosition.y = fStopY;
		gk2DVertices[iDump*4 + 0].mkTexCoord.x = kChar.mfu1;
		gk2DVertices[iDump*4 + 0].mkTexCoord.y = kChar.mfv2;
		gk2DVertices[iDump*4 + 0].miColor		= iColors;

		gk2DVertices[iDump*4 + 1].mkPosition.x = fStartX;
		gk2DVertices[iDump*4 + 1].mkPosition.y = fStartY;
		gk2DVertices[iDump*4 + 1].mkTexCoord.x = kChar.mfu1;
		gk2DVertices[iDump*4 + 1].mkTexCoord.y = kChar.mfv1;
		gk2DVertices[iDump*4 + 1].miColor		= iColors;

		fStartX += (f32)mpkChars[idx].width;

		gk2DVertices[iDump*4 + 2].mkPosition.x = fStartX;
		gk2DVertices[iDump*4 + 2].mkPosition.y = fStopY;
		gk2DVertices[iDump*4 + 2].mkTexCoord.x = kChar.mfu2;
		gk2DVertices[iDump*4 + 2].mkTexCoord.y = kChar.mfv2;
		gk2DVertices[iDump*4 + 2].miColor		= iColors;

		gk2DVertices[iDump*4 + 3].mkPosition.x = fStartX;
		gk2DVertices[iDump*4 + 3].mkPosition.y = fStartY;
		gk2DVertices[iDump*4 + 3].mkTexCoord.x = kChar.mfu2;
		gk2DVertices[iDump*4 + 3].mkTexCoord.y = kChar.mfv1;
		gk2DVertices[iDump*4 + 3].miColor		= iColors;

		++iDump;
		sText++;
	
		if( iDump >= N3D2DQuadPrecache || *sText == '\0')
		{
			DUMP_BUFFER(iDump);
			iDump = 0;
		}

		if( fStartX >= kClient.right ) 
		{
			if( fStopY > kClient.bottom ) 
				return;
nextline:

			fStartY = fStopY + 1;
			fStartX = fStartXPos;
			fStopY = (f32)char_height + fStartY;
		}
	}

}

}
