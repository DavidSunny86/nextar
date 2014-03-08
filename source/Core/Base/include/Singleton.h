#ifndef NEXTAR_SINGLETON_H
#define NEXTAR_SINGLETON_H

#ifndef NEXTYPES_H_
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

		inline static bool IsValidPtr() {
			return sSingletonPtr != 0;
		}

		inline static T* InstancePtr() {
			return sSingletonPtr;
		}

		inline static T& Instance() {
			NEX_ASSERT(sSingletonPtr);
			return *sSingletonPtr;
		}

		inline Singleton() {
			sSingletonPtr = static_cast<T*>(this);
		}

		inline ~Singleton() {
			sSingletonPtr = 0;
		}
	};

}
#endif //NEXTAR_SINGLETON_H
