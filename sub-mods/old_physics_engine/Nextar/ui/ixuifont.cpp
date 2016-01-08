
//@ Abhishek Dey
//@ Jan 10 2007
//@ Jan 19 2007
//@ September 17 2007

#include <xmemory.h>
#include <isystem.h>
#include <ui/ixuifont.h>
#include <io/ifilesystem.h>
#include <graphics/xvertex.h>
#include <graphics/irenderer.h>
#include <common/xfontfile_s.h>

using namespace nextar;

// ctor
iXuiFont::iXuiFont() : mbHasSymbols(0), 
miMaxIndex(0), mpkChars(0), muFontStyle(0), char_height(0)
{
}

/* dtor */
iXuiFont::~iXuiFont()
{
	if( mpkChars )
		delete [] mpkChars;
}

/* _createFromFile from file */
bool iXuiFont::_createFromFile( iFileReader* pkReader )
{
	font_header kHeader = {0};
	iTexture*   pkTexture = 0;

	if( pkReader->readData( &kHeader, sizeof(kHeader) ) )
	{
		if( kHeader.bits_per_pix == 8 )
		{
			char_info* pkCharInfo = 0;
			u8*	puBuffer = 0;
			miMaxIndex = (s32)kHeader.char_count;
			s32 iWidth  = (s32)kHeader.texture_width;
			s32 iHeight = (s32)kHeader.texture_height;
			char_height = kHeader.char_height;
			muFontStyle = 0;

			if(kHeader.is_italized)
				muFontStyle |= FONTSTYLE_ITALIC;
			if(kHeader.is_underlined)
				muFontStyle |= FONTSTYLE_UNDERLINED;
			if(kHeader.is_strokeout)
				muFontStyle |= FONTSTYLE_STRIKEOUT;

			bool bResult = true;

			if( miMaxIndex < 0 || iWidth < 0 || iHeight < 0 ) return false;
			
			if( kHeader.has_smoothing )
				pkTexture = X_SharedPtr(iRenderer)->createTexture( iWidth, iHeight, TCF_32B );
			else
				pkTexture = X_SharedPtr(iRenderer)->createTexture( iWidth, iHeight, TCF_16B );
			
			if( pkTexture == NULL ) 
				return false;

			pkCharInfo = new char_info[ miMaxIndex ];
			puBuffer   = new u8[ kHeader.texture_size ];

			/* read data from file */

			bResult &= pkReader->readData( pkCharInfo, miMaxIndex*sizeof(char_info) );
			bResult &= pkReader->readData( puBuffer, kHeader.texture_size );

			if( bResult )
			{				
				/* done with reading, now _createFromFile font data */
				mpkChars = new cFontChar[ miMaxIndex ];

				for( s32 i = 0; i < miMaxIndex; i++ )
				{
					mpkChars[i].width = pkCharInfo[i].width;
					mpkChars[i].mfu1 = pkCharInfo[i].u0;
					mpkChars[i].mfv1 = pkCharInfo[i].v0;
					mpkChars[i].mfu2 = pkCharInfo[i].u1;
					mpkChars[i].mfv2 = pkCharInfo[i].v1;
				}

				s32 iSize = iWidth*iHeight;

				if( kHeader.has_smoothing )
				{
					/* indicates 32 bit texture */
					u32* piPixels = (u32*)pkTexture->lock();
					if( piPixels ) 
						for( s32 i = 0; i < iSize; i++ )
							piPixels[i] = A8R8G8B8( puBuffer[i], puBuffer[i], puBuffer[i], puBuffer[i] );
					else
						bResult = false;
				}
				else
				{
					/* indicates 16 bit texture */
					u16* piPixels = (u16*)pkTexture->lock();
					if( piPixels ) 
						for( s32 i = 0; i < iSize; i++ )
							piPixels[i] = puBuffer[i] ? (u16)0xffff : 0;
					else
						bResult = false;
				}

				pkTexture->unlock();
				setTexture(pkTexture);
				pkTexture->release();
			}

			delete [] pkCharInfo;
			delete [] puBuffer;

			return bResult;
		}
	}

	return false;
}

X_Api s32 iXuiFont::getTextExtents( const tchar* sText, s32 iStart, s32 iEnd ) const
{
	s32 iSize = 0;

	if( iEnd >= 0 )
	{
		for( s32 i = iStart; i < iEnd; i++ )
		{
			register tchar t = sText[i];
			if( t == '\t' )
				iSize += mpkChars[0].width*TAB_SIZE;
			else if( t < 128 && t >= 32  ) 
				iSize += mpkChars[t-32].width;
			else
				iSize += mpkChars[WRONG_CHAR].width;
		}
	}
	else
	{
		while(*sText)
		{
			register tchar t = *sText;
			if( t == '\t' )
				iSize += mpkChars[0].width*TAB_SIZE;
			else if( t < 128 && t >= 32  ) 
				iSize += mpkChars[t-32].width;
			else
				iSize += mpkChars[WRONG_CHAR].width;
			sText++;
		}
	}

	return iSize;
}

X_Api s32 iXuiFont::getTextExtents( const tchar* sText, s32 iStart, s32 iEnd, s32& y ) const
{
	y = 0;
	s32 iSize = 0;

	if( iEnd >= 0 )
	{
		for( s32 i = iStart; i < iEnd; i++ )
		{
			register tchar t = sText[i];
			if( t == '\t' )
				iSize += mpkChars[0].width*TAB_SIZE;
			if( t == '\n' )
				y++;
			else if( t < 128 && t >= 32  ) 
				iSize += mpkChars[t-32].width;
			else
				iSize += mpkChars[WRONG_CHAR].width;
		}
	}
	else
	{
		while(*sText)
		{
			register tchar t = *sText;
			if( t == '\t' )
				iSize += mpkChars[0].width*TAB_SIZE;
			if( t == '\n' )
				y++;
			else if( t < 128 && t >= 32  ) 
				iSize += mpkChars[t-32].width;
			else
				iSize += mpkChars[WRONG_CHAR].width;
			sText++;
		}
	}

	return iSize;
}

/*
X_Api s32 iXuiFont::getCharIdxFromPos( const tchar* sText, i2d iPix, i2d& start_x, i2d& end_x ) const
{
	register s32 x = 0;
	register s32 i = 0;
	register tchar t = sText[i];
	start_x = 0;

	while (t)
	{
		if( t < 128 && t >= 32  ) 
			x += mpkChars[t-32].width;
		else
			x += mpkChars[WRONG_CHAR].width;

		if (x >= iPix)
		{
			end_x = x;
			return i;
		}

		++i;
		t = sText[i];
		start_x = x;
	}

	return i;
}

X_Api void iXuiFont::drawTextSingleLine( const rect2_x& kClient, u32 uDrawFlags, 
 const s32 *piColors, const s32* piPos, 
 const tchar* sText )
{
	X_Assert(piPos);
	X_Assert(piColors);
	X_Assert(sText);
	X_Assert(gk2DVertices);

	X_SharedPtr(iRenderer)->setVertexShader(VERTEX_2D);
	X_SharedPtr(iRenderer)->setTexture(0,mpkTexture);
	size2_x&		rkRm = X_SharedPtr(iRenderer)->getScreenSize();
	s32				iDump = 0;
	s32				iStartX = 0;
	f32				fStartY,fStopY;

	s32 iXPlus = rkRm.dx>>1;
	s32 iYPlus = rkRm.dy>>1;
	f32	fXFact = 1.f/(f32)iXPlus;
	f32 fYFact = 1.f/(f32)iYPlus;
	iXPlus = -iXPlus;
	iYPlus = rkRm.dy - iYPlus;

	if(uDrawFlags & DrawYCentered)
	{
		register s32 iVal = iYPlus - (kClient.top + ((kClient.height() - char_height) >> 1));
		fStartY = (f32)iVal * fYFact;
		fStopY = (f32)(iVal - char_height) * fYFact;
	}
	else
	{
		register s32 iVal = iYPlus - kClient.top;
		fStartY = (f32)iVal * fYFact;
		fStopY = (f32)(iVal - char_height) * fYFact;
	}

	if(uDrawFlags & DrawXCentered)
	{
		s32 iLine;
		register s32 iVal = (kClient.width() - getTextExtents( sText, 0, -1, iLine )) >> 1;
		iStartX = kClient.left + iVal + iXPlus;
	}
	else
	{
		iStartX = kClient.left + iXPlus;
	}
	// assuming 2d mode 
	s32 i = 0;
	while(*sText)
	{
		if( i == *piPos )
		{
			piColors++;
			piPos++;
		}

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
		
		f32 fTmp = iStartX*fXFact;
		
		gk2DVertices[iDump*4 + 0].mkPosition.x = fTmp;
		gk2DVertices[iDump*4 + 0].mkPosition.y = fStopY;
		gk2DVertices[iDump*4 + 0].mkTexCoord.x = kChar.mfu1;
		gk2DVertices[iDump*4 + 0].mkTexCoord.y = kChar.mfv2;
		gk2DVertices[iDump*4 + 0].miColor	   = *piColors;

		gk2DVertices[iDump*4 + 1].mkPosition.x = fTmp;
		gk2DVertices[iDump*4 + 1].mkPosition.y = fStartY;
		gk2DVertices[iDump*4 + 1].mkTexCoord.x = kChar.mfu1;
		gk2DVertices[iDump*4 + 1].mkTexCoord.y = kChar.mfv1;
		gk2DVertices[iDump*4 + 1].miColor	   = *piColors;

		iStartX += kChar.width;
		fTmp = iStartX*fXFact;

		gk2DVertices[iDump*4 + 2].mkPosition.x = fTmp;
		gk2DVertices[iDump*4 + 2].mkPosition.y = fStopY;
		gk2DVertices[iDump*4 + 2].mkTexCoord.x = kChar.mfu2;
		gk2DVertices[iDump*4 + 2].mkTexCoord.y = kChar.mfv2;
		gk2DVertices[iDump*4 + 2].miColor	   = *piColors;

		gk2DVertices[iDump*4 + 3].mkPosition.x = fTmp;
		gk2DVertices[iDump*4 + 3].mkPosition.y = fStartY;
		gk2DVertices[iDump*4 + 3].mkTexCoord.x = kChar.mfu2;
		gk2DVertices[iDump*4 + 3].mkTexCoord.y = kChar.mfv1;
		gk2DVertices[iDump*4 + 3].miColor	   = *piColors;

		++iDump;
		sText++;
		++i;

		if( iDump >= N3D2DQuadPrecache || *sText == '\0')
		{
			DUMP_BUFFER(iDump);
			iDump = 0;
		}
	}
}

// X_Api drawMultilineText 
X_Api void iXuiFont::drawMultilineText( const rect2_x& kClient, u32 uDrawFlags, 
 const s32 *piColors, const s32* piPos, 
 const tchar* sText )
{

	////////////
	//------------------------------------
	// INCORRECT CODE (REWRITE)

	X_BreakOutIf(!piColors);
	X_BreakOutIf(!sText);
	X_BreakOutIf(!gk2DVertices);
	
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
	s32 i = 0;

	while(*sText)
	{

		register s32	idx;

		if( i == *piPos )
		{
			piColors++;
			piPos++;
		}

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
		gk2DVertices[iDump*4 + 0].miColor		= *piColors;

		gk2DVertices[iDump*4 + 1].mkPosition.x = fStartX;
		gk2DVertices[iDump*4 + 1].mkPosition.y = fStartY;
		gk2DVertices[iDump*4 + 1].mkTexCoord.x = kChar.mfu1;
		gk2DVertices[iDump*4 + 1].mkTexCoord.y = kChar.mfv1;
		gk2DVertices[iDump*4 + 1].miColor		= *piColors;

		fStartX += (f32)mpkChars[idx].width;

		gk2DVertices[iDump*4 + 2].mkPosition.x = fStartX;
		gk2DVertices[iDump*4 + 2].mkPosition.y = fStopY;
		gk2DVertices[iDump*4 + 2].mkTexCoord.x = kChar.mfu2;
		gk2DVertices[iDump*4 + 2].mkTexCoord.y = kChar.mfv2;
		gk2DVertices[iDump*4 + 2].miColor		= *piColors;

		gk2DVertices[iDump*4 + 3].mkPosition.x = fStartX;
		gk2DVertices[iDump*4 + 3].mkPosition.y = fStartY;
		gk2DVertices[iDump*4 + 3].mkTexCoord.x = kChar.mfu2;
		gk2DVertices[iDump*4 + 3].mkTexCoord.y = kChar.mfv1;
		gk2DVertices[iDump*4 + 3].miColor		= *piColors;

		++iDump;
		sText++;
		++i;

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
*/
