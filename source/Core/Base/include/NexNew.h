#ifndef NEXTAR_NEXNEW_H__
#define NEXTAR_NEXNEW_H__

/*! \file NexNew.h
 *  
 *  @author Abhishek Dey 
 *  
 *  @brief 
 *  
 *  
 */

#ifndef NEXTAR_NEXALLOC_H
#error include NexAlloc.h
#endif

/**
 * @brief Construct object.
 *
 * @author Abhishek Dey
 * @date 10/28/2009
 *
 * @param [in,out] ptr If non-null, the pointer.
 *
 * @return null if it fails, else the ptr address.
 **/
template<typename T>
T* ConstructObject(T* ptr) {
	return new (ptr) T;
}

/**
 * @brief Construct objects.
 *
 * @author Abhishek Dey
 * @date 10/28/2009
 *
 * @param [in,out] ptr If non-null, the pointer.
 * @param n 		   The number of objects.
 *
 * @return null if it fails, else returns ptr.
 **/
template<typename T>
T* ConstructObjects(T* ptr, long n) {
	T* ptrr = ptr;
	while (n--)
		ConstructObject<T>(ptrr++);
	return ptr;
}

/**
 * @brief Construct object.
 *
 * @author Abhishek Dey
 * @date 10/28/2009
 *
 * @param [in,out] objptr If non-null, the objptr.
 * @param obj 			  The initializer object.
 *
 * @return null if it fails, else the ptr address.
 **/
template<typename T, typename Tc>
T* ConstructObject(T* objptr, const Tc& obj) {
	return new (objptr) T(obj);
}

/**
 * @brief Destroys the object described by ptr.
 *
 * @author Abhishek Dey
 * @date 10/28/2009
 *
 * @param [in,out] ptr If non-null, the pointer.
 **/
template<typename T>
void DestroyObject(T* ptr) {
	if (ptr)
		ptr->~T();
}

template<typename T>
void DestroyObjects(T* ptr, size_t n) {
	while (n--) {
		ptr->~T();
		ptr++;
	}
}

/**
 * @property template <typename Allocator_Class> class AllocObjectBase
 *
 * @brief  the base class for all objects.
 *
 * @return The base class for all objects that overrides new.
 **/
template<typename Allocator_Class>
class AllocObjectBase {
public:
#ifdef NEX_EXTENSIVE_MEM_TRACKER

	void* operator new(size_t s,
			const char* func,
			const char* file,
			long line) throw () {
		return Allocator_Class::Alloc(s, func, file, line);
	}

	void* operator new[](size_t s,
			const char* func,
			const char* file,
			long line) throw () {
		return Allocator_Class::Alloc(s, func, file, line);
	}

	void operator delete(void* ptr, const char*, const char*, long) throw () {
		Allocator_Class::Free(ptr);
	}

	void operator delete[](void* ptr, const char*, const char*, long) throw () {
		Allocator_Class::Free(ptr);
	}
#endif

	void* operator new(size_t, void* ptr) {
		return ptr;
	}

	void* operator new(size_t s) {
		return Allocator_Class::Alloc(s);
	}

	void* operator new[](size_t s) {
		return Allocator_Class::Alloc(s);
	}

	void operator delete(void* ptr) throw () {
		Allocator_Class::Free(ptr);
	}

	void operator delete[](void* ptr) throw () {
		Allocator_Class::Free(ptr);
	}

	void operator delete(void*, void*) {
	}
};

template<typename Allocator_Class>
class AllocPooledObjectBase {
public:
#ifdef NEX_EXTENSIVE_MEM_TRACKER

	void* operator new(size_t s,
			const char* func,
			const char* file,
			long line) throw () {
		return Allocator_Class::AllocSingle(s);
	}

	void* operator new[](size_t s,
			const char* func,
			const char* file,
			long line) throw () {
		return Allocator_Class::AllocMultiple(s);
	}

	void operator delete(void* ptr, const char*, const char*, long) throw () {
		Allocator_Class::FreeSingle(ptr);
	}

	void operator delete[](void* ptr, const char*, const char*, long) throw () {
		Allocator_Class::FreeMultiple(ptr);
	}
#endif

	void* operator new(size_t, void* ptr) {
		return ptr;
	}

	void* operator new(size_t s) {
		return Allocator_Class::AllocSingle(s);
	}

	void* operator new[](size_t s) {
		return Allocator_Class::AllocMultiple(s);
	}

	void operator delete(void* ptr) throw () {
		Allocator_Class::FreeSingle(ptr);
	}

	void operator delete[](void* ptr) throw () {
		Allocator_Class::FreeMultiple(ptr);
	}

	void operator delete(void*, void*) {
	}
};

#endif //NEXTAR_NEXNEW_H__