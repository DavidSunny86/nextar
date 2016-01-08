//@ Abhishek Dey
//@	12/25/2006		AT:		5:39:13 PM
//@	1/27/2007		AT:		6:49:19 PM

#include <stdio.h>
#include <string.h>
#include <cstr.h>
#include <stdarg.h>

namespace nextar
{

//[default ctor]
cStr::cStr() : marrChars(new tchar[1]),miStrLength(0),mnAllocated(0)
{
	marrChars[0] = 0;
}

/************************************
./ cStr::~cStr	
./ Decscription: dtor
./ Return type:	
************************************/
cStr::~cStr()
{
	delete [] marrChars;
}

/************************************
./ cStr::operator =	
./ Decscription: assignment, assembly alternate
./ Return type:	
./ param: const tchar* x	[]:		
************************************/
const cStr& cStr::operator = ( const tchar* x )
{
	if (!x)
	{
		if (!marrChars)
		{
			marrChars = new tchar[1];
			mnAllocated = 1;
			miStrLength = 1;
		}
		*marrChars = 0x0;
		return *this;
	}

	if(marrChars) delete [] marrChars;

	/*block1: use this if unsure of byte order
	s32* alc = &this->mnAllocated;
	s32* len = &this->miStrLength;
	*/

#if !defined( UNICODE ) && X_USE_ASM_ALTERNATIVES >= 2 && defined(_MSC_VER)
	__asm
	{
		
		mov		eax, 0						; initialize the eax register to 0
		mov		ecx, x						; set the pointer location
loopthru1:									; loop and check for null '\0'	
		movzx	edx, byte ptr[ecx + eax]
		inc		eax
		cmp		edx, 0
		jne		loopthru1
		
		mov		edx, this					; check out for BYTEORDER
		mov		[edx+8], eax				; this->mnAllocated = eax
		dec		eax
		mov		[edx+4], eax				; this->miStrLength = eax
	
		; to correct byte order remove the above lines and the commented block1 and use this
		; mov	[alc], eax
		; dec	eax
		; mov	[len], eax
	}
	
	
	marrChars = new s8[mnAllocated];

	// copy them down
	// kinda strcpy
	__asm
	{
		mov		edx,	this		; check out for BYTEORDER
		mov		ecx,	[edx]
		mov		edx,	x

loopthru2:
		movzx	eax,	byte ptr[edx]
		mov		byte ptr[ecx], al
		add		ecx, 1
		add		edx, 1
		cmp		eax, 0
		jne		loopthru2
	}

#else

	// DO UNICODE SPECEFIC STUFF
	s32 length = 0;

	while(x[length])
		++length;

    miStrLength = length;
	mnAllocated = length+1;

	marrChars = new tchar[miStrLength];

	for (s32 l = 0; l<length+1; ++l)
		marrChars[l] = x[l];

#endif

	return *this;
}

/************************************
./ cStr::operator = 	
./ Decscription: assignmenr
./ Return type:	const cStr& 
./ param: const cStr& str	[]:		
************************************/
const cStr& cStr::operator = ( const cStr& str )
{
	if(marrChars) delete [] marrChars;
	marrChars = 0;
	
	miStrLength = str.strlen();
	mnAllocated = miStrLength + 1;
	if(!miStrLength) return *this;

	marrChars = new s8[mnAllocated];

#if !defined( UNICODE ) && X_USE_ASM_ALTERNATIVES >= 2 && defined(_MSC_VER)

	const s8* str_ = str.c_str();
	s8*	str2 = marrChars;
	// copy them down
	__asm
	{
		mov		ecx,	str2
		mov		edx,	str_

loopthru3:
		movzx	eax,	byte ptr[edx]
		mov		byte ptr[ecx], al
		inc		ecx
		inc		edx
		cmp		eax, 0
		jne		loopthru3
	}
#else
	const tchar* str_ = str.c_str();
	// simply copy
	for (s32 l = 0; l<=miStrLength; ++l)
		marrChars[l] = str_[l];
#endif
	return *this;
}

/************************************
./ cStr::operator == 	
./ Decscription: returns 1 if both cStr are equivalent
./ Return type:	bool 
./ param: const cStr& str	[]:		
************************************/
bool cStr::operator == ( const cStr& str ) const
{
	const s8* strc1 = str.c_str(), *strc2 = marrChars;

#if !defined( UNICODE ) && X_USE_ASM_ALTERNATIVES >= 2 && defined(_MSC_VER)

		__asm
		{
			mov		edx,	strc1
			mov		ecx,	strc2

loop1:
			movzx	eax, byte ptr[edx]
			cmp		al, byte ptr[ecx]
			jne		exitfalse
			cmp		eax, 0
			je		exittrue
			inc		ecx
			inc		edx
			jmp		loop1
		}

exitfalse:
			return false;
exittrue:
			return true;
#else

	for( ;*strc1 == *strc2 && *strc1 && *strc2; strc1++, strc2++ );
	if( *strc1 != *strc2 ) return false;
	return true;
#endif
}

/************************************
./ cStr::operator == 	
./ Decscription: returns 1 if both cStr are equivalent
./ Return type:	bool 
./ param: const tchar* str	[]:		
************************************/
bool cStr::operator == ( const tchar* strc1 ) const
{
	const s8* strc2 = marrChars;

#if !defined( UNICODE ) && X_USE_ASM_ALTERNATIVES >= 2 && defined(_MSC_VER)

		__asm
		{
			mov		edx,	strc1
			mov		ecx,	strc2

loop1:
			movzx	eax, byte ptr[edx]
			cmp		al, byte ptr[ecx]
			jne		exitfalse
			cmp		eax, 0
			je		exittrue
			inc		ecx
			inc		edx
			jmp		loop1
		}

exitfalse:
			return false;
exittrue:
			return true;
#else

	for( ;*strc1 == *strc2 && *strc1 && *strc2; strc1++, strc2++ );
	if( *strc1 != *strc2 ) return false;
	return true;
#endif
}

/************************************
./ cStr::operator < 	
./ Decscription:	
./ Return type:	bool  
./ param: const cStr&	[]:		
************************************/
bool cStr::operator < ( const cStr& str ) const
{
	for( s32 i = 0; marrChars[i] && str.marrChars[i]; ++i )
		if ( marrChars[i] != str.marrChars[i] )
			return (marrChars[i] < str.marrChars[i]);
	return miStrLength < str.miStrLength;
}

// op = float
const cStr& cStr::operator =(const f32 val)
{
	reserve(64);
	_stprintf_s(marrChars,64,"%e",val);
	_verifyLength();
	return *this;
}
// op = int
const cStr& cStr::operator =(const s32 val)
{
	reserve(20);
	_ltot_s(val,marrChars,20,10);
	_verifyLength();
	return *this;
}
// op = unsigned int
const cStr& cStr::operator =(const u32 val)
{
	reserve(20);
	_ultot_s(val,marrChars,20,10);
	_verifyLength();
	return *this;
}

/************************************
./ cStr::makeLower	
./ Decscription: convert to lower case
./ Return type:	void 
************************************/
void cStr::makeLower()
{
	for (s32 i=0; i<miStrLength; ++i)
		if ( marrChars[i] >= 'A' && marrChars[i] <= 'Z' )
			marrChars[i] |= 0x20;
}

/************************************
./ cStr::makeUpper	
./ Decscription: convert to upper case	
./ Return type:	void 
************************************/
void cStr::makeUpper()
{
	for (s32 i=0; i<miStrLength; ++i)
		if ( marrChars[i] >= 'a' && marrChars[i] <= 'z' )
			marrChars[i] &= ~0x20;
}

/************************************
./ cStr::noCaseCompare	
./ Decscription: 
./ Return type:	bool 
./ param: const cStr &str	[]:		
************************************/
bool cStr::noCaseCompare( const cStr &str ) const
{
	for(s32 i=0; marrChars[i] && str.marrChars[i]; ++i)
	{
		if( marrChars[i] != str.marrChars[i] )
		{
			if( (( marrChars[i] >= 'a' && marrChars[i] <= 'z' ) ||
				 ( marrChars[i] >= 'A' && marrChars[i] <= 'Z' )) )
			{
				 if( (marrChars[i]|0x20) == (str.marrChars[i]|0x20) ) 
					 continue;
			}
			 return false;
		}
	}
	return miStrLength == str.miStrLength;
}

/************************************
./ cStr::compareN	
./ Decscription: compare first n character of a cStr
./ Return type:	bool 
./ param:  const cStr& other	[]:		
./ param: s32 len	[]:		
************************************/
bool cStr::compare( const cStr& other, s32 len ) const
{
	s32 i = 0;
	for( ; marrChars[i] && other.marrChars[i] && i < len; ++i )
			if (marrChars[i] != other.marrChars[i])
				return false;

	return (i == len) || (miStrLength == other.miStrLength);
}

/************************************
./ cStr::compareN	
./ Decscription: compare first n character of a cStr
./ Return type:	bool 
./ param:  const tchar* other	[]:		
./ param: s32 len	[]:		
************************************/
bool cStr::compare( const tchar* other, s32 len ) const
{
	s32 i=0;
	for( ; marrChars[i] && other[i] && i < len; ++i )
			if (marrChars[i] != other[i])
				return false;

	return (i == len) || (!marrChars[i] && !other[i]);
}

/************************************
./ cStr::append	
./ Decscription:	
./ Return type:	void  
./ param: const tchar c	[]:		
************************************/
void  cStr::append( const tchar character )
{
	if (miStrLength + 1 >= mnAllocated) 
		reserve(miStrLength + miStrLength/4 + 1);

	marrChars[miStrLength] = character;
	marrChars[++miStrLength] = '\0';
}

// need do edit later

//------------------------------------------------------------------------------
// append
//------------------------------------------------------------------------------
void cStr::append( const cStr& other )
{
	s32 len = other.strlen();
		
	if ( miStrLength + len >= mnAllocated )
		reserve(miStrLength + len);

	for (s32 l=0; l<=len; ++l)
		marrChars[l+miStrLength] = other.marrChars[l];

	miStrLength = miStrLength + len;
}

//------------------------------------------------------------------------------
// append
//------------------------------------------------------------------------------
void cStr::append( const tchar* other )
{

	s32 len = 0;
	const tchar *tmp = other;
	while( *tmp )
	{
		*tmp++;len++;
	}
		
	if ( miStrLength + len >= mnAllocated )
		reserve(miStrLength + len);

	for (s32 l=0; l<=len; ++l)
		marrChars[l+miStrLength] = other[l];

	miStrLength = miStrLength + len;
}

//------------------------------------------------------------------------------
// append
//------------------------------------------------------------------------------
void cStr::append( const cStr& other, s32 length )
{
	if (other.strlen() <= length)
	{
		append(other);
		return;
	}
		
	if (miStrLength + length >= mnAllocated )
		reserve(miStrLength + length);

	for (s32 l=0; l<length; ++l)
		marrChars[l+miStrLength] = other.marrChars[l];

	miStrLength = miStrLength + length;
	*(marrChars + miStrLength) = '\0';
}

//------------------------------------------------------------------------------
// append
//------------------------------------------------------------------------------
void cStr::append( const tchar* other, s32 length )
{
	if (::_tcslen(other) <= (u32)length)
	{
		append(other);
		return;
	}

	if (miStrLength + length >= mnAllocated )
		reserve(miStrLength + length);

	for (s32 l=0; l<length; ++l)
		marrChars[l+miStrLength] = other[l];

	miStrLength = miStrLength + length;
	*(marrChars + miStrLength) = '\0';
}

//------------------------------------------------------------------------------
// findchar
//------------------------------------------------------------------------------
s32 cStr::findchar( const tchar t, s32 offset )
{
	if( offset >= miStrLength ) return -1;
	s32 i = offset;
	for( ; i < miStrLength && marrChars[i] != t; i++ );
	return i==miStrLength ? -1 : i;
}

//------------------------------------------------------------------------------
// strpos
//------------------------------------------------------------------------------
s32 cStr::strpos( const tchar *str ) const
{
	s32 j1, j2, i;
	s32 l = (s32)::_tcslen( str );
	for( i = 0; i + l <= miStrLength; i++ )
		for( j1 = i, j2 = 0; j2 <= l && marrChars[j1] == str[j2]; j1++, j2++ )
			if( j2 == l ) return (i);
	return (-1);
}

//------------------------------------------------------------------------------
// strpos
//------------------------------------------------------------------------------
s32 cStr::strpos( const cStr &str ) const
{
	s32 j1, j2, i;
	s32 l = str.strlen();
	for( i = 0; i + l <= miStrLength; i++ )
		for( j1 = i, j2 = 0; j2 <= l && marrChars[j1] == str.marrChars[j2]; j1++, j2++ )
			if( j2 == l ) return (i);
	return (-1);
}

//------------------------------------------------------------------------------
// substr
//------------------------------------------------------------------------------
void cStr::substr( tchar* str, s32 i, s32 j ) const
{
	if( i >= miStrLength ) return;
	if( i + j >= miStrLength ) j = miStrLength - i;
	s32 m = 0;
	for( s32 k = i; m < j; str[m++] = marrChars[k++] );
	str[m] = '\0';
}

//------------------------------------------------------------------------------
// substr
//------------------------------------------------------------------------------
void cStr::substr( cStr& str, s32 i, s32 j ) const
{
	X_BreakOutIf(i<0);
	if( i >= miStrLength ) return;
	if( i + j >= miStrLength ) j = miStrLength - i;
	str.reserve(j);
	s32 m = 0;
	for( s32 k = i ; m < j; str.marrChars[m++] = marrChars[k++] );
	str.marrChars[m] = '\0';
	str.miStrLength = m;
}

//------------------------------------------------------------------------------
// replace
//------------------------------------------------------------------------------
void cStr::replace( const tchar toReplace, const tchar replaceWith )
{
	for ( s32 i=0; i<miStrLength; ++i )
		if (marrChars[i] == toReplace)
			marrChars[i] = replaceWith;
}

//------------------------------------------------------------------------------
// insertAt
//------------------------------------------------------------------------------
void cStr::insert( s32 idx, const tchar c )
{
	if( idx < 0 ) idx = 0;
	if( idx >=miStrLength ) idx = miStrLength;

	if( miStrLength + 1 >= mnAllocated )
		reserve(miStrLength+1);

	miStrLength++;
	tchar last = marrChars[idx];	
	marrChars[idx] = c;
	
#if !defined( UNICODE ) && X_USE_ASM_ALTERNATIVES >= 2 && defined(_MSC_VER)

	s8* str2 = marrChars;
	s32   l   = miStrLength;

	__asm
	{
		mov		eax,	[idx]
		inc		eax
		mov		ecx,	str2
		mov		edx,	0
		mov		dl,		[last]
cmp_eax_l:
		cmp		eax,	[l]
		jge		exitloop
		xchg	dl,		byte ptr[ecx + eax]
		inc		eax
		jmp		cmp_eax_l
exitloop:
	}

#else

	for( s32 i = idx+1; i < miStrLength; i++ )
	{
		tchar t = last;
		last = marrChars[i];
		marrChars[i] = t;
	}

#endif

	marrChars[miStrLength] = '\0';
}

 //------------------------------------------------------------------------------
// insertAt
//------------------------------------------------------------------------------
void cStr::insert( s32 idx, const cStr& str )
{
	if( idx < 0 ) idx = 0;
	if( idx >=miStrLength ) idx = miStrLength;

	const tchar* t = str.c_str();
	s32 ll = miStrLength + str.strlen();

	if( ll >= mnAllocated )
		reserve( ll );

	s32 i = miStrLength;
	for( s32 last = ll; last >= idx; i--, last-- )
		marrChars[last] = marrChars[i];

	for( i = 0; i < str.strlen(); i++ )
		marrChars[i+idx] = t[i];

	miStrLength = ll;
}

/************************************
./ cStr::erase	
./ Decscription: erase a s8
************************************/
void cStr::erase( s32 index )
{
	X_BreakOutIf( index>=miStrLength || index<0 );
	
	for (s32 i=index+1; i<miStrLength; ++i)
		marrChars[i-1] = marrChars[i];

	marrChars[--miStrLength] = '\0'; 
}

// a number of chars
void cStr::erase( s32 start, s32 end )
{
	X_BreakOutIf( start < 0 || end < 0 );
	X_BreakOutIf( start>=miStrLength || end>miStrLength || start > end );
	
	s32 erased_char = end-start;
	for (s32 i=end; i<=miStrLength; ++i)
		marrChars[start++] = marrChars[i];
	miStrLength -= erased_char;
}

/************************************
./ cStr::trim	
./ Decscription:	
./ Return type:	void 
************************************/
void cStr::trim()
{
	s32 begin = -1;
	s32 end = -1;
	for( s32 i = 0; i < miStrLength; ++i )
	{
		if( marrChars[i] != ' ' && marrChars[i] != '\t' && marrChars[i] != '\n' )
			/* trim found */
		{
			begin = i;	break;
		}
	}

	if( begin == -1 ) 
	{ 
		marrChars[0] = '\0'; 
		miStrLength = 0;
		return;
	}

	for( s32 i = miStrLength-1; i >= 0; --i )
	{
		if( marrChars[i] != ' ' && marrChars[i] != '\t' && marrChars[i] != '\n' )
			/* trim found */
		{
			end = i; break;
		}
	}

	if( begin == 0 )
	{
		if( end <= miStrLength-2 )
			marrChars[end+1] = '\0';
		miStrLength = end+1;
		return;
	}

	s32 i = 0;
	for( ; begin <= end; begin++, i++ )
		marrChars[i] = marrChars[begin];
	marrChars[i] = '\0';
	miStrLength = i;
}

/************************************
./ cStr::crop	
./ Decscription: crops and stores in buff
************************************/
void cStr::crop( s32 b, s32 length )
{
	if( b < 0 ) b = 0;
	s32 tmp = b + length;
	if( tmp > miStrLength ) tmp = miStrLength;

	s32 i = b;
	for( ; i < miStrLength && tmp < miStrLength; i++, tmp++ )
		marrChars[i] = marrChars[tmp];
	marrChars[i] = '\0';
	miStrLength = i;
}

/* format print */
void cStr::format( const tchar* str, s32 maxchar, ... )
{
	if( maxchar >= mnAllocated )
		reserve(maxchar);

	va_list	val;
	va_start( val, maxchar );
	_vsntprintf_s( marrChars, mnAllocated, _TRUNCATE, str, val );
	_verifyLength();
}

// replace
void cStr::replace(const tchar replace_char, s32 start, s32 end)
{
	X_Assert(end<=miStrLength);
	X_Assert(start<miStrLength);
	X_Assert(start<=end);
	if(end==start)
		insert(start,replace_char);
	else
	{
		s32 tmp = end;
		marrChars[start] = replace_char;
		for(s32 i=start+1;i < end;i++)
			marrChars[i] = marrChars[tmp++];
		miStrLength -= (end-start);
		marrChars[miStrLength] = 0;
	}
}

/************************************
./ cStr::_reallocate	
./ Decscription:	
./ Return type:	void 
./ param: s32 new_size	[]:		
************************************/
void cStr::_reallocate( s32 new_size )
{
	tchar *old_arrChars = marrChars;
	
	marrChars = new tchar[new_size+1];
	X_BreakOutIf(!marrChars);
	mnAllocated = new_size;

	s32 amount = miStrLength < new_size ? miStrLength : new_size-1;

	for (s32 i=0; i<amount; ++i)
		marrChars[i] = old_arrChars[i];
	marrChars[amount] = 0;

	if (mnAllocated < miStrLength+1)
		miStrLength = mnAllocated-1;
		
	delete [] old_arrChars;
}

/* TODO: Look for more hash funcs in the net */
X_Api u32 getHashKeyForStringA( const char* sStr )
{
	u32 uHashVal = X_Err_Index;
	u32 uTmp;
	while( *sStr )
	{
		uHashVal = ( uHashVal << 4 ) + *sStr;
		if( (uTmp = uHashVal & 0xf0000000) )
			uHashVal ^= (uTmp >> 24);
		uHashVal &= ~uTmp;
		sStr++;
	}
	return uHashVal;
}

X_Api u32 getHashKeyForStringW( const wchar_t* sStr )
{
	u32 uHashVal = X_Err_Index;
	u32 uTmp;
	while( *sStr )
	{
		uHashVal = ( uHashVal << 8 ) + *sStr;
		if( (uTmp = uHashVal & 0xff000000) )
			uHashVal ^= (uTmp >> 20);
		uHashVal &= ~uTmp;
		sStr++;
	}
	return uHashVal;
}

/* returns the pointer where no whitespace [' ','\n','\t'] is encountered */
X_Api const tchar* skipWhiteSpace( const tchar* p )
{
	if ( !p || !*p )
		return 0;

	while ( p && *p )
	{
		if ( isspace(*p) || *p == '\n' || *p =='\r' )
			++p;
		else
			break;
	}

	return p;
}

void cStr::_verifyLength()
{
	miStrLength = 0;
	while(marrChars[miStrLength]) miStrLength++;
}

/*
X_Api const bool strCompare( const tchar* sStr1, const tchar* sStr2, bool bCaseSensitive )
{
	X_BreakOutIf( !sStr1 );
	X_BreakOutIf( !sStr2 );

	if (bCaseSensitive)
	{
		
		while ( *sStr1 && *sStr2 && *sStr1 == *sStr2 )
		{	++sStr1;	++sStr2;	}
		if ( *sStr2 == 0 && *sStr1 == 0 )
			return true;
	}
	else
	{
		while ( *sStr1 && *sStr2 && tolower( *sStr1 ) == tolower( *sStr2 ) )
		{	++sStr1;	++sStr2;	}
		if ( *sStr2 == 0 && *sStr1 == 0 )
			return true;
	}
	return false;
}
*/

/*
//------------------------------------------------------------------------------
// findCharFirst
//------------------------------------------------------------------------------
s32 cStr::findCharFirst( const tchar c ) const
{
	for ( s32 i=0; i<miStrLength; ++i )
		if ( marrChars[i] == c )
			return i;
	return -1;
}

//------------------------------------------------------------------------------
// findCharInListFirst
//------------------------------------------------------------------------------
s32 cStr::findCharInListFirst( const tchar* c, s32 count ) const
{
	for ( s32 i=0; i<miStrLength; ++i )
		for ( s32 j=0; j<count; ++j )
			if ( marrChars[i] == c[j] )
				return i;
	return -1;
}

//------------------------------------------------------------------------------
// findCharNotInListFirst
//------------------------------------------------------------------------------
s32 cStr::findCharNotInListFirst( const tchar* c, s32 count ) const
{
	for (s32 i=0; i<miStrLength; ++i)
	{
		for ( s32 j=0; j<count; ++j )
			if (marrChars[i] == c[j])
				break;

		if (j==count)
			return i;
	}

	return -1;
}

//------------------------------------------------------------------------------
// findCharFirst
//------------------------------------------------------------------------------
s32 cStr::findCharLast( const tchar c ) const
{
	for ( s32 i=miStrLength-1; i>=0; --i )
		if ( marrChars[i] == c )
			return i;
	return -1;
}

//------------------------------------------------------------------------------
// findCharInListFirst
//------------------------------------------------------------------------------
s32 cStr::findCharInListLast( const tchar* c, s32 count ) const
{
	for ( s32 i=miStrLength-1; i>=0; --i )
		for ( s32 j=0; j<count; ++j )
			if ( marrChars[i] == c[j] )
				return i;
	return -1;
}

//------------------------------------------------------------------------------
// findCharNotInListFirst
//------------------------------------------------------------------------------
s32 cStr::findCharNotInListLast( const tchar* c, s32 count ) const
{
	for (s32 i=miStrLength-2; i>=0; --i)
	{
		for ( s32 j=0; j<count; ++j )
			if (marrChars[i] == c[j])
				break;

		if (j==count)
			return i;
	}

	return -1;
}

//------------------------------------------------------------------------------
// findCharNext
//------------------------------------------------------------------------------
s32 cStr::findCharNext( const tchar c, s32 startPos ) const
{
	for (s32 i=startPos; i<miStrLength; ++i)
		if (marrChars[i] == c)
			return i;
		return -1;
}
*/

}