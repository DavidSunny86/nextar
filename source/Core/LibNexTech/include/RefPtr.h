#ifndef NEXTAR_SHARED_PTR_H
#define NEXTAR_SHARED_PTR_H

/**--------------------------------------------------------------------------------------------------
 * \file	RefPtr.h
 *
 * @brief	Defines shared pointers used as smart pointers across nextar.
 *-----------------------------------------------------------------------------------------------**/
#include "NexBase.h"
#include "Disposer.h"
#include "SharedObjectTracker.h"
#include "NexThread.h"

namespace nextar {

	/**
	 * @class   ScopedPtr
	 *
	 * @remarks This class can be used to implement a managed
	 *          pointer which is disposed when the object goes
	 *          out of scope.
	 **/
	template<typename T, typename Disposer = DisposerDelete<T> >
	class ScopedPtr: public AllocGeneral {
		T* scopedPtr;

	private:
		ScopedPtr(const ScopedPtr& p);

	public:
		inline ScopedPtr() :
				scopedPtr(0) {
		}

		inline ScopedPtr(T* obj) :
				scopedPtr(obj) {
		}

		inline ~ScopedPtr() {
			Clear();
		}

		inline void Clear() {
			if (scopedPtr)
				Disposer::Dispose(scopedPtr);
			scopedPtr = 0;
		}

		inline T * operator =(T* obj) {
			Clear();
			scopedPtr = obj;
			return obj;
		}

		inline operator T*() {
			return scopedPtr;
		}

		inline operator const T*() const {
			return scopedPtr;
		}

		inline T * operator ->() {
			return scopedPtr;
		}

		inline	const T * operator ->() const {
			return scopedPtr;
		}
	};

	/**
	 * @class	Referenced
	 *
	 * @brief	Base class for reference counted object classes,
	 *          this should only be used for classes not supporting
	 *          iUnknown but still requires shareability.
	 * @par     Counters will be MT safe.
	 * @author	Abhishek Dey
	 * @date	11/28/2009
	 **/
	template<typename T, typename BaseClass>
	class Referenced: public BaseClass {
	dbg_BreakFeature();
	private:
		std::atomic_int_fast32_t refCount;
	public:

		Referenced() {
			dbg_Ctor(refCount);
			refCount = 1;
		}

		virtual ~Referenced() {
			dbg_Dtor();
		}

		inline int32 GetRefCount() const {
			return refCount;
		}

		inline void AddRef() {
			refCount++;
			dbg_IncRef();
		}

		inline void Release() {
			NEX_ASSERT(refCount > 0);
			dbg_DecRef();
			if (--(refCount) == 0)
			NEX_DELETE (static_cast<T*>(this));
		}
	};

	/**
	 * @class	RefPtr
	 *
	 * @brief	Reference pointer, classes which support reference
	 *          counting by basically having an IncRef and DecRef
	 *          method can use this class to implement sharable
	 *          resources.
	 *
	 * @author	Abhishek Dey
	 * @date	11/28/2009
	 **/
	template<typename T>
	class RefPtr: public AllocGeneral {
		T* object;
	public:

		inline RefPtr() :
				object(0) {
		}

		/*	RefPtr(T* obj) : object((obj))
		 {
		 }*/

		inline RefPtr(const RefPtr<T>& p) :
				object(p.object) {
			if (object)
				object->AddRef();
		}

		//template <typename T2>
		//RefPtr(T2* obj) : object(static_cast<T*>(obj))
		//{
		//}

		template<typename T2>
		inline RefPtr(const RefPtr<T2>& p) :
				object(static_cast<T*>(const_cast<T2*>(p.GetPtr()))) {
			if (object)
				object->AddRef();
		}

		inline RefPtr & operator =(const RefPtr& p) {
			if (p.GetPtr() != object) {
				Clear();
				object = const_cast<T*>(p.GetPtr());
				if (object)
					object->AddRef();
			}
			return *this;
		}

		template<typename T2>
		inline RefPtr & operator =(const RefPtr<T2>& p) {
			if (p.GetPtr() != static_cast<T2*>(object)) {
				Clear();
				object = static_cast<T*>(const_cast<T2*>(p.GetPtr()));
				if (object)
					object->AddRef();
			}
			return *this;
		}

		inline ~RefPtr() {
			Clear();
		}

		inline operator T*() {
			return object;
		}

		inline T * operator ->() {
			NEX_ASSERT(object);
			return object;
		}

		inline T* GetPtr() {
			return object;
		}

		inline operator const T*() const {
			return object;
		}

		inline const T * operator ->() const {
			NEX_ASSERT(object);
			return object;
		}

		inline const T* GetPtr() const {
			return object;
		}

		inline int32 GetRefCount() const {
			return object ? object->GetRefCount() : -1; // invalid call
		}

		/**
		 * @remarks This will not Add a reference to the pointer.
		 *          Usefull and should be used to bind pointers
		 *          that are returned by allocators or Create
		 *          methods.
		 **/
		inline
		void Assign(T* ptr) {
			Clear();
			NEX_ASSERT(ptr);
			object = ptr;
		}

		template<typename T2>
		inline void Assign(T2* ptr) {
			Clear();
			NEX_ASSERT(ptr);
			object = static_cast<T*>(ptr);
		}

		/** @remarks This will Add a reference to the pointer.
		 *          Usefull and should be used to bind pointers
		 *          that are not returned by allocators or Create
		 *          methods.
		 **/
		inline
		void Bind(T* ptr) {
			Clear();
			NEX_ASSERT(ptr);
			object = ptr;
			object->AddRef();
		}

		inline
		bool IsNotNull() const {
			return object != 0;
		}

		inline
		bool IsNull() const {
			return object == 0;
		}

		inline operator bool() const {
			return object != 0;
		}

		inline
		bool operator !() const {
			return object == 0;
		}

		inline
		void Clear() {
			if (object) {
				NEX_ASSERT(object->GetRefCount() > 0);
				object->Release();
				object = 0;
			}
		}

	public:

		friend inline bool operator ==(const RefPtr<T>& p1,
				const RefPtr<T>& p2) {
			return (p1.GetPtr() == p2.GetPtr()) != 0;
		}

		friend inline bool operator !=(const RefPtr<T>& p1,
				const RefPtr<T>& p2) {
			return (p1.GetPtr() != p2.GetPtr()) != 0;
		}

		//friend inline void Swap(RefPtr<T>& p1,RefPtr<T>& p2) {
		//	T* p = p1.object;
		//	p1.object = p2.object;
		//	p2.object = p;
		//}
	};

	/** Assign the pointer to smart pointer and add
	 * an extra reference.
	 */
	template<typename T>
	inline RefPtr<T> Bind(T* what) {
		RefPtr<T> w;
		w.Bind(what);
		return w;
	}

	/** Just assign the pointer to smart pointer without adding
	 * any extra reference.
	 */
	template<typename T>
	inline RefPtr<T> Assign(T* what) {
		RefPtr<T> w;
		w.Assign(what);
		return w;
	}

}

#endif //NEXTAR_SHARED_PTR_H
