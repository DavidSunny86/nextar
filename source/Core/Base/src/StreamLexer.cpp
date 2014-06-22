/*
 * StreamLexer.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <StreamLexer.h>

namespace nextar {

StreamLexer::StreamLexer(const char* _buffer, size_t _size) :
		pos(0), buffer(_buffer), size(_size), lineNum(0) {
}

StreamLexer::~StreamLexer() {
}

void StreamLexer::SkipWhite() {
	while (pos < size && buffer[pos] && std::isspace(buffer[pos]))
		pos++;
}

void StreamLexer::SkipLine() {
	while (pos < size && buffer[pos] && buffer[pos] != '\n')
		pos++;
	lineNum++;
}

const String& StreamLexer::ReadWord(const String& delimiters) {
	temp.clear();
	while (pos < size && buffer[pos]
			&& delimiters.find(buffer[pos]) == String::npos)
		temp += buffer[pos++];
	return temp;
}

const String& StreamLexer::ReadWord() {
	bool lookForTerminatingQuotes = false;
	temp.clear();
	// skip white

	while (pos < size && buffer[pos] && std::isspace(buffer[pos]))
		pos++;
	char c;
	while (pos < size && (c = buffer[pos])) {
		switch (c) {
		case 0:
			break;
		case '\r':
		case '\n':
			// for CR LF
			if (!pos || buffer[pos - 1] != '\r')
				lineNum++;
			/* no break */
		case '{': // special characters
		case '}':
		case '\f':
		case ' ':
		case '\v':
		case '\t':
			// append
			if (lookForTerminatingQuotes) {
				temp += c;
				pos++;
			} else
				break;
			continue; // continue with the loop
		case '\\':
			// escaped character?
			pos++;
			if (pos < size) {
				c = buffer[pos++];
				switch (c) {
				case 'n':
					temp += '\n';
					break;
				case 'r':
					temp += '\r';
					break;
				default:
					temp += c;
					break;
				}
			} else
				break;
			continue; // with the loop
		case ']':
			if (!lookForTerminatingQuotes) {
				Error("unexpected ']' string terminator");
				break;
			}
			/* no break */
		case '[':
			if (lookForTerminatingQuotes) {
				Error("expected ']', but got '['");
				break;
			}
			/* no break */
		case '"':
			pos++;
			lookForTerminatingQuotes = !lookForTerminatingQuotes;
			/* no break */
		default:
			temp += c;
			pos++;
			continue;
		}
		// break out of the loop
		break;
	}

	return temp;
}

const String& StreamLexer::ReadStringLiteral() {
	bool lookForTerminatingQuotes = false;
	temp.clear();
	// skip white

	while (pos < size && buffer[pos] && std::isspace(buffer[pos]))
		pos++;
	if (buffer[pos] == '"') {
		lookForTerminatingQuotes = true;
		pos++;
	}
	char c;
	while (pos < size && (c = buffer[pos])) {

		switch (c) {

		case '\r':
		case '\n': {
			// for CR LF
			if (!pos || buffer[pos - 1] != '\r')
				lineNum++;
			if (lookForTerminatingQuotes)
				pos++;
			else
				break;
		}
			;
			continue; // continue with the loop

		case '\\': {
			// escaped character?
			pos++;
			if (pos < size) {
				c = buffer[pos++];
				switch (c) {
				case 'n':
					temp += '\n';
					break;
				case 'r':
					temp += '\r';
					break;
				default:
					temp += c;
					break;
				}
			} else
				break;
		}
			continue; // with the loop

		case '"':
			if (lookForTerminatingQuotes) {
				pos++;
				break;
			}
			/* no break */
		default:
			temp += c;
			pos++;
			continue;

		}
		// break out of the loop
		break;
	}

	return temp;
}

} /* namespace nextar */
