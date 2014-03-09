#ifndef NEXTAR_DISPOSER_H
#define NEXTAR_DISPOSER_H

#include <NexAlloc.h>

namespace nextar {
	/**
	 * @class	DisposerNull
	 *
	 * @brief	Null disposer, has no function but acts as a placeholder. 
	 *
	 * @author	Abhishek Dey
	 * @date	11/19/2009
	 **/
	template<typename T>
	class DisposerNull {
	public:
		_NexInline
		void operator ()(T*&) {
		}
		_NexInline
		static void Dispose(T*&) {
		}
	};
	/**
	 * @class	template <typename T> class DisposerDelete
	 *
	 * @brief	 The disposer using delete operator. T must be a pointer.
	 **/
	template<typename T>
	class DisposerDelete {
	public:
		_NexInline
		void operator ()(T*& obj) {
			NEX_DELETE obj;
			obj = 0;
		}
		_NexInline
		static void Dispose(T*& obj) {
			NEX_DELETE obj;
			obj = 0;
		}
	};

	/**
	 * @class	DisposerFree
	 *
	 * @brief	Disposer using free. T must be a pointer.
	 *
	 **/
	template<typename T, typename Allocator>
	class DisposerFree {
	public:
		_NexInline
		void operator ()(T*& obj) {
			NEX_ALLOCATOR_FREE(obj, Allocator);
			obj = 0;
		}
		_NexInline
		static void Dispose(T*& obj) {
			NEX_ALLOCATOR_FREE(obj, Allocator);
			obj = 0;
		}
	};

	/**
	 * @class	DisposerRelease
	 *
	 * @brief	T must be a RefPtr<T> object. 
	 *
	 * @author	Abhishek Dey
	 * @date	11/3/2009
	 **/
	template<typename T>
	class DisposerRelease {
	public:
		_NexInline
		void operator ()(T* obj) {
			obj->Release();
		}
		_NexInline
		static void Dispose(T* obj) {
			obj->Release();
		}
	};

	template<typename Disposer, typename It>
	void DisposeRange(It first, It end) {
		std::for_each(first, end, Disposer());
	}

}

#endif //
