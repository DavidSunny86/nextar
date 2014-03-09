/*
 * RadixSort.h
 *
 *  Created on: 26-Jan-2014
 *      Author: obhi
 */

#ifndef RADIXSORT_H_
#define RADIXSORT_H_

#include "NexBase.h"

namespace nextar {

	/** @reference DrDoob Radix Sort implementation
	 * Sort based on 32/64 bit integers * */
	template<typename _It, const uint32 Threshold, typename Integer>
	inline void RadixSort( _It* a, uint32 size, Integer bitMask, uint32 shiftRightAmount )	{
		const uint32 numberOfBins = 256;
		// initially stores the count
		uint32 endOfBin[ numberOfBins ];
		std::memset(endOfBin, 0, sizeof(endOfBin));

		for ( uint32 _current = 0; _current <= size; _current++ )	{
			uint32 digit = (uint32)(( a[ _current ] & bitMask ) >> shiftRightAmount );
			// extract the digit we are sorting based on
			endOfBin[ digit ]++;
		}

	   uint32 startOfBin[ numberOfBins ], nextBin;
	   startOfBin[ 0 ] = endOfBin[ 0 ] = nextBin = 0;
	   for( uint32 i = 1; i < numberOfBins; i++ )
	            startOfBin[ i ] = startOfBin[ i - 1 ] + endOfBin[ i - 1 ];
	   std::memcpy(endOfBin, startOfBin, sizeof(endOfBin));

	   for ( uint32 _current = 0; _current <= size; ) {
	           unsigned long digit;
	           _It tmp = a[ _current ];
	           // take tmp and put it in its place, iteratively
	           // swapping it until done
	           while ( true ) {
	        	   // extract the digit we are sorting based on
	        	   digit = (unsigned long)(( tmp & bitMask ) >> shiftRightAmount );
	        	   if ( endOfBin[ digit ] == _current )
	        		   break;
	        	   std::swap( tmp, a[ endOfBin[ digit ] ] );
	        	   endOfBin[ digit ]++;
	           }

	           a[ _current ] = tmp;
	           endOfBin[ digit ]++;   // leave the element at its location and grow the bin
	           _current++;  // advance the current pointer to the next element
	           while( _current >= startOfBin[ nextBin ] && nextBin < numberOfBins )
	        	   nextBin++;
	           while( endOfBin[ nextBin - 1 ] == startOfBin[ nextBin ] && nextBin < numberOfBins )
	        	   nextBin++;
	           if ( _current < endOfBin[ nextBin - 1 ] )
	        	   _current = endOfBin[ nextBin - 1 ];
	   }
	   bitMask >>= 8;
	   if ( bitMask != 0 ) {
		   shiftRightAmount -= 8;
		   for( uint32 i = 0; i < numberOfBins; i++ ) {
			   uint32 numberOfElements = endOfBin[ i ] - startOfBin[ i ];
			   if ( numberOfElements >= Threshold )
				   RadixSort< _It, Threshold, Integer >(
						   &a[ startOfBin[ i ]], numberOfElements - 1, bitMask, shiftRightAmount );
	          else if ( numberOfElements >= 2 ) {
	        	  auto begin = &a[ startOfBin[ i ]];
	        	  std::sort( begin, begin + numberOfElements );
	          }
	      }
	   }
	}

	template<typename _It>
	inline void RadixSortInt32( _It* a, uint32 size)	{
		RadixSort<_It, 32, uint32>(a, size, 0xff000000, 24);
	}
}

#endif /* RADIXSORTER_H_ */
