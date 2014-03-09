/* 
 * File:   JpegImageLoader.cpp
 * Author: obhi
 * 
 * Created on May 8, 2011, 2:29 AM
 */
#include <BaseHeaders.h>
#include <JPEGImageCodec.h>

extern "C" {
#include <jconfig.h>
#include <jpeglib.h>
}

namespace ImageLoader {

	JPEGImageCodec JPEGImageCodec::codec;

	struct JpegSourceManager {
	    jpeg_source_mgr pub;
	    InputStreamPtr inpSrc;
	    size_t leftToRead;
	    unsigned char readBuffer[4096];
	};

	void JpegNullFunc(j_decompress_ptr cinfo) {
	}

	boolean JpegNullFillFunc(j_decompress_ptr cinfo) {
	    return 0;
	}

	boolean JpegFillInputBuffer(j_decompress_ptr cinfo) {
	    JpegSourceManager* manager = (JpegSourceManager*) cinfo->src;
	    size_t bytesToRead = std::min<size_t>(manager->leftToRead, 4096);
	    if (manager->inpSrc->Read(manager->readBuffer, bytesToRead) == bytesToRead) {
	        manager->pub.bytes_in_buffer = bytesToRead;
	        manager->pub.next_input_byte = manager->readBuffer;
	        manager->leftToRead -= bytesToRead;
	        return 1;
	    }
	    return 0;
	}

	void JpegSkipInputData(j_decompress_ptr cinfo, long num_bytes) {
	    if (num_bytes > 0) {
	        while (num_bytes > (long) (cinfo->src->bytes_in_buffer) ) {
	            num_bytes -= (long) (cinfo->src->bytes_in_buffer);
	            cinfo->src->fill_input_buffer(cinfo);
	        }
	        cinfo->src->next_input_byte += (size_t) num_bytes;
	        cinfo->src->bytes_in_buffer -= (size_t) num_bytes;
	    }
	}


	JPEGImageCodec::JPEGImageCodec() {
	}

	JPEGImageCodec::~JPEGImageCodec() {
	}

	bool JPEGImageCodec::TryLoad(InputStreamPtr& file, const ImageParams& params) {
		char header[8];
		file->Read(header, 8);
	    // this will need a minor decompression
	    // so do that
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
	    jsrc.bytes_in_buffer = 8;
	    jsrc.next_input_byte = (JOCTET*)header;
	    jsrc.init_source = JpegNullFunc;
	    jsrc.fill_input_buffer = JpegNullFillFunc;
	    jsrc.skip_input_data = JpegSkipInputData;
	    jsrc.resync_to_restart = jpeg_resync_to_restart;
	    jsrc.term_source = JpegNullFunc;

	    cinfo.src = &jsrc;

	    // Decodes JPG input from whatever source
	    // Does everything AFTER jpeg_create_decompress
	    // and BEFORE jpeg_destroy_decompress
	    // Caller is responsible for arranging these + setting up cinfo

	    // read file parameters with jpeg_read_header()
	    int r = jpeg_read_header(&cinfo, FALSE);
	    if (r == JPEG_HEADER_OK || r == JPEG_HEADER_TABLES_ONLY)
	        return true;
	    return false;
	}

	ImageData JPEGImageCodec::Load(InputStreamPtr& stream, const ImageParams& params) {

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
	    JpegSourceManager jsrc;

	    // Set up data pointer
	    jsrc.inpSrc = stream;
	    jsrc.leftToRead = stream->GetSize();
	    jsrc.pub.bytes_in_buffer = 0;
	    jsrc.pub.next_input_byte = 0;
	    cinfo.src = &jsrc.pub;

	    jsrc.pub.init_source = JpegNullFunc;
	    jsrc.pub.fill_input_buffer = JpegFillInputBuffer;
	    jsrc.pub.skip_input_data = JpegSkipInputData;
	    jsrc.pub.resync_to_restart = jpeg_resync_to_restart;
	    jsrc.pub.term_source = JpegNullFunc;

	    // Decodes JPG input from whatever source
	    // Does everything AFTER jpeg_create_decompress
	    // and BEFORE jpeg_destroy_decompress
	    // Caller is responsible for arranging these + setting up cinfo

	    // read file parameters with jpeg_read_header()
	    (void) jpeg_read_header(&cinfo, TRUE);

	    // Start decompressor
	    (void) jpeg_start_decompress(&cinfo);

	    // Get image data
	    uint16 rowspan = cinfo.image_width * cinfo.num_components;
	    unsigned width = cinfo.image_width;
	    unsigned height = cinfo.image_height;

	    bool greyscale;

	    if (cinfo.jpeg_color_space == JCS_GRAYSCALE)
	        greyscale = true;
	    else
	        greyscale = false;

	    if (greyscale) {
	        //NextarFreea(input);
	        jpeg_destroy_decompress(&cinfo);
	        Warn(String("Greyscale not supported: ") + params.name);
	        NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	    }

	    ::byte* output = (::byte*)NEX_ALLOC(sizeof (::byte) * rowspan, MEMCAT_GENERAL);

	    // Here we use the library's state variable cinfo.output_scanline as the
	    // loop counter, so that we don't have to keep track ourselves.
	    // Create array of row pointers for lib
	    ::byte * rowPtr[1] = {output};

	    ImageData img;
	    uint8* buffer;
	    img.data = NEX_ALLOC( width * height * 4, MEMCAT_GENERAL );
	    buffer = (uint8*)img.data;
	    img.depth = 1;
	    img.format = PixelFormat::RGBA8;
	    img.height = (uint16)height;
	    img.width = (uint16)width;
	    img.numFaces = 1;
	    img.numMipMaps = 1;
	    img.totalMipMapCount = 1;

	    while (cinfo.output_scanline < cinfo.output_height) {
	        if (jpeg_read_scanlines(&cinfo, rowPtr, 1) != 1) {
	            NEX_FREE(output, MEMCAT_GENERAL);
	            NEX_FREE(img.data, MEMCAT_GENERAL);
	            img.data = 0;
	            Warn(String("Failed to read: ") + params.name);
	            NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	        }
	        JSAMPLE *j1 = rowPtr[0];
	        for (unsigned i = 0; i < cinfo.output_width; ++i) {
	            *buffer++ = *j1++;
	            *buffer++ = *j1++;
	            *buffer++ = *j1++;
	            *buffer++ =  0xff;
	        }
	    }

	    NEX_FREE(output, MEMCAT_GENERAL);
	    (void) jpeg_finish_decompress(&cinfo);
	    jpeg_destroy_decompress(&cinfo);
	    return img;
	}
}
