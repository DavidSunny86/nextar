#ifndef NEXTAR_NEXASSERT_H__
#define NEXTAR_NEXASSERT_H__

#include <NexBase.h>

#ifndef NEXTAR_BASE_DEF_H
#error Do not include this file directly, include "NexBase.h" instead
#endif

namespace nextar {

	template<bool> struct AssertFaliure;

	template<> struct AssertFaliure<true> {
		enum {
			value = 1
		};
	};

	template<int x> struct AssertTest {
	};
}

#define NEX_STATIC_ASSERT(x)                                   \
    typedef nextar::AssertTest<                                 \
        sizeof(nextar::AssertFaliure< (bool)( x ) >)>           \
            NEX_TOKEN_PASTE(_static_assert_typedef, __LINE__)

// assertion macro
#if NEX_USE_ASSERTION
#include <cassert>
#define NEX_ASSERT( arg1 )	assert(arg1)
#define NEX_ASSERT_PTR( arg1 )	NEX_ASSERT( (arg1 != NULL) )
#define NEX_ASSERT_PTR4( arg1 )	NEX_ASSERT( ((arg1 != NULL) && (( 0 == (((size_t)(arg1))&3)) )) )
#define NEX_ASSERT_PTR8( arg1 )	NEX_ASSERT( ((arg1 != NULL) && (( 0 == (((size_t)(arg1))&7)) )) )
#define NEX_ASSERT_PTR16( arg1 )	NEX_ASSERT( ((arg1 != NULL) && (( 0 == (((size_t)(arg1))&15)) )) )
#define NEX_ASSERT_FNEG( arg1 )	NEX_ASSERT(0.0f <= (arg1))
#define NEX_ASSERT_FZERO( arg1 ) NEX_ASSERT(0.0f != (arg1))
#define NEX_ASSERT_NEG( arg1 )	NEX_ASSERT(0 <= (arg1))
#define NEX_ASSERT_ZERO( arg1 )	NEX_ASSERT(0 != (arg1))
#else
#define NEX_ASSERT( arg1 )
#define NEX_ASSERT_PTR( arg1 )
#define NEX_ASSERT_PTR4( arg1 )
#define NEX_ASSERT_PTR8( arg1 )
#define NEX_ASSERT_PTR16( arg1 )
#define NEX_ASSERT_FNEG( arg1 )
#define NEX_ASSERT_FZERO( arg1 )
#define NEX_ASSERT_NEG( arg1 )
#define NEX_ASSERT_ZERO( arg1 )
#endif
#define NEX_ASSERT_NOT( arg1 ) NEX_ASSERT( !(arg1) )
#endif //NEXTAR_NEXASSERT_H__
