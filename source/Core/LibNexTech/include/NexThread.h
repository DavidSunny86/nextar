#ifndef NEXTAR_NEXTHREAD_H
#define NEXTAR_NEXTHREAD_H

#include "NexBase.h"
#include <thread>
#include <mutex>
#include <atomic>

namespace nextar {
	namespace mt {
		typedef std::mutex Mutex;
		typedef std::recursive_mutex RecursiveMutex;
		typedef std::lock_guard<std::mutex> LockGuardMutex;
		typedef std::lock_guard<std::recursive_mutex> LockGuardRecursiveMutex;
	}
}

#define NEX_LOCK_VARIABLE(name) lock_##__LINE__##name

#ifdef NEX_SUPPORT_MULTITHREADING
#	define NEX_THREAD_MUTEX(mutexName) mt::Mutex mutexName
#	define NEX_THREAD_RECURSIVE_MUTEX(mutexName) mt::RecursiveMutex mutexName
#	define NEX_THREAD_MUTEX_LOCK(mutexName) mutexName.lock()
#	define NEX_THREAD_MUTEX_TRY_LOCK(mutexName) mutexName.try_lock()
#	define NEX_THREAD_MUTEX_UNLOCK(mutexName) mutexName.unlock()
#	define NEX_THREAD_LOCK_GUARD_MUTEX(mutexName) mt::LockGuardMutex NEX_LOCK_VARIABLE(guard)(mutexName)
#	define NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(mutexName) mt::LockGuardRecursiveMutex NEX_LOCK_VARIABLE(guard)(mutexName)
#else
#	define NEX_THREAD_MUTEX(mutexName)
#	define NEX_THREAD_RECURSIVE_MUTEX(mutexName)
#	define NEX_THREAD_LOCK_GUARD_MUTEX(mutexName)
#	define NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(mutexName)
#	define NEX_THREAD_MUTEX_LOCK(mutexName)
#	define NEX_THREAD_MUTEX_TRY_LOCK(mutexName)	true
#	define NEX_THREAD_MUTEX_UNLOCK(mutexName)
#endif

#endif //NEXTAR_NEXTHREAD_H
