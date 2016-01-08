
#include "cTextureFactory.h"
#include <stdio.h>
typedef unsigned char boolean;

extern "C" {
#include "jpeglib/jconfig.h"
#include "jpeglib/jpeglib.h"
}
using namespace nextar;

void	init_source (j_decompress_ptr cinfo);
boolean fill_input_buffer (j_decompress_ptr cinfo);
void	skip_input_data (j_decompress_ptr cinfo, long num_bytes);
void	resync_to_restart (j_decompress_ptr cinfo, long desired);
void	term_source (j_decompress_ptr cinfo);

cSoftwareTexture* cTextureFactory::_jpgLoader(iFileReader *pkFile)
{
	X_Assert(pkFile->getCurrentSize());
	pkFile->seek(0,true);

	::byte* input = new ::byte[ pkFile->getCurrentSize() ];
	pkFile->readData( input, pkFile->getCurrentSize() );

	struct jpeg_decompress_struct cinfo;

	// allocate and initialize JPEG decompression object
	struct jpeg_error_mgr jerr;

	//We have to set up the error handler first, in case the initialization
	//step fails.  (Unlikely, but it could happen if you are out of memory.)
	//This routine fills in the contents of struct jerr, and returns jerr's
	//address which we place into the link field in cinfo.

	cinfo.err = jpeg_std_error(&jerr);
	// Now we can initialize the JPEG decompression object. 
	jpeg_create_decompress(&cinfo);

	// specify data source
	jpeg_source_mgr jsrc;

	// Set up data pointer
	jsrc.bytes_in_buffer = pkFile->getCurrentSize();
	jsrc.next_input_byte = (JOCTET*)input;
	cinfo.src = &jsrc;

	jsrc.init_source = init_source;
	jsrc.fill_input_buffer = fill_input_buffer;
	jsrc.skip_input_data = skip_input_data;
	jsrc.resync_to_restart = jpeg_resync_to_restart;
	jsrc.term_source = term_source;

	// Decodes JPG input from whatever source
	// Does everything AFTER jpeg_create_decompress
	// and BEFORE jpeg_destroy_decompress
	// Caller is responsible for arranging these + setting up cinfo

	// read file parameters with jpeg_read_header() 
	(void) jpeg_read_header(&cinfo, TRUE);

	// Start decompressor
	(void) jpeg_start_decompress(&cinfo);

	// Get image data
	u16 rowspan = cinfo.image_width * cinfo.num_components;
	unsigned width = cinfo.image_width;
	unsigned height = cinfo.image_height;

	bool has_alpha= false;  //(JPEG never has alpha)
	bool greyscale;

	if (cinfo.jpeg_color_space == JCS_GRAYSCALE)
		greyscale = true;
	else
		greyscale = false;

	if ( greyscale )
	{
		delete [] input;
		jpeg_destroy_decompress(&cinfo);
		ReportBug("Greyscale .jpg textures are not supported.", "cTextureFactory::_jpgLoader" );
		return 0;
	}

	// Allocate memory for buffer
	::byte *output = new ::byte[rowspan * height];

	// Here we use the library's state variable cinfo.output_scanline as the
	// loop counter, so that we don't have to keep track ourselves.
	// Create array of row pointers for lib
	::byte **rowPtr = new ::byte * [height];

	for( unsigned i = 0; i < height; i++ )
		rowPtr[i] = &output[ i * rowspan ];

	unsigned rowsRead = 0;

	while( cinfo.output_scanline < cinfo.output_height )
		rowsRead += jpeg_read_scanlines( &cinfo, &rowPtr[rowsRead], cinfo.output_height - rowsRead );

	delete [] rowPtr;
	// Finish decompression

	(void) jpeg_finish_decompress(&cinfo);

	// Release JPEG decompression object

	// This is an important step since it will release a good deal of memory.
	jpeg_destroy_decompress(&cinfo);

	// convert image
	cSoftwareTexture* image = cSoftwareTexture::create( CF_R8G8B8, size2_x(width, height), output, true );
	image->canDeleteExternalPtr(true);

	delete [] input;

	return image;
}

void init_source (j_decompress_ptr cinfo)
{
}

boolean fill_input_buffer (j_decompress_ptr cinfo)
{
	return 1;
}

void skip_input_data (j_decompress_ptr cinfo, long count)
{
	jpeg_source_mgr * src = cinfo->src;
	if(count > 0)
	{
		src->bytes_in_buffer -= count;
		src->next_input_byte += count;
	}
}

void resync_to_restart (j_decompress_ptr cinfo, long desired)
{
}

void term_source (j_decompress_ptr cinfo)
{
}
