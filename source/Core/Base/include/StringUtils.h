//@ Obi De
//@ 12/25/2006		AT:		5:19:51 PM
//@ 12/25/2006		AT:		5:19:51 PM
//@ 1/1/2007		AT:		8:03:49 PM
//@ Oct 13 07
//@ Dec 3 08

#ifndef NEXTAR_NSTR_H
#define NEXTAR_NSTR_H

#include <NexBase.h>
#include <MemUtils.h>
#include <StringInternTable.h>
#include <MultiString.h>

// some macros
#define NEX_DEFAULT_UCHAR	0xFFFD
#define NEX_WRONG_UCHAR		0xFFFF
#define NEX_LAST_UCHAR		0x10FFFF

namespace nextar {
typedef vector<String, AllocatorString>::type StringVector;
typedef std::pair<String, String> StringPair;


namespace StringUtils {
// const char* operator
//! Exported constants
enum {
	STRING_ID_START_OFFSET = 99999,
};

NEX_EXTERN_SYM _NexBaseAPI const String Null;
NEX_EXTERN_SYM _NexBaseAPI const String Default;
NEX_EXTERN_SYM _NexBaseAPI const String Unknown;
NEX_EXTERN_SYM _NexBaseAPI const String DefaultSymbol;
NEX_EXTERN_SYM _NexBaseAPI const UniString UniNull;
NEX_EXTERN_SYM _NexBaseAPI const StringID NullID;
NEX_EXTERN_SYM _NexBaseAPI const StringID DefaultID;
NEX_EXTERN_SYM _NexBaseAPI const StringRef NullRef;
NEX_EXTERN_SYM _NexBaseAPI const StringRef DefaultRef;

struct Less { // functor for operator <

	bool operator()(const char* _Left, const char* _Right) const { // apply operator< to operands
		return std::strcmp(_Left, _Right) < 0;
	}
};

//enum {
//	SEPARATOR_CHAR = 4, // EOT
//};

/**
 * @note
 * Multiple strings in nextar components are often stored in a single string
 * buffer and can be split via these utilities.
 * TokenIterator is the basic iterator used for those strings, it should
 * be initialized to 0.
 */
typedef MultiString::Iterator TokenIterator;
typedef nextar::StringPair StringPair;
typedef String WordList;

struct NotAsciiTest: public std::unary_function<int, bool > {
	inline bool operator ()(char a) {
		return !isascii(a);
	}
};
/**
 * Change the delimeter for each word in this string to
 * compatible string array type.
 */
/*
template<typename StringType>
void ReplaceDelimiter(StringType& strArr,
		typename StringType::value_type delt) {
	typename StringType::value_type val = StringUtils::SEPARATOR_CHAR;
	std::replace(strArr.begin(), strArr.end(), delt, val);
}
*/
/**
 * @remarks
 * For a container class, this will append all the strings in the
 * array or list or queue into one string
 */
/*template<typename _ContIter, typename StringType/
StringType Combine(_ContIter beg, _ContIter end) {
	StringType ret;
	size_t totalLength = 0;
	_ContIter it = beg;
	while (it != end) {
		totalLength += (*it).length() + 1;
		++it;
	}
	totalLength += 1;
	ret.reserve(totalLength);
	it = beg;
	while (it != end) {
		ret += (*it);
		ret += SEPARATOR_CHAR;
		++it;
	}
	return ret;
}*/

/**
 * Returns the count of the string words stored in this container
 */
/*template<typename StringType>
size_t GetWordCount(const StringType& arr) {
	size_t offset = 0;
	size_t i = 0;
	// check if offset is less than length
	while (offset < arr.length()) {
		size_t pos = arr.find_first_of(SEPARATOR_CHAR, offset);
		if (pos == StringType::npos)
			pos = arr.length();
		i++;
		offset = pos + 1;
	}
	return i;
}*/

/**
 * Get location of word within a string, returns -1 if not found.
 */
/*template<typename StringType>
size_t GetWordIndex(const StringType& arr, const StringType& word) {
	size_t location = arr.find(word);
	if (location == StringType::npos)
		return -1;
	return std::count(arr.c_str(), arr.c_str() + location,
			(StringType::value_type)(SEPARATOR_CHAR));
}*/

/**
 * Return's the ith token
 * @param num Index of the token
 * @param arr The string container
 * @return Null if no tokens at ith index, else the string.
 */
/*template<typename StringType>
StringType GetWord(size_t num, const StringType& arr) {
	size_t offset = 0;
	size_t i = 0;
	while (offset < arr.length()) {
		size_t pos = arr.find_first_of(SEPARATOR_CHAR, offset);
		if (pos == StringType::npos)
			pos = arr.length();
		if (i == num)
			return arr.substr(offset, pos - offset);
		offset = pos + 1;
		i++;
	}
	return StringType();
}*/

/**
 * Set the ith token to the given word
 * @param num Index of the token
 * @param arr The string container
 * @param what The string to store
 * @return Null if no tokens at ith index, else the string.
 */
/*template<typename StringType>
void SetWord(size_t num, StringType& arr, const StringType& what) {
	size_t offset = 0;
	size_t i = 0;
	while (offset <= arr.length()) {
		size_t pos = arr.find_first_of(SEPARATOR_CHAR, offset);
		if (pos == StringType::npos)
			pos = arr.length();
		if (i == num) {
			arr.replace(offset, pos - offset, what);
			break;
		}
		offset = pos + 1;
		i++;
	}
}*/

/**
 * Traverse a string array container
 * @param arr The string container
 * @param str To store the string into
 * @param prev Previous iterator
 * @return StringType::npos when done.
 */
inline TokenIterator NextWord(const String& arr, String& store,
		TokenIterator prev) {
	prev.HasNext(store);
	return prev;
}

/**
 * Replace a word at a particular position
 **/
 /*
template<typename StringType>
void ReplaceWord(StringType& arr, TokenIterator prev, const StringType& by) {

	if (prev < arr.length()) {
		size_t pos = arr.find_first_of(SEPARATOR_CHAR, prev);
		if (pos == StringType::npos)
			pos = arr.length();
		else if (by.length() <= 0)
			pos += 1;

		arr.replace(prev, pos - prev, by);
	}
}
*/
/**
 * Push the string at the very end of the buffer
 * @param arr The string array container
 * @param what The string to push
 */
inline void PushBackWord(String& arr, const String& what) {
	MultiString m(arr);
	m.PushBack(what);
}
/**
 * Push the string at the very begining of the buffer
 * @param arr The string array container
 * @param what The string to push
 */
inline void PushFrontWord(String& arr, const String& what) {
	MultiString m(arr);
	m.PushFront(what);
}

inline size_t Length(const char* s) {
	return std::strlen(s);
}

inline size_t Length(const wchar_t* s) {
	return std::wcslen(s);
}

inline size_t Compare(const char* s1, const char* s2) {
	return std::strcmp(s1, s2);
}

inline size_t Compare(const wchar_t* s1, const wchar_t* s2) {
	return std::wcscmp(s1, s2);
}

inline size_t Compare(const char* s1, const char* s2, size_t n) {
	return std::strncmp(s1, s2, n);
}

inline size_t Compare(const wchar_t* s1, const wchar_t* s2, size_t n) {
	return std::wcsncmp(s1, s2, n);
}

/**
 * @brief	special copy string, returns the pointer of str1 pointing to null
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]		If non-null, the.
 * @param [in,out]		If non-null, the.
 *
 * @return	null if it fails, else.
 **/
_NexBaseAPI char* CopyStr(char*, const char*);
_NexBaseAPI wchar_t* CopyStr(wchar_t*, const wchar_t*);

/**
 * @brief	Returns the pointer where no whitespace [' ','\n','\t','\r'] is encountered
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]	sStr	If non-null, the string.
 *
 * @return	null if it fails, else pointer where no whitespace [' ','\n','\t','\r'] is encountered.
 **/
_NexBaseAPI const char* EatWhite(const char* sStr);

/**
 * @brief	Match tag marked by tag.
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]	text	If non-null, the text.
 * @param [in,out]	tag		If non-null, the tag.
 *
 * @return	null if it fails, else the pointer right after the tag ends.
 **/
_NexBaseAPI const char* MatchTag(const char* text, const char* tag);

/**
 * @brief	Allocates a new string based on old one.
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]		If non-null, the.
 *
 * @return	null if it fails, else.
 **/
_NexBaseAPI wchar_t* NewStr(const wchar_t*);
_NexBaseAPI char* NewStr(const char*);

/**
 *
 * @brief	Allocates a new string based on old one.
 *          Will not check for delimiter '\0'.
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]		If non-null, the.
 * @param	count		Number of.
 *
 * @return	null if it fails, else.
 **/
_NexBaseAPI char* NewStr(const char*, size_t count); //

/**
 *
 * @brief	Allocates a new string based on old one and converts it.
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]		If non-null, the.
 *
 * @return	null if it fails, else.
 **/
_NexBaseAPI wchar_t* NewStrConv(const char*);
_NexBaseAPI char* NewStrConv(const wchar_t*);
/**
 *
 * @brief	Eat white utf 8.
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]		If non-null, the.
 *
 * @return	null if it fails, else.
 **/
_NexBaseAPI const utf8* EatWhiteUtf8(const utf8*);

/**
 *
 * @brief	Eat white utf 32.
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]		If non-null, the string.
 *
 * @return	null if it fails, else.
 **/
_NexBaseAPI const utf32* EatWhiteUtf32(const utf32*);

/**
 *
 * @brief	Gets an indexof utf8.
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]		If non-null, the string.
 * @param	utf8		The utf8 char.
 *
 * @return	returns at the encounter of 'indexofchar' or null
 **/
_NexBaseAPI const utf8* GetIndexOfUtf8(const utf8*, const utf8);
/**
 *
 * @brief	Gets an indexof utf32.
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]		If non-null, the string.
 * @param	utf32		The second utf32 char.
 *
 * @return	returns at the encounter of 'indexofchar' or null
 **/
_NexBaseAPI const utf32* GetIndexOfUtf32(const utf32*, const utf32);
/**
 *
 * @brief	Returns at the encounter of first whitespace character
 *          the returned poiner either points to the whitespace or
 *          Null
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]		If non-null, the.
 *
 * @return	null if it fails, else the word endptr utf 8.
 **/
_NexBaseAPI const utf8* GetWordEndPtrUtf8(const utf8*);
/**
 * @brief   Returns at the encounter of first whitespace character
 *          the returned poiner either points to the whitespace or
 *          Null
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]		If non-null, the.
 *
 * @return	null if it fails, else the word endptr utf 32.
 **/
_NexBaseAPI const utf32* GetWordEndPtrUtf32(const utf32*);
/**
 *
 * @brief	First eats white then starts a scan.
 *          returns at the encounter of first whitespace character
 *          the returned poiner either points to the whitespace or
 *          to NULL also accepts a ptr to recieve the start of word
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]			If non-null, the.
 * @param [in,out]	start	If non-null, the start.
 *
 * @return	null if it fails, else the nextword endptr utf 8.
 **/
_NexBaseAPI const utf8* GetNextWordEndPtrUtf8(const utf8*, const utf8**start);
/**
 *
 * @brief	First eats white then starts a scan.
 *          returns at the encounter of first whitespace character
 *          the returned poiner either points to the whitespace or
 *          to NULL also accepts a ptr to recieve the start of word
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]			If non-null, the.
 * @param [in,out]	start	If non-null, the start.
 *
 * @return	null if it fails, else the nextword endptr utf 32.
 **/
_NexBaseAPI const utf32* GetNextWordEndPtrUtf32(const utf32*,
		const utf32**start);

/**
 * @brief	String no case Compare.
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]	s1	If non-null, the first const char*.
 * @param [in,out]	s2	If non-null, the second const char*.
 *
 * @return	.
 **/
inline int32 NoCaseCompare(const char* s1, const char* s2) {
#if defined(NEX_MSVC)
	return ::_stricmp(s1, s2);
#elif defined(NEX_GCC)
	return strcasecmp(s1, s2);
#else
#error Implement strNocaseCmp
	return strNocaseCmp(s1, s2);
#endif
}

// This is an alternative to char declarations
// common defs

/**
 * @brief	Free strings allocated by NewStr.
 *
 * @author	Abhishek Dey
 * @date	11/9/2009
 *
 * @param [in,out]	ptr	If non-null, the pointer.
 **/_NexBaseAPI void FreeStr(char* ptr);
_NexBaseAPI void FreeStr(wchar_t* ptr);

template<typename T> inline void SafeFreeString(T*& ptr) {
	if (ptr) {
		FreeStr(ptr);
		ptr = 0;
	}
}

inline String Indent(int32 amt) {
	String s;
	s.resize(amt, ' ');
	return s;
}
/**
 * @brief	Replace all occurences of a substring in a string with another string, in-place.
 *
 * @author	Abhishek Dey
 * @date	11/27/2009
 *
 * @param [in,out]	s	String to Replace in. Will be modified.
 * @param	sub			Substring to Replace.
 * @param	other		String to Replace substring with.
 *
 * @return	The string after replacements.
 *
 **/
_NexBaseAPI String& Replace(String &s, const String &sub, const String &other);

/**
 *
 * @brief	Matches string pattern based on wildcards[*,?,etc].
 *
 * @author	Abhishek Dey
 * @date	12/1/2009
 *
 * @param [in,out]	pattern	If non-null, the pattern.
 * @param [in,out]	str		If non-null, the string.
 * @param	checkcase		true to checkcase.
 *
 * @return	true if it succeeds, false if it fails.
 **/
_NexBaseAPI bool PatternMatch(const char* pattern, const char* str, bool checkcase = true);

/** Convert a camel or upper case name into a formatted string.
 * For Eg: "madeInChina" to "Made In China" or
 * MADE_IN_CHINA to "Made In China" */
_NexBaseAPI String FormatName(const String& str);
/**
* @brief	Convert string to lower case.
*
* @author	Abhishek Dey
* @date	12/4/2009
*
* @param [in,out]	str	The string to convert.
**/
template <typename StringType>
inline void ToLower(StringType& str) {
	std::transform(std::begin(str), std::end(str), std::begin(str), ::tolower);
}

/**
* @brief	Convert string to upper case.
*
* @author	Abhishek Dey
* @date	12/4/2009
*
* @param [in,out]	str	The string to convert.
**/
template <typename StringType>
void ToUpper(StringType& str) {
	std::transform(std::begin(str), std::end(str), std::begin(str), ::toupper);
}

/** Seperates a string pair of the format Abc:efg into 'Abc' and 'efg' */
inline StringPair Split(const String& name, char by = ':') {
	size_t seperator = name.find_first_of(by);
	if (seperator != String::npos)
		return StringPair(name.substr(0, seperator), name.substr(seperator + 1));
	else
		return StringPair(name, Null);
}

/** @remarks Trim leading white space **/
inline void TrimLeading(String& str) {
	size_t endpos = str.find_first_not_of(" \t\n\r");
	if (endpos != 0)
		str.erase(0, endpos);
}

/** @remarks Trim trailing white space **/
inline void TrimTrailing(String& str) {
	size_t endpos = str.find_last_not_of(" \t\n\r");
	if (endpos != String::npos)
		str.erase(endpos + 1);
}

inline void Trim(String& str) {
	TrimLeading(str);
	TrimTrailing(str);
}
/* Unicode conversion UTF-8 to UTF-16 */
_NexBaseAPI UniString ToUtf16(const String& utf8String);
_NexBaseAPI String ToUtf8(const UniString& utf16String);

inline bool IsAscii(const String& utf8String) {
	return std::find_if(utf8String.begin(), utf8String.end(), NotAsciiTest())
			== utf8String.end();
}

struct NoCaseLess { // functor for operator <

	bool operator()(const char* _Left, const char* _Right) const { // apply operator< to operands
		return NoCaseCompare(_Left, _Right) < 0;
	}
};

/************************************** Legacy **************************************/

inline int32 LengthUtf32(const utf32* b) {
	const utf32* s = b;
	while (*b)
		++b;
	return (int32) (b - s);
}

// returns the advance amount in the buffer,
// invalid codepoints will be replaced by default uchar
inline int32 DecodeUtf8(const utf8* buffer, int32 length, utf32& c) {
	if (buffer == 0) {
		return 0;
	}
	utf8 scan = buffer[0];
	if (scan < 0x80) {
		c = scan;
		return 1;;
	} else if (scan < 0xE0 && length >= 2) {
		c = ((scan & 0x1F) << 6);
		c |= (buffer[1] & 0x3F);
		return 2;
	} else if (scan < 0xF0 && length >= 3) {
		c = ((scan & 0x0F) << 12);
		c |= ((buffer[1] & 0x3F) << 6);
		c |= (buffer[2] & 0x3F);
		return 3;
	} else if (length >= 4) {
		c = ((scan & 0x07) << 18);
		c |= ((buffer[1] & 0x3F) << 12);
		c |= ((buffer[2] & 0x3F) << 6);
		c |= (buffer[3] & 0x3F);
		return 4;
	}
	c = NEX_DEFAULT_UCHAR;
	return -1;
}

inline int32 QuickDecodeUtf8(const utf8* src, int32 srcLen, utf32* dest,
		int32 destLen) {
	int32 dc = destLen;

	for (int32 idx = 0; ((idx < srcLen) && (dc > 0));) {
		utf32 cp;
		utf8 cu = src[idx++];

		if (cu < 0x80) {
			cp = (utf32) (cu);
		} else if (cu < 0xE0) {
			cp = ((cu & 0x1F) << 6);
			cp |= (src[idx++] & 0x3F);
		} else if (cu < 0xF0) {
			cp = ((cu & 0x0F) << 12);
			cp |= ((src[idx++] & 0x3F) << 6);
			cp |= (src[idx++] & 0x3F);
		} else {
			cp = ((cu & 0x07) << 18);
			cp |= ((src[idx++] & 0x3F) << 12);
			cp |= ((src[idx++] & 0x3F) << 6);
			cp |= (src[idx++] & 0x3F);
		}

		*dest++ = cp;
		--dc;
	}

	return destLen - dc;
}

inline int32 DecodeSizeUtf8(const utf8* buf, int32 len) {
	utf8 tcp;
	int32 count = 0;
	while (len-- > 0) {
		tcp = *buf++;
		++count;
		if (tcp < 0x80) {
		} else if (tcp < 0xE0) {
			--len;
			++buf;
		} else if (tcp < 0xF0) {
			len -= 2;
			buf += 2;
		} else {
			len -= 2;
			buf += 3;
		}
	}
	return count;
}

}
}

#endif //NEXTAR_NSTR_H