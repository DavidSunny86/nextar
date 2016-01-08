//@ Abhishek Dey
//@ Jan 9 2007
//@ Sep 17 2007

#include <xtypes_c.h>
#include <xcolorroutines.h>

#define A1R5G5B5(a,r,g,b) ((u16) ( ((u16)(a) << 15) | ((u16)(r) << 10) | ((u16)(g) << 5) | ((u16)(b)) ))
#define A1R5G5B5AV(_a) (u8)(  ((u16)(_a) >> 15 ))
#define A1R5G5B5RV(_r) (u8)(  (((u16)(_r) >> 10) & 0x001f  ))
#define A1R5G5B5GV(_g) (u8)(  (((u16)(_g) >> 5) & 0x001f  ))
#define A1R5G5B5BV(_b) (u8)(  (((u16)(_b)) & 0x001f ))
#define LUM_16COL(col) (u32)( ((A1R5G5B5RV(col)<<3) + (A1R5G5B5GV(col)<<3) + (A1R5G5B5BV(col)<<3)) / 3 )

#define R5G6B5(r,g,b) ((u16) ( ((u16)(r) << 11) | ((u16)(g) << 6) | ((u16)(b)) ))
#define R5G6B5RV(_r) (u8)(  (((u16)(_r) >> 11) ))
#define R5G6B5GV(_g) (u8)(  (((u16)(_g) >> 6) & 0x003f  ))
#define R5G6B5BV(_b) (u8)(  (((u16)(_b)) & 0x001f ))

#define A8R8G8B8(a,r,g,b) ( A8R8G8B8_A(((u32)a),((u32)r),((u32)g),((u32)b)) )
#define A8R8G8B8_A(a,r,g,b)  (u32)( ((a&0xff) <<24) | ((r&0xff)<<16) | ((g&0xff)<<8) | (b&0xff) )
#define A8R8G8B8AV(_a) (u8)(  ((u32)(_a) >> 24 ))
#define A8R8G8B8RV(_r) (u8)(  (((u32)(_r) >> 16) & 0x000000ff  ))
#define A8R8G8B8GV(_g) (u8)(  (((u32)(_g) >> 8) & 0x000000ff  ))
#define A8R8G8B8BV(_b) (u8)(  (((u32)(_b)) & 0x000000ff ))

#define SETALPHA8(col,alpha) { col &= 0x00ffffff; col |= ( (u32)alpha << 24); }
#define RGB16( r, g, b )  (u16)( (u32)( r & 0xF8) << 7 |	(u32)( g & 0xF8) << 2 | (u32)( b & 0xF8) >> 3 )

void xFlipMirror16B(s16* outdata,const s16* indata,s32 width,s32 height,s32 pitch)
{
	s32 x,y;
	const s32 line = width + pitch;
	outdata += width * height;

	for (y=0; y<height; ++y)
	{
		for (x=0; x<width; ++x)
		{
			--outdata;
			*outdata = *(indata + (width-x-1));
		}

		indata += line;
	}
}

void xFlipMirror24Bto32BShuffle(s32* outdata,const char* indata,s32 width,s32 height,s32 pitch)
{
	s32 x,y;
	const char *p;
	const s32 line = 3 * width + pitch;
	outdata += width * height;

	for (y=0; y<height; ++y)
	{
		for (x=0; x<width; ++x)
		{
			p = (indata + (width-x-1)*3);
			--outdata;
			*outdata = A8R8G8B8(255,p[2], p[1],p[0]);
		}

		indata += line;
	}

}

void xFlipMirror24Bto16BShuffle(s32* outdata,const char* indata,s32 width,s32 height,s32 pitch)
{
	s32 x,y;
	const char *p;
	const s32 line = 3 * width + pitch;
	outdata += width * height;

	for (y=0; y<height; ++y)
	{
		for (x=0; x<width; ++x)
		{
			char r,g,b,a;
			p = (indata + (width-x-1)*3);
			--outdata;
			
			r = (char)(((f32)p[2]/255.0f)*31.0f);
			g = (char)(((f32)p[1]/255.0f)*31.0f);
			b = (char)(((f32)p[0]/255.0f)*31.0f);
			a = (r|g|b)?1:0;
			*outdata = A1R5G5B5(a,r,g,b);
		}
		indata += line;
	}

}


void xFlipMirror32Bto32B(s32* outdata,const s32* indata,s32 width,s32 height,s32 pitch)
{
	s32 x,y;
	const s32 *p;
	const s32 line = width + pitch;
	outdata += height * width;

	for (y=0; y<height; ++y)
	{
		for (x=0; x<width; ++x)
		{
			p = indata + (width-x-1);
			--outdata;
#ifdef PPC
			{
				char	*zi = (char *)p;
				char	*zo = (char *)outdata;

				zo[0] = zi[2];
				zo[1] = zi[1];
				zo[2] = zi[0];
				zo[3] = zi[3];

			}
#else
			*outdata = *p;
#endif
		}

		indata += line;
	}
}