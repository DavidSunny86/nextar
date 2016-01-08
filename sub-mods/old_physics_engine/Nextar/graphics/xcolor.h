
//@ Abhishek Dey
//@ Jan 9 2007
//@ Sep 17 2007

#ifndef __SCOLOR_H__
#define __SCOLOR_H__


#include <xtypes.h>
#include <xbasictypes.h>
#include <math/xmath_c.h>

//------------------------------------------------------------------------------------------------
//macros on color manipulation
//------------------------------------------------------------------------------------------------
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

namespace nextar
{

_n_inline u32 A1R5G5B5toA8R8G8B8(u16 color)	{
		return	( color & 0x8000 ) << 16 |
				( color & 0x7C00 ) << 9 |
				( color & 0x03E0 ) << 6 |
				( color & 0x001F ) << 3;
}

_n_inline s32 R5G6B5toA8R8G8B8(s16 color)
{
	return	0xFF000000 & ((((color >> 11)&0x1F)<<19) |
			(((color >> 5)&0x3F)<<11) |	(color&0x1F)<<3);
}

enum COLORFORMAT
{
	CF_A1R5G5B5,
	CF_A8R8G8B8,
	CF_R5G6B5,
	CF_R8G8B8,
	CF_UNKNOWN
};

/* cColor */
struct cColor
{
	union {
		struct
		{
#ifdef LITTLEENDIAN
			u8 b,g,r,a;
#else		
			u8 a,r,g,b;
#endif	
		};
		/*	I'm not sure about the byte order wil work here.
			So I'm not using it.
		*/
		u32	muColor;
	};

	_n_inline cColor() : a(0),r(0),g(0),b(0) { }
	_n_inline cColor( const u8 ax, const u8 rx, const u8 gx, const u8 bx ) : a(ax), r(rx), g(gx), b(bx)	{}
	_n_inline cColor( const u8 rx, const u8 gx, const u8 bx ) : a(255), r(rx), g(gx), b(bx) {}
	_n_inline cColor( u32 col ) : a( A8R8G8B8AV(col) ) , r( A8R8G8B8RV(col) ), g( A8R8G8B8GV(col) ), b( A8R8G8B8BV(col) ) {}
	
	_n_inline u8 alpha() const { return a; }
	_n_inline u8 red() const { return r; }
	_n_inline u8 green() const { return g; }
	_n_inline u8 blue() const { return b; }
	
	_n_inline void alpha( u8 ax ) { a = ax; }
	_n_inline void red( u8 rx ) { r = rx; }
	_n_inline void green( u8 gx ) { g = gx; }
	_n_inline void blue( u8 bx ) { b = bx; }
	
	_n_inline void set( const u8 ax, const u8 rx, const u8 gx, const u8 bx ) { a = ax; r = rx; g = gx; b = bx; }
	
	_n_inline u16 toA1R5G5B5() const { return A1R5G5B5(a,r,g,b); }
	_n_inline u16 toR5G6B5() const { return R5G6B5(r,g,b); }
	_n_inline u32 toA8R8G8B8() const { return A8R8G8B8(a,r,g,b); }
	_n_inline u32 toR8G8B8() const { return A8R8G8B8(0xff,r,g,b); }
	_n_inline u32 toOpenGlColor() const { return ((a & 0xff)<<24) |	((b & 0xff)<<16) |	((g & 0xff)<<8) | (r & 0xff); }

	_n_inline cColor	operator -  ( const cColor &c ) const { return cColor( a - c.a, r - c.r, g - c.g, b - c.b ); }
	_n_inline cColor	operator +  ( const cColor &c ) const { return cColor( a + c.a, r + c.r, g + c.g, b + c.b ); }
	_n_inline cColor&	operator += ( const cColor &c ) { a += c.a; r += c.r; g += c.g; b += c.b; return *this; }
	_n_inline cColor&	operator -= ( const cColor &c ) { a -= c.a; r -= c.r; g -= c.g; b -= c.b; return *this; }
	_n_inline bool	operator == ( const cColor& second ) const { return bool( second.a == a && second.r == r && second.g == g && second.b == b ); }
	_n_inline bool	operator != ( const cColor& second ) const { return bool( second.a != a || second.r != r || second.g != g || second.b != b ); }
	_n_inline cColor&	operator *= ( const f32 factor ) { a = (u8)((f32)a*factor);r = (u8)((f32)r*factor);g = (u8)((f32)g*factor);b = (u8)((f32)b*factor); return *this; }
	_n_inline cColor&	operator =  ( const u32 c ) { a = A8R8G8B8AV(c); r = A8R8G8B8RV(c); g = A8R8G8B8GV(c); b = A8R8G8B8BV(c); return *this; }

	_n_inline void fractionOf( const cColor &sec, const f32 factor ) { 
		a = (u8)((f32)sec.a*factor);
		r = (u8)((f32)sec.r*factor);
		g = (u8)((f32)sec.g*factor);
		b = (u8)((f32)sec.b*factor); 
	}
	_n_inline void interpolate( const cColor &sec, const f32 factor ) { 
		a = a+(u8)((f32)(sec.a-a)*factor);
		r = r+(u8)((f32)(sec.r-r)*factor);
		g = g+(u8)((f32)(sec.g-g)*factor);
		b = b+(u8)((f32)(sec.b-b)*factor); 
	}
	_n_inline void interpolate( const cColor &f, const cColor &s, f32 dt ) { 
		a =  s.a + (u8)((s.a - f.a)*dt);
		r =  s.r + (u8)((s.r - f.r)*dt);
		g =  s.g + (u8)((s.g - f.g)*dt);
		b =  s.b + (u8)((s.b - f.b)*dt);
	}
}; 
/* end cColor */

//------------------------------------------------------------------------------
// cColor4
//------------------------------------------------------------------------------
struct cColor4
{
	f32 r,g,b,a;
	cColor4() {
	}

	cColor4(f32 r,f32 g,f32 b) {
		this->r = (f32)r;
		this->g = (f32)g;
		this->b = (f32)b;
		this->a = (f32)1.f;
	}

	cColor4( cColor c )  { 
		this->r = (f32)c.r / 255.f ;
		this->g = (f32)c.g / 255.f;
		this->b = (f32)c.b / 255.f;
		this->a = (f32)c.a / 255.f;
	}

	cColor4( u32 col )  { 
		this->r = (f32)A8R8G8B8RV(col) / 255.f ;
		this->g = (f32)A8R8G8B8GV(col) / 255.f ;
		this->b = (f32)A8R8G8B8BV(col) / 255.f ;
		this->a = (f32)A8R8G8B8AV(col) / 255.f ;
	}

	cColor4(const cColor4& c) {
		this->a = c.a;
		this->r = c.r;
		this->g = c.g;
		this->b = c.b;
	}
	_n_inline const cColor4& operator = (const cColor4& c) { 
		this->a = c.a;
		this->r = c.r;
		this->g = c.g;
		this->b = c.b; 
		return  *this; 
	}
	_n_inline void set(f32 r,f32 g,f32 b) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 1.0f;
	}
	_n_inline u32 reduce() { 
		return A8R8G8B8((a*255.f),(r*255.f),(g*255.f),(b*255.f));
	}
};
/* end cColor4 */

}

#endif //__SCOLOR_H__