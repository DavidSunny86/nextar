

#include <xmemory.h>
#include <malloc.h>

// **************** Allocation **************** //
X_Api void*	allocMemory(size_t size)			  // aligned alloc
{	
	return malloc(size); 
}
// **************** Allocation **************** //

// **************** FreeMemory **************** //
X_Api void	freeMemory(void* ptr) // aligned free
{	
	free(ptr); 
}
// **************** FreeMemory **************** //

// **************** AllocMemoryAligned **************** //
X_Api void*	allocMemoryAligned(size_t size)			  // aligned alloc
{
#ifdef _MSC_VER
	return _aligned_malloc(size,16);
#else
	return malloc(size);
#endif
}
// **************** AllocMemoryAligned **************** //

// **************** FreeMemoryAligned **************** //
X_Api void	freeMemoryAligned(void* ptr) // aligned free
{
#ifdef _MSC_VER
	// free only if allocated by allocMemory
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}
// **************** FreeMemoryAligned **************** //

// **************** ReallocMemory **************** //
X_Api void* reallocMemory(void* cur,size_t size)
{
	//realloc
	X_Assert(size!=0);
	return realloc(cur,size);
}
// **************** ReallocMemory **************** //

// **************** ReallocMemoryAligned **************** //
X_Api void* reallocMemoryAligned(void* cur,size_t size)
{
	//realloc
	X_Assert(size!=0);
#ifdef _MSC_VER
	return _aligned_realloc(cur, size, 16);
#else
	return reallocMemory(cur,size);
#endif
}
// **************** ReallocMemoryAligned **************** //
