/*VARCHECK*/
/*FILE MARK
 DOC ORIGINAL NAME:	array_t.h
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	11/12/2006		AT:		3:46:49 PM
END
******************************************************************************************************
*/

#ifndef __XARRAY_H__
#define __XARRAY_H__

#include <xtypes.h>

namespace nextar
{

//------------------------------------------------------------------------------
// array_t
// This class is not templated, hence its usage might be usafe
// But this will avoid extra code generation.
// The requirement is typecast objects to alloc_type and save.
// Mostly objects will be either pointers or integers, other objects 
// can use std::vector, dynamic object support is not provided.
// array_t is a fundamental type hence in small letters.
//------------------------------------------------------------------------------
class array_t : public cAlignedBaseClass
{
public:
	typedef u32 alloc_type;
	typedef u32 index_type;
	protected:

		alloc_type*	mpkData;
		index_type	muAllocated;
		index_type	muUsed;
		bool		mbSorted;

	public:
				
		// ctors
		_n_inline array_t() : mpkData(0), muUsed(0), muAllocated(0), mbSorted(true)	{	}
		_n_inline array_t( index_type uStartCount )	: mpkData(0), muUsed(0), muAllocated(0), mbSorted(true) {	reserve(uStartCount); }		
		_n_inline array_t( const array_t& rkArray ) : mpkData(0) { *this = rkArray;	}
		_n_inline ~array_t()		{	if(mpkData) { delete [] mpkData;	mpkData = 0; }	}
		// ? set size to 0, without affecting the allocated space
		_n_inline void				clear_record() { muUsed = 0; }
		_n_inline void				set_used( alloc_type uUsed ) { X_BreakOutIf( uUsed > muAllocated ); muUsed = uUsed; }
		_n_inline const alloc_type&	get_last() const	{	X_BreakOutIf(!muUsed); /* access violation */	return mpkData[muUsed-1];	}	
		_n_inline alloc_type&			get_last()	{	X_BreakOutIf(!muUsed); /* access violation */	return mpkData[muUsed-1];	}
		_n_inline alloc_type*			pointer()	{	return mpkData;	}
		_n_inline const alloc_type*	const_pointer() const	{	return mpkData;	}
		_n_inline index_type			size() const	{ return muUsed; }
		_n_inline index_type			capacity() const	{	return muAllocated; }
		_n_inline bool				empty() const	{	return bool(!muUsed); }
		_n_inline void				set_sorted(bool _issorted) { mbSorted = _issorted;	}
		_n_inline void				pop() 	{	X_BreakOutIf(!muUsed); /* access violation	*/ muUsed--;	}
		_n_inline alloc_type			pop_with_return()	{ X_BreakOutIf(!muUsed); /* access violation */ muUsed--;	return mpkData[muUsed];	}
		_n_inline void				fix()	{	if( muUsed != muAllocated-1 )		{	reserve(muUsed);	}	}
		_n_inline alloc_type&			operator [] (alloc_type index)	{	X_BreakOutIf(index>=muUsed); /* access violation */ return mpkData[index]; }
		_n_inline const alloc_type&	operator [] (alloc_type index) const {	X_BreakOutIf(index>=muUsed); /* access violation */ return mpkData[index]; }
		_n_inline alloc_type&			at(index_type index)	{	X_BreakOutIf(index>=muUsed); return mpkData[index]; }
		_n_inline const alloc_type&	at(index_type index) const {	X_BreakOutIf(index>=muUsed); return mpkData[index]; }
		_n_inline s32					binary_search(const alloc_type element) {	return binary_search(element, 0, muUsed-1);	}
	
		// ? Decscription: STL type push_back
		void push_back(const alloc_type element);		
		// ? Decscription: STL type push_front
		void push_front(const alloc_type element);
		// ? Decscription: STL type insert
		void insert(const alloc_type element, index_type index=0 ) ;	
		// ? Decscription: STL type resize
		void resize(alloc_type usedNow);
		// ? Decscription: assignment
		const array_t& operator=(const array_t& other);
		// ? Decscription: Sort the array_t
		void sort();
		// ? Decscription: Binary search			
		s32 binary_search(const alloc_type element, s32 left, s32 right);
		// ? Decscription: Linear search			
		s32 linear_search( const alloc_type element );
		// ? Decscription: Linear reverse search	
		s32 linear_reverse_search( const alloc_type element );
		// ? Decscription: erase an element
		void erase( const index_type index );
		// ? Decscription: erase multiple element
		void erase( const index_type index, const s32 count );
		// ? Decscription: clear and delete allocated space
		void clear();
		// ? Decscription: reserve some space
		void reserve( index_type uSize );
};

//------------------------------------------------------------------------------
}

#endif //__XARRAY_H__















