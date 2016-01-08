

#include "cTextureFactory.h"
#include <xcolorroutines.h>

using namespace nextar;

#pragma pack( push, packing )
#pragma pack( 1 )

struct TGAHeader
{	   
	u8		IdLength;
	u8		ColorMapType;
	u8		ImageType;
	u8		FirstEntryIndex[2];
	u16		ColorMapLength;
	u8		ColorMapEntrySize;
	u8		XOrigin[2];
	u8		YOrigin[2];
	u16		ImageWidth;
	u16		ImageHeight;
	u8		PixelDepth;
	u8		ImageDescriptor;
};

#pragma pack( pop, packing )

//------------------------------------------------------------------------------
//	This union is a total of 2 bytes in size. s and b both share the
//	same memory so the 2byte variable can be easily split into its constituant bytes.
//	By assigning 1 to the 2byte variable, we can expect the array b to hold
//	b[] = { 1, 0 } on a PC (LSB) and b[] = { 0, 1 } on an o2 (MSB). It also makes
//	it nice and easy to swap the variables bytes.
//------------------------------------------------------------------------------
union charToShort
{
	/* 1 x 2 byte variable */
	unsigned short s;
	/* 2 x 1 byte variables */
	unsigned char  b[2];
}XALIGN_STRUCT_1;

//------------------------------------------------------------------------------
//	This function is used to determine if the system has LSB or MSB byte ordering.
//	By using a union, both the 2byte variable(short) and the array of 2 individual
//	bytes (chars) will be stored in the same memory location. ie, the charToShort
//	union data type is only two bytes in size and allows you to split apart a 2byte//	variable to check the ordering.
//
//	The two byte variables in the tga header will have to have their byte orderings
//	switched on irix and mac, hence the use of this (sneaky) function to see if we need
//	to swap the data bytes.
//------------------------------------------------------------------------------
int CheckByteOrder()
{
	/* Create a 2byte check variable */
	union charToShort cs;

	/* assign 1 to the 2byte variable */
	cs.s = 1;

	/* check the first byte */
	if(cs.b[0] == 0)
		return 1;

	return 0;
}

//------------------------------------------------------------------------------
//	This function switches the byte orderings of two byte variables. It accepts a
//	pointer to the memory location you wish to swap.
//------------------------------------------------------------------------------
void Swap2Bytes(void *ptr)
{
	/* assign a pointer to the 2byte variable */
	unsigned char *pcs = (unsigned char*)ptr;

	/* swap the bytes around */
	unsigned char temp = pcs[0];
	pcs[0] = pcs[1];
	pcs[1] = temp;
}

char* loadCompressedImage( iFileReader *file, TGAHeader *header );

cSoftwareTexture* cTextureFactory::_tgaLoader(iFileReader* pkFile)
{
	TGAHeader			header = {0};
	cSoftwareTexture*	image = 0;

	pkFile->seek(0,true);
	pkFile->readData( &header, sizeof(TGAHeader) );

	if( CheckByteOrder() )
	{
		Swap2Bytes(&header.ColorMapLength);
		Swap2Bytes(&header.ImageWidth);
		Swap2Bytes(&header.ImageHeight);
	}

	// skip image identifikation field
	if (header.IdLength)
		pkFile->seek( header.IdLength,false);

	if (header.ColorMapType != 0)
	{
		// skip color map
		pkFile->seek( (header.ColorMapEntrySize/8 * header.ColorMapLength),false);
	}

	// read image
	s32 bytesPerPixel = header.PixelDepth/8;
	s32 imageSize = header.ImageHeight * header.ImageWidth * bytesPerPixel;
	char* data = 0;

	if (header.ImageType == 2)
	{
		data = new char[imageSize];
	  	pkFile->readData(data, imageSize);
	}
	else if(header.ImageType == 10)
		data = loadCompressedImage( pkFile, &header ); 
	else
	{
		ReportBug("CTgaLoader::LoadImage := Format not supported","cTextureFactory::_tgaLoader");
		return 0; 
	}

	size2_x kDimn((s32)header.ImageWidth, (s32)header.ImageHeight);
	switch(bytesPerPixel)
	{

	case 1: 
		ReportBug("CTgaLoader::LoadImage := Pixel Format not supported","cTextureFactory::_tgaLoader");
		break;
	case 2:
		// 16 bit image
		image = cSoftwareTexture::create( CF_A1R5G5B5, kDimn );
		xFlipMirror16B((s16*)image->lock(), (s16*)data, header.ImageWidth, header.ImageHeight, 0 );
		break;
	case 3:
		// 24 bit image
		image = cSoftwareTexture::create( CF_A8R8G8B8, kDimn );
		xFlipMirror24Bto32BShuffle((s32*)image->lock(),(char*)data, header.ImageWidth, header.ImageHeight, 0);
		break;

	case 4:
		// 32 bit image
		image = cSoftwareTexture::create( CF_A8R8G8B8, kDimn );
		xFlipMirror32Bto32B((s32*)image->lock(),(s32*)data,header.ImageWidth, header.ImageHeight, 0);
		break;
	}

	delete [] data;
	if(image) image->unlock();
	return image;
}

char* loadCompressedImage( iFileReader *file, TGAHeader *header )
{
	// This was written and sent in by Jon Pry, thank you very much!
	// I only changed the formatting a little bit.

	s32 bytesPerPixel	= header->PixelDepth/8;
	s32		imageSize	= header->ImageHeight * header->ImageWidth * bytesPerPixel;
	char*		 data	= new char[imageSize];
	s32	  currentByte	= 0;

	while(currentByte < imageSize)					
	{
		::byte chunkheader = 0;
		file->readData( &chunkheader, sizeof(char) ); // readData The Chunk's Header

		if(chunkheader < 128) // If The Chunk Is A 'RAW' Chunk
		{
			chunkheader++; // Add 1 To The Value To Get Total Number Of Raw Pixels

			file->readData( &data[currentByte], bytesPerPixel * chunkheader );
			currentByte += bytesPerPixel * chunkheader;
		}
		else
		{
			// thnx to neojzs for some fixes with this code

			// If It's An RLE Header
			chunkheader -= 127; // Subtract 127 To Get Rid Of The ID Bit 

			s32 dataOffset = currentByte; 
			file->readData( &data[dataOffset], bytesPerPixel ); 

			currentByte += bytesPerPixel; 

			for(s32 counter = 1; counter < chunkheader; counter++) 
			{ 
				for(s32 elementCounter=0; elementCounter < bytesPerPixel; elementCounter++) 
					data[currentByte + elementCounter] = data[dataOffset + elementCounter]; 

				currentByte += bytesPerPixel;
			}
		}
	}
	
	return data;
}
