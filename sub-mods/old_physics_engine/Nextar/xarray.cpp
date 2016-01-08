
#include <array_t.h>

namespace nextar
{

void array_t::push_back(const array_t::alloc_type element)
{
	if (muUsed + 1 > muAllocated)
	{
		array_t::alloc_type e;
		e = element;           // copy element
		reserve(muUsed + 2); // increase mpkData block
		mpkData[muUsed++] = e;        // push_back
		mbSorted = false; 
		return;
	}
	
	mpkData[muUsed++] = element;
	mbSorted = false;
}

void array_t::push_front(const array_t::alloc_type element)
{
	if (muUsed + 1 > muAllocated)
		reserve(muUsed + 2);
	
	for (index_type i=muUsed; i>0; --i)
		mpkData[i] = mpkData[i-1];
	
	mpkData[0] = element;
	mbSorted = false;
	++muUsed;
}

void array_t::insert(const array_t::alloc_type element, array_t::index_type index) 
{
	X_BreakOutIf(index>muUsed); // access violation
	
	if (muUsed + 1 > muAllocated)
		reserve(muUsed + 2);

	for (index_type i=muUsed++; i>index; i--) 
		mpkData[i] = mpkData[i-1];
	
	mpkData[index] = element;
	mbSorted = false;
}

void array_t::resize(array_t::alloc_type usedNow)
{
	if (muAllocated < usedNow)
		reserve(usedNow);
	muUsed = usedNow;
}

const array_t& array_t::operator=(const array_t& other)
{
	if (mpkData)
		delete [] mpkData;
	
	//if (muAllocated < other.muAllocated)
	if (other.muAllocated == 0)
		mpkData = 0;
	else
		mpkData = new array_t::alloc_type[other.muAllocated];

	muUsed = other.muUsed;
	mbSorted = other.mbSorted;
	muAllocated = other.muAllocated;
	for (array_t::alloc_type i=0; i<other.muUsed; ++i)
		mpkData[i] = other.mpkData[i];
	return *this;
}

void array_t::sort()
{
	if (mbSorted || muUsed<2)
		return;
	
	/* heap sort */
	register s32 s,f;
	register array_t::alloc_type elt;
	register array_t::alloc_type ivalue;
	
	/* creating heap */
	for( s32 i = 1; i < (s32)muUsed; i++ )
	{
		elt = mpkData[i];
		/* pqinsert */
		s = i;
		f = (s-1)/2;
		while( s > 0 && mpkData[f] < elt )
		{
			mpkData[s] = mpkData[f];
			s = f;
			f = (s-1)/2;
		} /* end while */
		mpkData[s] = elt;
	} /* end for */
	
	/* selection phase : select mpkData[0] and find it its proper position */
	for( s32 i = (s32)muUsed-1; i > 0; i-- )
	{
		/* pqmaxdelete */
		ivalue = mpkData[i];
		mpkData[i] = mpkData[0];
		f = 0;
		/* s = largeson( 0, i-1 ) */
		if( i == 1 )
			s = -1;
		else
			s = 1;
		if( i > 2 && mpkData[2] > mpkData[1] )
			s = 2;
		while( s >= 0 && ivalue < mpkData[s] ) 
		{
			mpkData[f] = mpkData[s];
			f = s;
			/* s = largeson( f, i-1 ) */
			s = 2*f + 1;
			if( s+1 <= i-1 && mpkData[s] < mpkData[s+1] )
				s = s+1;
			if( s > i-1 )
				s = -1;
		}/* end while */
		mpkData[f] = ivalue;
	}/* end for */
	mbSorted = true;
}

s32 array_t::binary_search(const array_t::alloc_type element, s32 left, s32 right)
{
	if (!muUsed)
		return -1;
	sort();
	s32 m;
	do
	{
		m = (left+right)>>1;
		if (element < mpkData[m])
			right = m - 1;
		else
			left = m + 1;
		
	} while((element < mpkData[m] || mpkData[m] < element) && left<=right);
	
	if (!(element < mpkData[m]) && !(mpkData[m] < element))
		return m;
	return -1;
}

s32 array_t::linear_search( const array_t::alloc_type element )
{
	for (array_t::alloc_type i=0; i<muUsed; ++i)
		if (!(element < mpkData[i]) && !(mpkData[i] < element))
			return (s32)i;
		return -1;
}

s32 array_t::linear_reverse_search( const array_t::alloc_type element )
{
	for (s32 i=(s32)muUsed-1; i>=0; --i)
		if (mpkData[i] == element)
			return i;
		return -1;
}

void array_t::erase( const array_t::index_type index )
{
	X_BreakOutIf(index>=muUsed || index<0); // access violation
	
	for (array_t::index_type i=index+1; i<muUsed; ++i)
		mpkData[i-1] = mpkData[i];
	--muUsed;
}

void array_t::erase( const array_t::index_type index, const s32 count )
{
	X_BreakOutIf(index>=muUsed || count<1 || index+count>muUsed); // access violation
		
	for (array_t::index_type i=index+count; i<muUsed; ++i)
		mpkData[i-count] = mpkData[i];
		
	muUsed-= count;
}


void array_t::clear() 
{
	if(mpkData) { delete [] mpkData;	mpkData = 0; }
	muUsed = muAllocated = 0;
	mbSorted = true;
}

void array_t::reserve( array_t::index_type uSize )
{
	X_BreakOutIf(!uSize);
	array_t::alloc_type* pkOldData = mpkData;
	
	mpkData = new array_t::alloc_type[uSize];
	muAllocated = uSize;
	
	index_type end = muUsed < uSize ? muUsed : uSize;
	for (index_type i=0; i<end; ++i)
		mpkData[i] = pkOldData[i];
	
	if (muAllocated < muUsed)
		muUsed = muAllocated;
	
	if( pkOldData ) delete [] pkOldData;
}

}