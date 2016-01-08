//@ Abhishek Dey
//@ September 16 07
//@ September 27 07

#include "cTextureFactory.h"
#include <png.h>
#include <isystem.h>

using namespace nextar;

#define PNG_BYTES_TO_CHECK 4
bool check_if_png(iFileReader *pkFile)
{
	u8 buf[PNG_BYTES_TO_CHECK];
	// Read in some of the signature bytes
	if (!pkFile->readData(buf,PNG_BYTES_TO_CHECK)) 
		return 0;
	// Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	// Return nonzero (true) if they match
	return (!png_sig_cmp((png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

void png_readfn_ifile(png_structp png_ptr,png_bytep data,png_size_t length)
{
	if( !((iFileReader*)png_ptr->io_ptr)->readData((void*)data,(u32)length) )
		png_error(png_ptr,"Read Error");
}

#pragma warning( disable : 4996 )
cSoftwareTexture* cTextureFactory::_pngLoader(iFileReader *pkFile)
{
	if(!check_if_png(pkFile)) return 0;

	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	bool alpha;
	s8 gbuffer[4096];
	cSoftwareTexture* pkImage = 0;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	if (png_ptr == NULL)
	{
		return 0;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return 0;
	}

	png_set_read_fn(png_ptr, (void *)(pkFile), png_readfn_ifile);

	png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, int_p_NULL, int_p_NULL);

	png_set_strip_16(png_ptr);

	if ( bit_depth != 8 && bit_depth != 16 )
		goto error;

	if (color_type == PNG_COLOR_TYPE_RGBA)
		alpha = true;
	else if(color_type != PNG_COLOR_TYPE_RGB )
		alpha = false;
	else
		goto error;

	// setup gamma
	//@ TODO: add code to retrieve the gamma here
	double screen_gamma = 0;//X_SharedPtr(iSystem)->getUserGamma();
	// and comment this
	s8* gamma_str;
	if( (gamma_str = getenv("SCREEN_GAMMA")) != NULL )
		screen_gamma = atof(gamma_str);
	else
		screen_gamma = 2.2;

	int intent;

	if (png_get_sRGB(png_ptr, info_ptr, &intent))
		png_set_gamma(png_ptr, screen_gamma, 0.45455);
	else
	{
		double image_gamma;
		if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
			png_set_gamma(png_ptr, screen_gamma, image_gamma);
		else
			png_set_gamma(png_ptr, screen_gamma, 0.45455);
	}

	// update
	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, int_p_NULL, int_p_NULL);

	int bytes_per_row = png_get_rowbytes(png_ptr, info_ptr); 

	if( bytes_per_row >= (int)sizeof( gbuffer ) )
		goto error;
	
	if(alpha)
		pkImage = cSoftwareTexture::create(CF_A8R8G8B8,size2_x(width,height));
	else
		pkImage = cSoftwareTexture::create(CF_R8G8B8,size2_x(width,height));

	s8 *src = 0;
	s8* dest = (s8*)pkImage->lock();
	for ( u32 y = 0; y < height; y++ ) 
	{
		png_read_row(png_ptr, (png_bytep)gbuffer, 0);
		src = gbuffer;
		for ( u32 x = 0; x < width; x++ ) 
		{
			if(alpha)
			{
				// bgra
				dest[0] = *(src+2);
				dest[1] = *(src+1);
				dest[2] = *(src+0);
				dest[3] = *(src+3);
				dest += 4;
				src += 4;
			}
			else
			{
				dest[0] = src[0];
				dest[1] = src[1];
				dest[2] = src[2];
				dest += 3;
				src += 3;
			}
		}
	}
	pkImage->unlock();
	goto noerror;
error:
	X_SharedPtr(iSystem)->getLogger().print(LL_INFO,"_pngLoader: Cannot load image : %s",pkFile->getFileName());
noerror:
	png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
	return pkImage;
}
