/*
 * EnumMacros.h
 *
 *  Created on: 10-May-2014
 *      Author: obhi
 */

#ifndef ENUMMACROS_H_
#define ENUMMACROS_H_


#define NEX_ENUM_UINT(EnumName, IntType)									\
	inline constexpr bool													\
		operator<(EnumName __x, EnumName __y) {								\
			return static_cast<bool>										\
					(static_cast<IntType>(__x) < static_cast<IntType>(__y));\
			}																\
	inline constexpr bool													\
		operator>(EnumName __x, EnumName __y) {								\
			return static_cast<bool>										\
					(static_cast<IntType>(__x) > static_cast<IntType>(__y));\
			}																\
	inline constexpr bool													\
		operator<(IntType __x, EnumName __y) {								\
			return static_cast<bool>										\
					(static_cast<IntType>(__x) < static_cast<IntType>(__y));\
			}																\
	inline constexpr bool													\
		operator<(EnumName __x, IntType __y) {								\
			return static_cast<bool>										\
					(static_cast<IntType>(__x) < static_cast<IntType>(__y));\
			}																\
	inline constexpr bool													\
		operator>(IntType __x, EnumName __y) {								\
			return static_cast<bool>										\
					(static_cast<IntType>(__x) > static_cast<IntType>(__y));\
			}																\
	inline constexpr bool													\
		operator>(EnumName __x, IntType __y) {								\
			return static_cast<bool>										\
					(static_cast<IntType>(__x) > static_cast<IntType>(__y));\
			}																\
	inline constexpr bool													\
		operator==(IntType __x, EnumName __y) {								\
			return static_cast<bool>										\
					(static_cast<IntType>(__x) == static_cast<IntType>(__y));\
			}																\
	inline constexpr bool													\
		operator==(EnumName __x, IntType __y) {								\
			return static_cast<bool>										\
					(static_cast<IntType>(__x) == static_cast<IntType>(__y));\
			}																\
	inline constexpr bool													\
		operator!=(IntType __x, EnumName __y) {								\
			return static_cast<bool>										\
					(static_cast<IntType>(__x) != static_cast<IntType>(__y));\
			}																\
	inline constexpr bool													\
		operator!=(EnumName __x, IntType __y) {								\
			return static_cast<bool>										\
					(static_cast<IntType>(__x) != static_cast<IntType>(__y));\
			}																\

#define NEX_ENUM_FLAGS(EnumName, IntType)									\
	inline constexpr EnumName												\
		operator&(EnumName __x, EnumName __y) {								\
		return static_cast<EnumName>										\
			(static_cast<IntType>(__x) & static_cast<IntType>(__y));		\
	}																		\
																			\
	inline constexpr EnumName												\
		operator|(EnumName __x, EnumName __y) {								\
		return static_cast<EnumName>										\
			(static_cast<IntType>(__x) | static_cast<IntType>(__y));		\
	}																		\
																			\
	inline constexpr EnumName												\
		operator^(EnumName __x, EnumName __y) {								\
		return static_cast<EnumName>										\
			(static_cast<IntType>(__x) ^ static_cast<IntType>(__y));		\
	}																		\
																			\
	inline constexpr EnumName												\
		operator~(EnumName __x)	{											\
		return static_cast<EnumName>(~static_cast<IntType>(__x));			\
	}																		\
																			\
	inline EnumName &														\
		operator&=(EnumName & __x, EnumName __y) {							\
		__x = __x & __y;													\
		return __x;															\
	}																		\
																			\
	inline EnumName &														\
		operator|=(EnumName & __x, EnumName __y) {							\
		__x = __x | __y;													\
		return __x;															\
	}																		\
																			\
	inline EnumName &														\
		operator^=(EnumName & __x, EnumName __y) {							\
		__x = __x ^ __y;													\
		return __x;															\
	}																		\
																			\
	inline bool operator ! (EnumName __x) {									\
		return static_cast<uint32>(__x) == 0;								\
	}																		\
																			\
	inline bool Test(EnumName __x) {										\
		return static_cast<uint32>(__x) != 0;								\
	}




#endif /* ENUMMACROS_H_ */
