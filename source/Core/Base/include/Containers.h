#pragma once

#include <utility>
#include <functional>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <list>
#include <forward_list>
#include <algorithm>
#include <array>

#if defined( NEX_USE_TBB )
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_unordered_set.h>
#include <tbb/concurrent_vector.h>
#elif defined(NEX_MSVC)
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <concurrent_vector.h>
#endif

namespace nextar {
// stl types
typedef STDAllocator<char, AllocatorString> STLAllocString;
typedef STDAllocator<char16_t, AllocatorString> STLAllocUniString;

typedef std::basic_string<char, std::char_traits<char>, STLAllocString> String;
typedef std::basic_string<char16_t, std::char_traits<char16_t>,
		STLAllocUniString> UniString;

typedef std::basic_ostringstream<char, std::char_traits<char>, STLAllocString> OutStringStream;

typedef std::basic_istringstream<char, std::char_traits<char>, STLAllocString> InStringStream;

typedef std::reference_wrapper<const String> StringRef;

template<typename T, std::size_t _Nm>
struct array {
	typedef typename std::array<T, _Nm> type;
};

template<typename T, typename Allocator = AllocatorGeneral,
		typename STDAllocatorType = STDAllocator<T, Allocator> >
struct vector {
	typedef typename std::vector<T, STDAllocatorType > type;
};

template<typename T, typename Allocator = AllocatorGeneral,
		typename STDAllocatorType = STDAllocator<T, Allocator> >
struct list {
	typedef typename std::list<T, STDAllocatorType > type;
};

template<typename T, typename Allocator = AllocatorGeneral,
		typename STDAllocatorType = STDAllocator<T, Allocator> >
struct forward_list {
	typedef typename std::forward_list<T, STDAllocatorType > type;
};

template<typename T, typename Pr = std::less<T>,
		typename Allocator = AllocatorGeneral,
		typename STDAllocatorType = STDAllocator<T, Allocator> >
struct set {
	typedef typename std::set<T, Pr, STDAllocatorType > type;
};

template<typename Kty, typename T, typename Pr = std::less<Kty>,
		typename Allocator = AllocatorGeneral,
		typename STDAllocatorType = STDAllocator<std::pair<Kty, T>, Allocator> >
struct map {
	typedef typename std::map<Kty, T, Pr,
			STDAllocatorType > type;
};

template<typename T, typename Hsh = std::hash<T>,
		typename Pr = std::equal_to<T>, typename Allocator = AllocatorGeneral,
		typename STDAllocatorType = STDAllocator<T, Allocator> >
struct unordered_set {
	typedef typename std::unordered_set<T, Hsh, Pr, STDAllocatorType> type;
};

template<typename Kty, typename T, typename Hsh = std::hash<Kty>,
		typename Pr = std::equal_to<Kty>, typename Allocator = AllocatorGeneral,
		typename STDAllocatorType = STDAllocator<std::pair<Kty, T>, Allocator> >
struct unordered_map {
	typedef typename std::unordered_map<Kty, T, Hsh, Pr,
			STDAllocatorType > type;
};

template<typename Kty, typename T, typename Hsh = std::hash<Kty>,
		typename Pr = std::equal_to<Kty>, typename Allocator = AllocatorGeneral,
		typename STDAllocatorType = STDAllocator<std::pair<Kty, T>, Allocator> >
struct unordered_multimap {
	typedef typename std::unordered_multimap<Kty, T, Hsh, Pr,
			STDAllocatorType > type;
};

template<typename Kty, typename T, typename Pr = std::less<Kty>,
		typename Allocator = AllocatorGeneral,
		typename STDAllocatorType = STDAllocator< std::pair<Kty, T>, Allocator> >
struct multimap {
	typedef typename std::multimap<Kty, T, Pr,
			STDAllocatorType > type;
};

template<typename T, typename Allocator = AllocatorGeneral,
		typename STDAllocatorType = STDAllocator< T, Allocator > >
struct deque {
	typedef typename std::deque< T, STDAllocatorType > type;
};

#if defined( NEX_USE_TBB )
#define NEX_HAS_CONCURRENT_CONTAINERS
template<typename Kty, typename T, typename Hsh = tbb::tbb_hash<Kty>, typename Pr = std::equal_to<Kty>,
typename Allocator = AllocatorGeneral>
struct concurrent_unordered_map {
	typedef typename tbb::concurrent_unordered_map<Kty, T, Hsh, Pr,
	STDAllocator<std::pair<Kty, T>, Allocator> > type;
};

template<typename T, typename Allocator = AllocatorGeneral>
struct concurrent_vector {
	typedef typename tbb::concurrent_vector<T, STDAllocator<T, Allocator> > type;
};
template<typename Kty, typename Hsh = tbb::tbb_hash<Kty>, typename Pr = std::equal_to<Kty>,
typename Allocator = AllocatorGeneral>
struct concurrent_unordered_set {
	typedef typename tbb::concurrent_unordered_set<Kty, Hsh, Pr,
	STDAllocator<Kty, Allocator> > type;
};
#elif defined( NEX_MSVC )
#define NEX_HAS_CONCURRENT_CONTAINERS
template<typename Kty, typename T, typename Hsh = std::hash<Kty>, typename Pr = std::equal_to<Kty>,
typename Allocator = AllocatorGeneral>
struct concurrent_unordered_map {
	typedef typename concurrency::concurrent_unordered_map<Kty, T, Hsh, Pr,
	STDAllocator<std::pair<Kty, T>, Allocator> > type;
};

template<typename T, typename Allocator = AllocatorGeneral>
struct concurrent_vector {
	typedef typename concurrency::concurrent_vector<T, STDAllocator<T, Allocator> > type;
};
template<typename Kty, typename Hsh = std::hash<Kty>, typename Pr = std::equal_to<Kty>,
typename Allocator = AllocatorGeneral>
struct concurrent_unordered_set {
	typedef typename concurrency::concurrent_unordered_set<Kty, Hsh, Pr,
	STDAllocator<Kty, Allocator> > type;
};
#else
// never define
#endif

// @remarks std::vector erase by element for unsorted lists.
template<typename Vec>
inline void BestErase(Vec& _cont, typename Vec::value_type& _what) {
	BestErase(_cont, std::find(_cont.begin(), _cont.end(), _what));
}

// @remarks std::vector erase by index for unsorted lists.
template<typename Vec>
inline void BestErase(Vec& _cont, typename Vec::iterator _where) {
	if (_where != _cont.end()) {
		if (_cont.size() > 1)
			*_where = *_cont.rbegin();
		_cont.pop_back();
	}
}

// @remarks sorted insertion by value.
template<typename _Cont, typename _Val>
inline void SortedInsert(_Cont& _cont, const _Val& _what) {
	typedef typename _Cont::iterator iteratorType;
	for (iteratorType it = _cont.begin(); it != _cont.end(); ++it) {
		if (_what < *it) {
			_cont.insert(it, _what);
			return;
		}
	}
	_cont.push_back(_what);
}

template<typename _It>
void DeleteContents(_It beg, _It en) {
	while (beg != en) {
		NEX_DELETE(*beg);
		++beg;
	}
}

template<typename _It>
void DeleteMapContents(_It beg, _It en) {
	while (beg != en) {
		NEX_DELETE( (*beg).second );
		++beg;
	}
}

inline bool operator <(const StringRef& r1, const StringRef& r2) {
	return r1.get() < r2.get();
}

typedef vector<uint16>::type UShortArray;
typedef vector<uint8>::type ByteStream;
}

#ifdef NEX_GCC
namespace std {

	template <> struct hash<nextar::String> {
		size_t operator()(const nextar::String& x) const {
			// ugly hack
			return std::_Hash_impl::hash(x.data(), x.length());
		}
	};
	template <> struct hash<nextar::StringRef> {
		size_t operator()(const nextar::StringRef& x) const {
			// ugly hack
			return std::_Hash_impl::hash(x.get().data(), x.get().length());
		}
	};
}
#endif
