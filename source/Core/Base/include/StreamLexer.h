/*
 * StreamLexer.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef STREAMLEXER_H_
#define STREAMLEXER_H_

namespace nextar {

	class StreamLexer : public AllocGeneral {
		NEX_LOG_HELPER(StreamLexer);
	public:
		StreamLexer(const char* buffer, size_t size);
		~StreamLexer();

		inline bool   IsEndOfStream();
		inline char   Current() const;
		inline char   Forward();
		inline void   Backward();
		inline int    Line() const;
		inline size_t Tell() const;
		inline String AsText(size_t offset = 0, size_t n = String::npos);
		       void   SkipWhite();
		       void   SkipLine();
		/* Reads space separated words. Reads a string within quotes or '[]'
		 * appearing within the word and completely. So it will read:
		 * 'Example"Escaped quote\"and slash\\String Value"$Appended [some more example]'
		 * as a whole word. The quote will not appear in the word read unless escaped
		 * using '\'. The result word read thus will be
		 * - ExampleEscaped quote "and slash\String Value$Appended some more example*/
		const String& ReadWord();
		const String& ReadWord(const String& delimiters);
		/* Will either read the whole line or read multiple lines within a quoted string */
		const String& ReadStringLiteral();

	protected:
		size_t size;
		size_t pos;
		const char* buffer;
		String temp;
		int lineNum;
	};

	inline int StreamLexer::Line() const {
		return lineNum;
	}

	inline char StreamLexer::Current() const {
		if (pos >= size)
			return 0;
		return buffer[pos];
	}

	inline size_t StreamLexer::Tell() const {
		return pos;
	}

	inline char StreamLexer::Forward() {
		if (pos >= size)
			return 0;
		return buffer[pos++];
	}

	inline void StreamLexer::Backward() {
		if (pos)
			--pos;
	}

	inline bool StreamLexer::IsEndOfStream() {
		return (pos >= size) != 0;
	}

	inline String StreamLexer::AsText(size_t off, size_t n) {
		String str;
		size_t copyAmt = std::min(n, size-off);
		str.assign(buffer+off, copyAmt);
		return str;
	}

} /* namespace nextar */
#endif /* STREAMLEXER_H_ */
