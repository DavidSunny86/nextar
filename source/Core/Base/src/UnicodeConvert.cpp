
#include <BaseHeaders.h>
#include <StringUtils.h>

namespace nextar {
	namespace StringUtils {

		_NexInline void Encode(String& enc, utf32 c) {

		}

		_NexBaseAPI UniString ToUtf16(const String& utf8string) {

			UniString outPut;
			outPut.reserve(utf8string.length() + 1);
			const utf8* buffer = (const utf8*) utf8string.c_str();
			size_t length = utf8string.length();
			utf32 c;
			while (*buffer && length > 0) {
				utf8 scan = buffer[0];
				if (scan < 0x80) {
					outPut += scan;
					++buffer;
					--length;
				} else if (scan < 0xE0 && length >= 2) {
					c = ((scan & 0x1F) << 6);
					c |= (buffer[1] & 0x3F);
					// NEX_ASSERT (c <= 0xD800 && c >= 0xDFFF);
					outPut += (utf16) (c);
					buffer += 2;
					length -= 2;
				} else if (scan < 0xF0 && length >= 3) {
					// decode
					c = ((scan & 0x0F) << 12);
					c |= ((buffer[1] & 0x3F) << 6);
					c |= (buffer[2] & 0x3F);
					// NEX_ASSERT (c <= 0xD800 && c >= 0xDFFF);
					outPut += (utf16) (c);
					buffer += 3;
					length -= 3;
				} else if (length >= 4) {
					c = ((scan & 0x07) << 18);
					c |= ((buffer[1] & 0x3F) << 12);
					c |= ((buffer[2] & 0x3F) << 6);
					c |= (buffer[3] & 0x3F);
					// encode
					uint32 v = c - 0x10000;
					outPut += (utf16) ((v >> 10) + 0xD800);
					outPut += (utf16) ((v & 0x3FF) + 0xDC00);
					buffer += 4;
					length -= 4;
				} else
					NEX_THROW_GracefulError(EXCEPT_MALFORMED_ENCODING);
			}

			return outPut;
		}

		_NexBaseAPI String ToUtf8(const UniString& utf16string) {
			String outPut;
			outPut.reserve(utf16string.length() + 1);
			const utf16* buffer = (const utf16*) utf16string.c_str();
			size_t length = utf16string.length();
			while (*buffer && length > 0) {
				utf16 scan = buffer[0];
				if (scan < 0x80) {
					outPut += (utf8) scan;
					--length;
					++buffer;
				} else if (scan >= 0xD800 && scan <= 0xDFFF && length >= 2) {
					// higher 10 bits
					scan = scan & 0x3FF;
					// lower 10 bits
					utf16 scan2 = (buffer[1] & 0x3FF);
					uint32 c = ((scan << 10) | scan2) + 0x10000;
					length -= 2;
					buffer += 2;
					// U+10000 to U+1FFFFF
					outPut += (utf8) ((char) (c >> 18) & 0x07) | 0xF0;
					outPut += (utf8) ((char) (c >> 12) & 0x3F) | 0x80;
					outPut += (utf8) ((char) (c >> 6) & 0x3F) | 0x80;
					outPut += (utf8) (((char) c) & 0x3F) | 0x80;
				} else {
					// U+0080 - U+07FF
					if (scan < 0x07FF) {
						outPut += (((char) (scan >> 6) & 0x1F) | 0xC0);
						outPut += (((char) scan) & 0x3F) | 0x80;
					}
					// U+0800 - U-FFFF
					else {
						outPut += ((char) (scan >> 12) & 0x0F) | 0xE0;
						outPut += ((char) (scan >> 6) & 0x3F) | 0x80;
						outPut += (((char) scan) & 0x3F) | 0x80;
					}
					--length;
					++buffer;
				}
			}

			return outPut;
		}

	}
}
