
//@ Abhishek Dey
//@ September 03 2007
#ifndef NEXTAR_RECT_H
#define NEXTAR_RECT_H

#include <xbasictypes.h>

namespace nextar
{

struct coord2_x : public coord2
{
	coord2_x( const i2d _x, const i2d _y ) { x = _x; y = _y; }
	coord2_x( ) { x = y = 0; }
	coord2_x( const i2d c ) { x = y = c; }
	coord2_x( const coord2_x &c ) { x = c.x; y = c.y; }

	_n_inline	bool operator == ( const coord2_x& other ) const { return x == other.x && y == other.y; }
	_n_inline	bool operator != ( const coord2_x& other ) const { return x != other.x || y != other.y; }
	_n_inline	const coord2_x& operator	+=	( const coord2_x& other ) { x += other.x; y += other.y; return *this; }
	_n_inline	const coord2_x& operator	-=	( const coord2_x& other )	{ x -= other.x; y -= other.y;	return *this; }
	_n_inline	coord2_x operator - (	const coord2_x& other ) const { return coord2_x(x-other.x, y-other.y); }
	_n_inline	coord2_x operator + (	const coord2_x& other ) const	{ return coord2_x(x+other.x, y+other.y);	}
	_n_inline	const coord2_x& operator = ( const coord2_x& other ) { x=other.x;y=other.y;	return *this;	}
};

struct size2_x : public size2
{
	size2_x( ) { dx=dy=0; }
	size2_x( const i2d ldx, const i2d ldy ) { dx=ldx; dy=ldy; }
	size2_x( const i2d ld ) { dx=dy=ld; }
	size2_x( const size2_x &d ) { dx=d.dx; dy=d.dy; }

	_n_inline bool operator == (const size2_x& o) const	{	return bool(dx == o.dx && dy == o.dy); }
	_n_inline bool operator != (const size2_x& o) const	{	return bool(dx != o.dx || dy != o.dy); }
	_n_inline bool operator ! () const { return bool( !dx && !dy ); }
	_n_inline const size2_x& operator = (const size2_x& o) { dx = o.dx; dy = o.dy;	return *this; }
	_n_inline const size2_x operator - (const size2_x& o) { return size2_x( dx - o.dx, dy - o.dy ); }
	_n_inline const size2_x operator + (const size2_x& o) { return size2_x( dx + o.dx, dy + o.dy ); }
};

struct rect2_x : public rect2
{
	_n_inline rect2_x() { }
	_n_inline rect2_x( const rect2_x& rc ) {
		left = rc.left; 
		top = rc.top;
		right = rc.right;
		bottom = rc.bottom;
	}
	_n_inline rect2_x(i2d left, i2d top, i2d right, i2d bottom)  { 
		this->left = left; 
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
	_n_inline rect2_x( const coord2_x& lt, const coord2_x &rb ) { 
		this->left = lt.x; 
		this->top = lt.y;
		this->right = rb.x;
		this->bottom = rb.y;
	}
	_n_inline rect2_x( const coord2_x &pos, const size2_x &size ) {
		this->left = pos.x; 
		this->top = pos.y; 
		this->right = pos.x + size.dx; 
		this->bottom = pos.y + size.dy;
	}

	_n_inline const rect2_x& operator += (	const coord2_x& pos	) {	
		left += pos.x; right += pos.x;
		top += pos.y; bottom += pos.y;
		return *this;
	}
	_n_inline const rect2_x& operator -= (	const coord2_x& pos	) {	
		left -= pos.x; right -= pos.x;
		top -= pos.y; bottom -= pos.y;
		return *this;
	}
	_n_inline bool operator == (const rect2_x& r ) const	{	
		return bool(left==r.left&&right==r.right&&top==r.top&&bottom==r.bottom);
	}
	_n_inline bool operator != (const rect2_x& r ) const	{	
		return bool(left!=r.left||right!=r.right||top!=r.top||bottom!=r.bottom);
	}
	_n_inline const i2d operator [] (const s32 i) const {
		return *(&left + i);
	}
	_n_inline i2d& operator [] (const s32 i) {
		return *(&left + i);
	}
	_n_inline const rect2_x& operator = ( const rect2_x& rc )
	{
		left = rc.left; 
		top = rc.top;
		right = rc.right;
		bottom = rc.bottom;
		return *this;
	}
	// returns width of rectangle.
	_n_inline i2d width() const	{		return right - left;	}
	// returns height of rectangle.
	_n_inline i2d height() const	{		return bottom - top;	}
	// return dimn
	_n_inline size2_x dimension() const	{	return size2_x( right - left, bottom - top );	}
	// pointinside
	_n_inline bool isInside( i2d x, i2d y ) const	{
		return bool( left <= x && top <= y && right >= x && bottom >= y );
	}
	//  test rectangle intersection with another rectangle.
	_n_inline bool isInside(const rect2_x& r) const	{
		return bool(bottom > r.top && top < r.bottom &&
			right > r.left && left < r.right);
	}
	// set this rect2_x as intersection area of two rects
	_n_inline void setAsIntersectionArea( const rect2_x& r1, const rect2_x& r2 )
	{
		left = __max( r1.left, r2.left );
		top = __max( r1.top, r2.top );
		right = __min( r1.right, r2.right );
		bottom = __min( r1.bottom, r2.bottom );
	}
	// clip this rectangle with another one. This rect2_x gets modified by 'r'
	_n_inline void clip(	const rect2_x& r	) 
	{
		if (r.right < right)
			right = r.right;
		if (r.bottom < bottom)
			bottom = r.bottom;
					
		if (r.left > left)
			left = r.left;
		if (r.top > top)
			top = r.top;
		
		if (top > bottom)
			top = bottom;
		if (left > right)
			left = right;
	}
	// check if this rect2_x is corrected
	_n_inline bool isOk() const
	{			
		return !bool( (right - left) <= 0 || (bottom - top) <= 0 );
	}

	// repair invalid rect2_x
	_n_inline void repair()
	{
		if (right < left)
		{
			i2d t = right;
			right = left;
			left = t;
		}
					
		if (bottom < top)
		{
			i2d t = bottom;
			bottom = top;
			top = t;
		}
	}
	
	// get the center of the rectangle
	_n_inline coord2_x center() const
	{
		return coord2_x( ( left + right ) / 2,	( top + bottom ) / 2 );
	}
};

}

#endif //NEXTAR_RECT_H