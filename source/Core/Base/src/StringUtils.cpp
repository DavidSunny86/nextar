#include <BaseHeaders.h>
#include <NexBase.h>
#include <StringUtils.h>

namespace nextar {
	namespace StringUtils {
		const String Null;
		const String Default("Default");
		const UniString UniNull;
		const String Unknown("<?>");
		const StringID NullID(0);
		const StringID DefaultID(1);
		const StringRef NullRef(Null);
		const StringRef DefaultRef(Default);

		_NexBaseAPI char* NewStr(const char* str) {
			size_t s;
			char* str2;
			s = StringUtils::Length(str) + 2;
			str2 = (char*) NEX_ALLOC(s, MEMCAT_STRINGPOOL);
			std::memcpy(str2, str, s);
			//str2[s-1] = 0;
			return str2;
		}

		_NexBaseAPI char* NewStr(const char* str, size_t count) {
			char* str2;
			str2 = (char*) NEX_ALLOC(count + 1, MEMCAT_STRINGPOOL);
			std::memcpy(str2, str, count);
			str2[count] = 0;
			return str2;
		}

		_NexBaseAPI wchar_t* NewStr(const wchar_t* str) {
			size_t s;
			wchar_t* str2;

			s = wcslen(str) + 1;
			str2 = (wchar_t*) NEX_ALLOC(s * sizeof (wchar_t), MEMCAT_STRINGPOOL);
			std::memcpy(str2, str, s);
			//str2[s-1] = 0;
			return str2;
		}

		_NexBaseAPI char* NewStrConv(const wchar_t* str) {
			// this might actually be invalid
			size_t s;
			char *str2, *ret;
			s = wcslen(str) + 1;
			str2 = (char*) NEX_ALLOC(s * sizeof (wchar_t), MEMCAT_GENERAL);
#if defined(NEX_MSVC)
			wcstombs_s(&s, str2, s*sizeof (wchar_t), str, _TRUNCATE);
#else
			s = wcstombs(str2, str, s);
#endif
			ret = (char*) NEX_ALLOC(s, MEMCAT_STRINGPOOL);
			std::memcpy(ret, str2, s);
			NEX_FREE(str2, MEMCAT_GENERAL);
			return ret;
		}

		_NexBaseAPI wchar_t* NewStrConv(const char* str) {
			size_t s;
			wchar_t *str2, *ret;
			s = StringUtils::Length(str) + 1;
			str2 = (wchar_t*) NEX_ALLOC(s * sizeof (wchar_t), MEMCAT_GENERAL);
#if defined(NEX_MSVC)
			mbstowcs_s(&s, str2, s, str, _TRUNCATE);
#else
			s = mbstowcs(str2, str, s);
#endif
			ret = (wchar_t*) NEX_ALLOC(s, MEMCAT_STRINGPOOL);
			std::memcpy(ret, str2, s);
			NEX_FREE(str2, MEMCAT_GENERAL);
			return ret;
		}

#define utf32isspace(p) (((p) >= 0x09 && (p) <= 0x0D) || (p) == 0x20)
#define utf8isspace(p) (((p) >= 0x09 && (p) <= 0x0D) || (p) == 0x20)

		_NexBaseAPI const utf8* GetNextWordEndPtrUtf8(const utf8* p,
				const utf8**start) {
			std::locale l;
			while (*p && std::isspace((int) *p, l))
				++p;
			if (start)
				*start = p;
			while (*p) {
				if (std::isspace((int) *p, l))
					break;
				++p;
			}
			return p;
		}

		_NexBaseAPI const utf32* GetNextWordEndPtrUtf32(const utf32* p,
				const utf32**start) {
			while (*p && utf32isspace(*p))
				++p;
			if (start)
				*start = p;
			while (*p) {
				if (utf32isspace(*p))
					break;
				++p;
			}
			return p;
		}

		_NexBaseAPI const utf8* GetWordEndPtrUtf8(const utf8* p) {
			while (*p) {
				if (isspace(*p))
					break;
				++p;
			}
			return p;
		}

		_NexBaseAPI const utf32* GetWordEndPtrUtf32(const utf32* p) {
			while (*p) {
				if (utf32isspace(*p))
					break;
				++p;
			}
			return p;
		}

		/* returns the pointer where no whitespace [' ','\n','\t'] is encountered */_NexBaseAPI const utf32* EatWhiteUtf32(
				const utf32* p) {
			if (!p)
				return 0;
			while (*p && utf32isspace(*p))
				++p;
			return p;
		}

		/* returns the pointer where no whitespace [' ','\n','\t'] is encountered */_NexBaseAPI const utf8* EatWhiteUtf8(
				const utf8* p) {
			if (!p)
				return 0;
			while (*p && utf8isspace(*p))
				++p;
			return p;
		}

//@ returns at the encounter of 'indexofchar' or null

		_NexBaseAPI const utf8* GetIndexOfUtf8(const utf8* p, const utf8 c) {
			while (*p && *p != c)
				++p;
			return p;
		}

//@ returns at the encounter of 'indexofchar' or null

		_NexBaseAPI const utf32* GetIndexOfUtf32(const utf32* p, const utf32 c) {
			while (*p && *p != c)
				++p;
			return p;
		}

		/* returns the pointer where no whitespace [' ','\n','\t'] is encountered */_NexBaseAPI const char* EatWhite(
				const char* p) {
			if (!p)
				return 0;
			std::locale l;
			while (*p && std::isspace(*p, l))
				++p;
			return p;
		}

		_NexBaseAPI const char* MatchTag(const char* text, const char* tag) {
			const char *mtext = text;
			mtext = EatWhite(mtext);
			while (*mtext && *tag) {
				if (*(mtext++) != *(tag++))
					return 0;
			}
			if (!(*tag)) {
				return mtext;
			}
			return 0;
		}

// ctype functions not as efficient as asm
// but the return value is significant

		_NexBaseAPI char* CopyStr(char* dst, const char* src) {
			char * cp = dst;
			while (*cp++ = *src++)
				;
			return (cp - 1);
		}

		_NexBaseAPI wchar_t* CopyStr(wchar_t* dst, const wchar_t* src) {
			wchar_t * cp = dst;
			while (*cp++ = *src++)
				;
			return (cp);
		}

		_NexBaseAPI String& Replace(String &s, const String &sub,
				const String &other) {
			NEX_ASSERT(!sub.empty());
			size_t b = 0;
			while (((b = s.find(sub, b)) != String::npos)) {
				s.replace(b, sub.size(), other);
				b += other.size();
			}
			return s;
		}

		_NexBaseAPI bool PatternMatch(const char* pat, const char* str,
				bool checkcase) {
			while (*str) {
				switch (*pat) {
				case '?':
					if (*str == '.')
						return false;
					break;
				case '*':
					do {
						++pat;
					} while (*pat == '*');
					if (!*pat)
						return true;
					while (*str) {
						if (PatternMatch(pat, str++))
							return true;
					}
					return false;
				default: {
					char c1 = *str;
					char c2 = *pat;
					// doing here to avoid extra allocations
					if (!checkcase) {
						c1 = tolower(c1);
						c2 = tolower(c2);
					}
					if (c1 != c2)
						return false;
				}
					break;
				}
				++pat, ++str;
			}
			while (*pat == '*')
				++pat;
			return (*pat) == 0;
		}

		_NexBaseAPI void ToLower(String& str) {
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		}

		_NexBaseAPI void ToUpper(String& str) {
			std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		}

		_NexBaseAPI void FreeStr(char* ptr) {
			NEX_FREE(ptr, MEMCAT_STRINGPOOL);
		}

		_NexBaseAPI void FreeStr(wchar_t* ptr) {
			NEX_FREE(ptr, MEMCAT_STRINGPOOL);
		}

		String FormatUnderscoreString(const String& str) {
			String ret;
			ret.reserve(str.length()+1);
			bool upper = true;
			size_t i = 0;
			while (i < str.length()) {
				if (str[i]=='_') {
					if(!upper) {
						ret += ' ';
						upper = true;
					}
				} else {
					ret += upper ? ::toupper(str[i]) : ::tolower(str[i]);
					upper = false;
				}
				i++;
			}
			return ret;
		}

		_NexBaseAPI String FormatCamelCaseString(String& str) {
			String ret;
			ret.reserve(str.length()+4);
			size_t i = 1;
			ret += ::toupper(str[0]);
			while (i < str.length()) {
				if (::isupper(str[i])) {
					ret += ' ';
				}
				ret += str[i];
				i++;
			}
			return ret;
		}

		_NexBaseAPI String FormatName(String& str) {
			if (!str.length())
				return StringUtils::Null;
			if (str.find('_') != String::npos)
				return FormatUnderscoreString(str);
			else
				return FormatCamelCaseString(str);
		}
	}

}

