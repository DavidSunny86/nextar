#ifndef NEXTAR_NEXTHREAD_H
#define NEXTAR_NEXTHREAD_H

#include <BaseTypes.h>

namespace nextar {

class spinlock_mutex
{
	std::atomic_flag flag;
public:
	spinlock_mutex() {
		flag.clear(std::memory_order_release);
	}

	void lock()	{
		while(flag.test_and_set(std::memory_order_acquire));
	}

	void unlock() {
		flag.clear(std::memory_order_release);
	}

	bool try_lock() {
		return !flag.test_and_set(std::memory_order_acquire);
	}
};

struct NullMutex {
	void lock() {
	}
	void unlock() noexcept {
	}
	bool try_lock() {
		return true;
	}
};

typedef std::mutex Mutex;
typedef std::recursive_mutex RecursiveMutex;
typedef std::thread::id ThreadID;
typedef std::atomic_int_fast32_t atomic_int;
typedef std::atomic_uint_fast32_t atomic_uint;
typedef std::atomic_flag atomic_flag;
typedef std::atomic_bool atomic_bool;

}

#define NEX_LOCK_VARIABLE(name) lock_##__LINE__##name
#define NEX_THREAD_ID() std::this_thread::get_id()

#ifdef NEX_SUPPORT_MULTITHREADING
#	define NEX_THREAD_MUTEX(mutexName) Mutex mutexName
#	define NEX_THREAD_RECURSIVE_MUTEX(mutexName) RecursiveMutex mutexName
#	define NEX_THREAD_MUTEX_LOCK(mutexName) mutexName.lock()
#	define NEX_THREAD_MUTEX_TRY_LOCK(mutexName) mutexName.try_lock()
#	define NEX_THREAD_MUTEX_UNLOCK(mutexName) mutexName.unlock()
#	define NEX_THREAD_LOCK_GUARD_MUTEX(mutexName) std::lock_guard<Mutex> NEX_LOCK_VARIABLE(guard)(mutexName)
#	define NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(mutexName) std::lock_guard<RecursiveMutex> NEX_LOCK_VARIABLE(guard)(mutexName)
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