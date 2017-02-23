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
#include <NexAssert.h>

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
enum MemoryCategory {
	//! Indicates basic memory catagory.
	MEMCAT_GENERAL,
	//! Memory catagory cache aligned
	MEMCAT_CACHEALIGNED,
	//! Memory catagory general SIMD
	MEMCAT_SIMDALIGNED,
	//! Memory catagory math core
	MEMCAT_MATH_CORE,
	//! Cpu loaded vertex/index/texture data
	MEMCAT_BUFFER_DATA,
};

/**! @brief This class is the simple memory catagory based allocator
 *
 *   This should be specialized for pooled allocations, etc.
 */
template<enum MemoryCategory cat>
class AllocatorBase: public allocator::Default {
public:
	template <const size_t offset>
	struct Offset {
		enum : size_t {
			_value = offset
		};
	};
};

template<> class AllocatorBase<MEMCAT_CACHEALIGNED> : public allocator::Aligned<
		16> {
public:
	template <const size_t offset>
	struct Offset {
		enum : size_t {
			_value = offset + (16 - (offset % 16))
		};
	};

};

template<> class AllocatorBase<MEMCAT_SIMDALIGNED> : public allocator::Aligned<
		16> {
public:
	template <const size_t offset>
	struct Offset {
		enum : size_t {
			_value = offset + (16 - (offset % 16))
		};
	};
};

template<> class AllocatorBase<MEMCAT_MATH_CORE> : public allocator::Aligned<
	16> {
public:
	template <const size_t offset>
	struct Offset {
		enum : size_t {
			_value = offset + (16 - (offset % 16))
		};
	};
};

typedef AllocatorBase<MEMCAT_GENERAL> AllocatorGeneral;
typedef AllocatorBase<MEMCAT_CACHEALIGNED> AllocatorCacheAligned;
typedef AllocatorBase<MEMCAT_SIMDALIGNED> AllocatorSimdAligned;
typedef AllocatorBase<MEMCAT_MATH_CORE> AllocatorMathCore;
typedef AllocatorBase<MEMCAT_BUFFER_DATA> AllocatorBufferData;

/**! \include Include inside namespace nextar. */
#include <NexNew.h>

/**! Macros for allocation and deallocations */

#ifdef NEX_EXTENSIVE_MEM_TRACKER
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
#define NEX_ALLOC_INIT_T(T,catagory,...)	(ConstructObject<T>((T*)NEX_ALLOC(sizeof(T),catagory), ##__VA_ARGS__))
//! basic deallocator for objects not using AllocObjectBase
#define NEX_FREE_T(T,catagory,ptr)			DestroyObject<T>(ptr); NEX_FREE(ptr,catagory)
//! basic allocator for objects not using AllocObjectBase
#define NEX_ALLOCATOR_ALLOC_T(T,allocator)	(ConstructObject<T>((T*)NEX_ALLOCATOR_ALLOC(sizeof(T),allocator)))
//! basic allocator for objects not using AllocObjectBase
#define NEX_ALLOCATOR_ALLOC_INIT_T(T,allocator,...)	(ConstructObject<T>((T*)NEX_ALLOCATOR_ALLOC(sizeof(T),allocator), ##__VA_ARGS__))
//! basic deallocator for objects not using AllocObjectBase
#define NEX_ALLOCATOR_FREE_T(T,allocator,ptr)			DestroyObject<T>(ptr); NEX_ALLOCATOR_FREE(ptr,allocator)
//! basic allocator for objects not using AllocObjectBase
#define NEX_ALLOC_ARRAY_T(T,catagory,num)     (ConstructObjects<T>((T*)NEX_ALLOC_ARRAY(sizeof(T)*num,catagory), num))
//! basic deallocator for objects not using AllocObjectBase
#define NEX_FREE_ARRAY_T(T,catagory,num,ptr)  DestroyObjects<T>(ptr,num); NEX_FREE_ARRAY(ptr, sizeof(T)*num, catagory)
//! delete operator override
#ifdef NEX_DEBUG
#	define NEX_DELETE( statement )	{ auto _pointer = statement; NEX_ASSERT( (_pointer) ); delete _pointer; }
#	define NEX_DELETE_ARRAY( statement )	{ auto _pointer = statement; NEX_ASSERT( (_pointer) ); delete [] _pointer; }
#else
#	define NEX_DELETE( statement )	delete statement
#	define NEX_DELETE_ARRAY( statement )	delete [] statement
#endif
#	define NEX_SAFE_DELETE( pointer ) if ( pointer ) { NEX_DELETE(pointer); pointer = nullptr; }
#	define NEX_SAFE_DELETE_ARRAY( pointer ) if ( pointer ) { NEX_DELETE_ARRAY(pointer); pointer = nullptr; }
//! AllocObjectBase classes
typedef AllocObjectBase<AllocatorGeneral> AllocGeneral;
typedef AllocObjectBase<AllocatorSimdAligned> AllocSimdAligned;
typedef AllocObjectBase<AllocatorMathCore> AllocMathCore;
typedef AllocObjectBase<AllocatorCacheAligned> AllocCacheAligned;
typedef AllocObjectBase<AllocatorBufferData> AllocBufferData;

template<typename T, enum MemoryCategory category = MEMCAT_GENERAL>
class GenericFree {
public:
	inline void operator ()(T* ptr) const {
		NEX_FREE(ptr, category);
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