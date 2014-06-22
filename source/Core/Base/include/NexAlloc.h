#ifndef NEXTAR_NEXALLOC_H
#define NEXTAR_NEXALLOC_H

/*! \file NexAlloc.h
 *  @author Abhishek Dey
 *
 *  @brief
 *
 *
 */

#include <Allocators.h>
#include <new>

namespace nextar {

// basic alignment
#define NEX_ALIGN2( len ) (( (len) + 1 ) & ~1 )
#define NEX_ALIGN4( len ) (( (len) + 3 ) & ~3 )
#define NEX_ALIGN8( len ) (( (len) + 7 ) & ~7 )
#define NEX_ALIGN16( len ) (( (len) + 15 ) & ~15 )
// cd sectors
#define NEX_ALIGN2048( len ) (( (len) + 2047 ) & ~2047 )
#define NEX_ALIGN2336( len ) (( (len) + 2335 ) % 2336 )

/**
 * @enum MemoryCatagory
 *
 * @brief Memory catagories. Basic allocator and deallocator design
 **/
enum MemoryCatagory {
	//! Indicates basic memory catagory.
	MEMCAT_GENERAL,
	//! Indicates memory for scene objects
	MEMCAT_SCENE,
	//! Indicates memory for graphics objects
	MEMCAT_GRAPHICS,
	//! Indicates memory for loggers.
	MEMCAT_LOGGERS,
	//! Memory for strings used in strsets.
	MEMCAT_STRINGPOOL,
	//! Memory catagory cache aligned
	MEMCAT_CACHEALIGNED,
	//! Memory catagory general SIMD
	MEMCAT_SIMDALIGNED,
	//! Memory catagory general MMX
	MEMCAT_MMXALIGNED,
	//! Memory catagory math core
	MEMCAT_MATH_CORE,
	//! Memory catagory for components
	MEMCAT_COMPONENT,
	//! Cpu loaded vertex/index/texture data
	MEMCAT_BUFFER_DATA,
};

/**! @brief This class is the simple memory catagory based allocator
 *
 *   This should be specialized for pooled allocations, etc.
 */
template<enum MemoryCatagory cat>
class AllocatorBase: public allocator::Default {
};

template<> class AllocatorBase<MEMCAT_CACHEALIGNED> : public allocator::Aligned<
		16> {
};

template<> class AllocatorBase<MEMCAT_SIMDALIGNED> : public allocator::Aligned<
		16> {
};

template<> class AllocatorBase<MEMCAT_MMXALIGNED> : public allocator::Aligned<8> {
};

typedef AllocatorBase<MEMCAT_GENERAL> AllocatorGeneral;
typedef AllocatorBase<MEMCAT_SCENE> AllocatorScene;
typedef AllocatorBase<MEMCAT_GRAPHICS> AllocatorGraphics;
typedef AllocatorBase<MEMCAT_LOGGERS> AllocatorLogger;
typedef AllocatorBase<MEMCAT_GENERAL> AllocatorString;
typedef AllocatorBase<MEMCAT_CACHEALIGNED> AllocatorCacheAligned;
typedef AllocatorBase<MEMCAT_SIMDALIGNED> AllocatorSimdAligned;
typedef AllocatorBase<MEMCAT_MMXALIGNED> AllocatorMmxAligned;
typedef AllocatorBase<MEMCAT_MATH_CORE> AllocatorMathCore;
typedef AllocatorBase<MEMCAT_COMPONENT> AllocatorComponent;
typedef AllocatorBase<MEMCAT_BUFFER_DATA> AllocatorBufferData;

/**! \include Include inside namespace nextar. */
#include <NexNew.h>

/**! Macros for allocation and deallocations */

#if NEX_MEMTRACKERLEVEL > 1
//! basic allocator functions
#define NEX_ALLOCATOR_ALLOC(size,alloct) (alloct::Alloc(size,NEX_FUNCTION_NAME,NEX_SOURCEFILE_NAME,NEX_SOURCEFILE_LINE))
//! basic allocator functions
#define NEX_ALLOC(size,catagory) (AllocatorBase<catagory>::Alloc(size,NEX_FUNCTION_NAME,NEX_SOURCEFILE_NAME,NEX_SOURCEFILE_LINE))
//! new operator override
#define NEX_NEW( statement ) new (NEX_FUNCTION_NAME,NEX_SOURCEFILE_NAME,NEX_SOURCEFILE_LINE) statement
#else
//! basic allocator functions
#define NEX_ALLOCATOR_ALLOC(size,alloct) (alloct::Alloc(size))
//! basic allocator functions
#define NEX_ALLOC(size,catagory) AllocatorBase<catagory>::Alloc(size)
//! new operator override
#define NEX_NEW( statement )			new statement
#endif
//! basic free
#define NEX_ALLOCATOR_FREE(ptr,alloctor)		(alloctor::Free(ptr))
//! basic free
#define NEX_FREE(ptr,catagory)				(AllocatorBase<catagory>::Free(ptr))
#define NEX_FREE_ARRAY(ptr, size, catagory)	(AllocatorBase<catagory>::Free(ptr, size))
#define NEX_ALLOC_ARRAY NEX_ALLOC
//! basic allocator for objects not using AllocObjectBase
#define NEX_ALLOC_T(T,catagory)				(ConstructObject<T>((T*)NEX_ALLOC(sizeof(T),catagory)))
//! basic allocator for objects not using AllocObjectBase
#define NEX_ALLOC_INIT_T(T,catagory,init)	(ConstructObject<T>((T*)NEX_ALLOC(sizeof(T),catagory), (init)))
//! basic deallocator for objects not using AllocObjectBase
#define NEX_FREE_T(T,catagory,ptr)			DestroyObject<T>(ptr); NEX_FREE(ptr,catagory)
//! basic allocator for objects not using AllocObjectBase
#define NEX_ALLOC_ARRAY_T(T,catagory,num)     (ConstructObjects<T>((T*)NEX_ALLOC_ARRAY(sizeof(T)*num,catagory), num))
//! basic deallocator for objects not using AllocObjectBase
#define NEX_FREE_ARRAY_T(T,catagory,num,ptr)  DestroyObjects<T>(ptr,num); NEX_FREE_ARRAY(ptr, sizeof(T)*num, catagory)
//! delete operator override
#ifdef NEX_DEBUG
#	define NEX_DELETE( statement )	if ( (statement) ) delete statement
#	define NEX_DELETE_ARRAY( statement )	if ( (statement) ) delete [] statement
#else
#	define NEX_DELETE( statement )	delete statement
#	define NEX_DELETE_ARRAY( statement )	delete [] statement
#endif
//! AllocObjectBase classes
typedef AllocObjectBase<AllocatorGeneral> AllocGeneral;
typedef AllocObjectBase<AllocatorScene> AllocScene;
typedef AllocObjectBase<AllocatorGraphics> AllocGraphics;
typedef AllocObjectBase<AllocatorLogger> AllocLogger;
typedef AllocObjectBase<AllocatorString> AllocString;
typedef AllocObjectBase<AllocatorSimdAligned> AllocSimdAligned;
typedef AllocObjectBase<AllocatorMmxAligned> AllocMmxAligned;
typedef AllocObjectBase<AllocatorMathCore> AllocMathCore;
typedef AllocObjectBase<AllocatorCacheAligned> AllocCacheAligned;
typedef AllocObjectBase<AllocatorComponent> AllocComponent;
typedef AllocObjectBase<AllocatorBufferData> AllocBufferData;

template<typename T>
class GenericFree {
public:
	inline void operator ()(T* ptr) const {
		NEX_FREE(ptr, MEMCAT_GENERAL);
	}
};

template<typename T>
class GenericDelete {
public:
	inline void operator ()(T* ptr) const {
		NEX_DELETE(ptr);
	}
};
}

#include <StdAllocator.h>

#endif //NEXTAR_NEXALLOC_H