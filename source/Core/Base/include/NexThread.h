#ifndef NEXTAR_NEXTHREAD_H
#define NEXTAR_NEXTHREAD_H

#include "BaseTypes.h"
#include <thread>
#include <mutex>
#include <atomic>

namespace nextar {
	namespace mt {

		struct NullMutex {
			void lock() {}
			void unlock() noexcept {}
			bool try_lock() {}
		};

		typedef std::mutex Mutex;
		typedef std::recursive_mutex RecursiveMutex;
		typedef std::thread::id ThreadID;
	}
}

#define NEX_LOCK_VARIABLE(name) lock_##__LINE__##name
#define NEX_THREAD_ID() std::this_thread::get_id()

#ifdef NEX_SUPPORT_MULTITHREADING
#	define NEX_THREAD_MUTEX(mutexName) mt::Mutex mutexName
#	define NEX_THREAD_RECURSIVE_MUTEX(mutexName) mt::RecursiveMutex mutexName
#	define NEX_THREAD_MUTEX_LOCK(mutexName) mutexName.lock()
#	define NEX_THREAD_MUTEX_TRY_LOCK(mutexName) mutexName.try_lock()
#	define NEX_THREAD_MUTEX_UNLOCK(mutexName) mutexName.unlock()
#	define NEX_THREAD_LOCK_GUARD_MUTEX(mutexName) std::lock_guard<mt::Mutex> NEX_LOCK_VARIABLE(guard)(mutexName)
#	define NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(mutexName) std::lock_guard<mt::RecursiveMutex> NEX_LOCK_VARIABLE(guard)(mutexName)
#	define NEX_THREAD_LOCK_GUARD_MUTEX_T(T, mutexName) std::lock_guard<T> NEX_LOCK_VARIABLE(guard)(mutexName)
#else
#	define NEX_THREAD_MUTEX(mutexName)
#	define NEX_THREAD_RECURSIVE_MUTEX(mutexName)
#	define NEX_THREAD_LOCK_GUARD_MUTEX(mutexName)
#	define NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(mutexName)
#	define NEX_THREAD_MUTEX_LOCK(mutexName)
#	define NEX_THREAD_MUTEX_TRY_LOCK(mutexName)	true
#	define NEX_THREAD_MUTEX_UNLOCK(mutexName)
#	define NEX_THREAD_LOCK_GUARD_MUTEX_T(T, mutexName)
#endif

#endif //NEXTAR_NEXTHREAD_H
