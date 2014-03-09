/* 
 * File:   PngImageLoader.cpp
 * Author: obhi
 * 
 * Created on May 8, 2011, 4:57 AM
 */
#include "BaseHeaders.h"
#include "PNGImageCodec.h"
#include "png.h"

#define PNG_BYTES_TO_CHECK 4

namespace ImageLoader {

	PNGImageCodec PNGImageCodec::codec;
	PNGImageCodec::PNGImageCodec() {
	}

	PNGImageCodec::~PNGImageCodec() {
	}

	void PngReadFile(png_structp png_ptr, png_bytep data, png_size_t length) {
	    if (!((InputStream*) png_get_io_ptr(png_ptr))->Read((void*) data, (uint32) length))
	        png_error(png_ptr, "-read error");
	}

	void PngWarn(png_structp png_ptr, png_const_charp msg) {
	    LogManager::Instance().LogMsg(LOG_WARNING, "PngImageLoader", msg);
	}

	bool CheckIfPng(InputStream *readr) {
	    uint8 buf[PNG_BYTES_TO_CHECK];
	    // Read in some of the signature bytes
	    if (!readr->Read(buf, PNG_BYTES_TO_CHECK))
	        return 0;
	    // Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	    // Return nonzero (true) if they match
	    return png_sig_cmp((png_bytep) buf, (png_size_t) 0, PNG_BYTES_TO_CHECK) == 0;
	}

	bool PNGImageCodec::TryLoad(InputStreamPtr& file, const ImageParams& params) {
		char data[PNG_BYTES_TO_CHECK];
		file->Read(data, PNG_BYTES_TO_CHECK);
	    return png_sig_cmp((png_bytep) data, (png_size_t) 0, PNG_BYTES_TO_CHECK) == 0;
	}

	typedef enum {
	    GrayAlpha,
	    Palleted,
	    RGBImage
	} ImageType;

	ImageData PNGImageCodec::Load(InputStreamPtr& file, const ImageParams& params) {
	    InputStream* readr = file.GetPtr();
	    // check if png_ptr
	    if (!CheckIfPng(readr)) {
	        Warn(String("Image file format is not png: ") + params.name);
	    }

	    png_structp png_ptr;
	    png_infop info_ptr;
	    png_uint_32 width, height;
	    int bit_depth, color_type;
	    bool alpha;
	    Color32 keycolor;
	    int keycolor_index = -1;
	    bool hascolorkey = false;

	    ImageType imagetype;

	    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	    if (png_ptr == NULL) {
	        Error("Could not create read struct");
	        NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	    }

	    info_ptr = png_create_info_struct(png_ptr);
	    if (info_ptr == NULL) {
	        png_destroy_read_struct(&png_ptr, NULL, NULL);
	        Error("Could not create info struct");
	        NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	    }

	    png_set_error_fn(png_ptr, (void *) (readr), PngWarn, PngWarn);
	    png_set_read_fn(png_ptr, (void *) (readr), PngReadFile);

	    png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
	    png_read_info(png_ptr, info_ptr);
	    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	            0, NULL, NULL);

	    // todo Unstrip
	    png_set_strip_16(png_ptr);

	    if (bit_depth != 8 && bit_depth != 16) {
	        Error("Unsupported bit depth");
	        NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	    }

	    switch (color_type) {
	        case PNG_COLOR_TYPE_GRAY:
	        case PNG_COLOR_TYPE_GRAY_ALPHA:
	            if (color_type & PNG_COLOR_MASK_ALPHA)
	                imagetype = GrayAlpha;
	            else {
	                imagetype = Palleted;
	                png_set_strip_alpha(png_ptr);
	                if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
	                    png_color_16p trans_values;
	                    png_get_tRNS(png_ptr, info_ptr, 0, 0, &trans_values);
	                    hascolorkey = true;
	                    keycolor.red = uint8(trans_values->gray) & 0xff;
	                    keycolor.green = uint8(trans_values->gray) & 0xff;
	                    keycolor.blue = uint8(trans_values->gray) & 0xff;
	                    keycolor.alpha = uint8(trans_values->gray) & 0xff;
	                }

	            }
	            break;
	        case PNG_COLOR_TYPE_PALETTE:
	            imagetype = Palleted;
	            alpha = true;
	            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
	                // tRNS chunk. Every palette entry gets its own alpha value.
	                png_bytep trans;
	                int num_trans;
	                png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, 0);

	                // see if there is a single entry w/ alpha==0 and all other 255.
	                // if yes use keycolor transparency.
	                bool only_binary_trans = true;
	                for (int32 i = 0; (i < num_trans) && only_binary_trans; i++) {
	                    if (trans[i] != 0xff) {
	                        only_binary_trans = only_binary_trans && (trans[i] == 0)
	                                && (keycolor_index == -1);
	                        keycolor_index = i;
	                    }
	                }
	                if (!only_binary_trans) {
	                    keycolor_index = -1;
	                    png_set_palette_to_rgb(png_ptr);
	                    png_set_tRNS_to_alpha(png_ptr);
	                    imagetype = RGBImage;
	                } else
	                    alpha = false;
	            } else
	                alpha = false;

	            break;
	        case PNG_COLOR_TYPE_RGB:
	        case PNG_COLOR_TYPE_RGBA:
	            alpha = true;
	            imagetype = RGBImage;
	            if (!(color_type & PNG_COLOR_MASK_ALPHA)) {
	                alpha = false;
	                if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
	                    png_color_16p trans_values;
	                    png_get_tRNS(png_ptr, info_ptr, 0, 0, &trans_values);
	                    hascolorkey = true;
	                    keycolor.red = uint8((trans_values->red) & 0xff);
	                    keycolor.green = uint8((trans_values->green) & 0xff);
	                    keycolor.blue = uint8((trans_values->blue) & 0xff);
	                }
	            }
	            break;
	    }

	    // setup gamma
	    //@ TODO: add code to retrieve the gamma here
	    double screen_gamma = 0; //X_SharedPtr(iSystem)->getUserGamma();
	    // and comment this
	    screen_gamma = 2.2;

	    int intent;

	    if (png_get_sRGB(png_ptr, info_ptr, &intent))
	        png_set_gamma(png_ptr, screen_gamma, 0.45455);
	    else {
	        double image_gamma;
	        if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
	            png_set_gamma(png_ptr, screen_gamma, image_gamma);
	        else
	            png_set_gamma(png_ptr, screen_gamma, 0.45455);
	    }

	    /* todo Unstrip this */
	    if (bit_depth > 8) {
	        // tell libpng to strip 16 bit/color files down to 8 bits/color
	        png_set_strip_16(png_ptr);
	        bit_depth = 8;
	    } else if (bit_depth < 8)
	        // Expand pictures with less than 8bpp to 8bpp
	        png_set_packing(png_ptr);

	    // update
	    png_read_update_info(png_ptr, info_ptr);
	    png_uint_32 bytes_per_row = png_get_rowbytes(png_ptr, info_ptr);


	    //png_read_image
	    PixelFormat fmt;
	    uint32 bpp = 1;
	    // we dont support anything else
	    // other than RGBImage right now
	    // so!

	    if (imagetype == RGBImage) {
	        fmt = PixelFormat::RGBA8;
	        bpp = 4;
	    } else if (imagetype == GrayAlpha && bytes_per_row == 1)
	        fmt = PixelFormat::A8;
	    else { // pallete
	        // cant handle this
	        Warn(String("PixelFormat not supported.") + params.name);
	        NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	    }


	    int numPass = png_set_interlace_handling(png_ptr);

	    ImageData img;
	    uint8* imgDest = (uint8*) NEX_ALLOC(bpp * height * width, MEMCAT_GENERAL);
	    img.data = imgDest;
	    img.format = fmt;
	    img.depth = 1;
	    img.height = (uint16) height;
	    img.width = (uint16) width;
	    img.numMipMaps = 1;
	    img.numFaces = 1;
	    img.totalMipMapCount = 1;

	    uint8* imgRows = 0;
	    if (numPass > 1) {
	        imgRows = (uint8*) NEX_ALLOC(bytes_per_row * height, MEMCAT_GENERAL);
	    } else
	        imgRows = (uint8*) NEX_ALLOC(bytes_per_row, MEMCAT_GENERAL);

	    uint32 srcBpp = bytes_per_row / width;

	    for (uint32 i = 0; (int32)i < numPass; ++i) {
	        for (uint32 y = 0; y < height; y++) {
	            uint8* rowPtr = numPass > 1 ?
	                    imgRows + bytes_per_row * y : imgRows;
	            png_read_row(png_ptr, rowPtr, NULL);

	            // write only once
	            if (i == numPass - 1) {
	                for (uint32 x = 0; x < width; x++) {
	                    // bgra
	                    switch (fmt) {
	                        case PixelFormat::RGBA8:

	                            *(imgDest++) = *(rowPtr++);
	                            *(imgDest++) = *(rowPtr++);
	                            *(imgDest++) = *(rowPtr++);
	                            if(alpha)
	                                *(imgDest++) = *(rowPtr++);
	                            else
	                                *(imgDest++) = 0xff;
	                            break;
	                        case PixelFormat::A8:
	                            *(imgDest++) = *(rowPtr++);
	                            break;
	                    }
	                    imgDest += bpp;
	                    rowPtr += srcBpp;
	                }
	            }
	        }
	    }

	    png_read_end(png_ptr, (png_infop) 0);
	    png_destroy_read_struct(&png_ptr, NULL, NULL);
	    NEX_FREE(imgRows, MEMCAT_GENERAL);

	    if (hascolorkey) {
	        imgDest = (uint8*)img.data;

	        for (uint32 y = 0; y < height; y++) {
	            for (uint32 x = 0; x < width; x++) {
	                switch (fmt) {
	                    case PixelFormat::RGBA8:
	                        if (imgDest[0] == keycolor.red &&
	                            imgDest[1] == keycolor.green &&
	                            imgDest[2] == keycolor.blue)
	                            imgDest[3] = 0;
	                        break;
	                    case PixelFormat::A8:
	                        if( imgDest[0] == keycolor.alpha )
	                            imgDest[0] = 0;
	                        break;
	                }
	                imgDest += bpp;
	            }

	        }
	    }

	    return img;
	}
}
