#ifndef NEXTAR_STLALLOC_H
#define NEXTAR_STLALLOC_H

#include <limits>

namespace nextar {

	template<typename T, typename _NexAllocator>
	class STLAllocator: public std::allocator<T> {
	public:
		//    typedefs

		typedef typename std::allocator<T>::pointer pointer;
		typedef typename std::allocator<T>::size_type size_type;

	public:
		//    convert an allocator<T> to allocator<U>

		template<typename U>
		struct rebind {
			typedef STLAllocator<U, _NexAllocator> other;
		};

	public:

		_NexInline explicit STLAllocator() {
		}

		_NexInline ~STLAllocator() {
		}

		/*	_NexInline explicit STLAllocator(STLAllocator const&) {}*/
		template<typename U>
		_NexInline STLAllocator(
				STLAllocator<U, _NexAllocator> const&) {
		}

		//    memory allocation

		_NexInline
		pointer allocate(size_type cnt,
				typename std::allocator<void>::const_pointer = 0) const {
			return reinterpret_cast<pointer>(NEX_ALLOCATOR_ALLOC(
					sizeof(T) * cnt, _NexAllocator));
		}

		_NexInline
		void deallocate(pointer p, size_type) const {
			NEX_ALLOCATOR_FREE(p, _NexAllocator);
		}
		//    construction/destruction

		_NexInline
		void construct(pointer p, const T& t) const {
			nextar::ConstructObject(p, t);
		}

		_NexInline
		void destroy(pointer p) const {
			nextar::DestroyObject(p);
		}
		/*

		 //    address

		 _NexInline pointer address(reference r) const {
		 return &r;
		 }

		 _NexInline const_pointer address(const_reference r) const {
		 return &r;
		 }

		 //    size

		 _NexInline size_type max_size() const {

		 return std::numeric_limits<size_type>::max() / sizeof (T);
		 }

		 _NexInline bool operator==(STLAllocator const&) const {
		 return true;
		 }

		 _NexInline bool operator!=(STLAllocator const& a) const {
		 return !operator==(a);
		 }*/
	};
//    end of class STLAllocator

	template<typename T, size_t stack_capacity, typename _NexAllocator>
	class STLStackAllocator: public STLAllocator<T, _NexAllocator> {
	public:
		typedef typename STLAllocator<T, _NexAllocator>::pointer pointer;
		typedef typename STLAllocator<T, _NexAllocator>::size_type size_type;

		struct Source {

			Source() :
					used_stack_buffer_(false) {
			}

			T * stack_buffer() {
				return reinterpret_cast<T*>(stack_buffer_);
			}

			const T * stack_buffer() const {
				return reinterpret_cast<const T*>(stack_buffer_);
			}

			char stack_buffer_[sizeof(T[stack_capacity])];
			bool used_stack_buffer_;
		};

		template<typename U>
		struct rebind {
			typedef STLStackAllocator<U, stack_capacity, _NexAllocator> other;
		};

		STLStackAllocator(
				const STLStackAllocator<T, stack_capacity, _NexAllocator>& rhs) :
				source_(rhs.source_) {
		}

		// ISO C++ requires the following constructor to be defined,
		// and std::vector in VC++2008SP1 Release fails with an error
		// in the class _Container_base_aux_alloc_real (from <xutility>)
		// if the constructor does not exist.
		// For this constructor, we cannot share storage; there's
		// no guarantee that the Source buffer of Ts is large enough
		// for Us.
		// TODO: If we were fancy pants, perhaps we could share storage
		// iff sizeof(T) == sizeof(U).

		template<typename U, size_t other_capacity>
		STLStackAllocator(
				const STLStackAllocator<U, other_capacity, _NexAllocator>& other) :
				source_(NULL) {
		}

		explicit STLStackAllocator() :
				source_(NULL) {
		}

		explicit STLStackAllocator(Source* source) :
				source_(source) {
		}

		// Actually do the allocation. Use the stack buffer if nobody has used it yet
		// and the size requested fits. Otherwise, fall through to the standard
		// allocator.

		pointer allocate(size_type n, void* hint = 0) {
			if (source_ != NULL && !source_->used_stack_buffer_
					&& n <= stack_capacity) {
				source_->used_stack_buffer_ = true;
				return source_->stack_buffer();
			} else {
				return STLAllocator<T, _NexAllocator>::allocate(n, hint);
			}
		}

		// Free: when trying to free the stack buffer, just mark it as free. For
		// non-stack-buffer pointers, just fall though to the standard allocator.

		void deallocate(pointer p, size_type n) {
			if (source_ != NULL && p == source_->stack_buffer())
				source_->used_stack_buffer_ = false;
			else
				STLAllocator<T, _NexAllocator>::deallocate(p, n);
		}

	private:
		Source* source_;
	};

// A wrapper around STL containers that maintains a stack-sized buffer that the
// initial capacity of the vector is based on. Growing the container beyond the
// stack capacity will transparently overflow onto the heap. The container must
// support reserve().
//
// WATCH OUT: the ContainerType MUST use the proper STLStackAllocator for this
// type. This object is really intended to be used only internally. You'll want
// to use the wrappers below for different types.

	template<typename TContainerType, int stack_capacity, typename _NexAllocator>
	class STLStackContainer {
	public:
		typedef TContainerType ContainerType;
		typedef typename ContainerType::value_type ContainedType;
		typedef STLStackAllocator<ContainedType, stack_capacity, _NexAllocator> Allocator;

		// Allocator must be constructed before the container!

		STLStackContainer() :
				allocator_(&stack_data_), container_(allocator_) {
			// Make the container use the stack allocation by reserving our buffer size
			// before doing anything else.
			container_.reserve(stack_capacity);
		}

		// Getters for the actual container.
		//
		// Danger: any copies of this made using the copy constructor must have
		// shorter lifetimes than the source. The copy will share the same allocator
		// and therefore the same stack buffer as the original. Use std::copy to
		// copy into a "real" container for longer-lived objects.

		ContainerType& container() {
			return container_;
		}

		const ContainerType& container() const {
			return container_;
		}

		// Support operator-> to get to the container. This allows nicer syntax like:
		//   StackContainer<...> foo;
		//   std::sort(foo->begin(), foo->end());

		ContainerType * operator->() {
			return &container_;
		}

		const ContainerType * operator->() const {
			return &container_;
		}

#ifdef UNIT_TEST
		// Retrieves the stack source so that that unit tests can verify that the
		// buffer is being used properly.

		const typename Allocator::Source& stack_data() const {
			return stack_data_;
		}
#endif

	protected:
		typename Allocator::Source stack_data_;
		Allocator allocator_;
		ContainerType container_;

	};

// StackString

	template<size_t stack_capacity, typename _NexAllocator>
	class StackString: public STLStackContainer<
			std::basic_string<char, std::char_traits<char>,
					STLStackAllocator<char, stack_capacity, _NexAllocator> >,
			stack_capacity, _NexAllocator> {
	public:

		StackString() :
				STLStackContainer<
						std::basic_string<char, std::char_traits<char>,
								STLStackAllocator<char, stack_capacity,
										_NexAllocator> >, stack_capacity,
						_NexAllocator>() {
		}

	private:

	};
}

#endif //NEXTAR_STLALLOC_H
