#ifndef NEXTAR_SINGLETON_H
#define NEXTAR_SINGLETON_H

#ifndef NEXTAR_TYPES_BASE_H
#error include NexTypes.h rather
#endif
/*! \file Singleton.h
 *  
 *  @author Abhishek Dey 
 *  
 *  @brief 
 *  
 *  
 */

namespace nextar {

//! helper macro
#define NEX_DEFINE_SINGLETON_PTR(__class__name)   \
    template <> __class__name* Singleton<__class__name>::sSingletonPtr = 0

	/** @brief Top level class with selfptrs: members of these
	 *         classes can be accessed by a static GetSingleton function
	 *         you cannot change the value of this member by any means
	 **/
	template<typename T>
	class Singleton {
		static T* sSingletonPtr;
	public:

		_NexInline
		static bool IsValidPtr() {
			return sSingletonPtr != 0;
		}

		_NexInline static T* InstancePtr() {
			return sSingletonPtr;
		}

		_NexInline static T& Instance() {
			NEX_ASSERT(sSingletonPtr);
			return *sSingletonPtr;
		}

		_NexInline Singleton() {
			sSingletonPtr = static_cast<T*>(this);
		}

		_NexInline ~Singleton() {
			sSingletonPtr = 0;
		}
	};

}
#endif //NEXTAR_SINGLETON_H
