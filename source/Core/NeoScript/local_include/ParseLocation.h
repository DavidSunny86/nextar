/**
 * Created on: 18th Oct 15
 * @author: obhi
 **/
#ifndef PARSELOCATION_H_
#define PARSELOCATION_H_

namespace nextar {

class ParsePosition {
public:
	ParsePosition() :
			_fileName(nullptr), _line(-1), _column(-1) {
	}
	ParsePosition(const ParsePosition& p) :
			_fileName(p._fileName), _line(p._line), _column(p._column) {
	}
	ParsePosition(ParsePosition&& p) :
			_fileName(p._fileName), _line(p._line), _column(p._column) {
	}
	ParsePosition(const String* _f) :
			_fileName(_f), _line(0), _column(0) {
	}
	ParsePosition(const String* _f, uint32 col, uint32 line) :
			_fileName(_f), _line(line), _column(col) {
	}

	ParsePosition& operator =(const ParsePosition& o) {
		_fileName = o._fileName;
		_line = o._line;
		_column = o._column;
		return *this;
	}

	ParsePosition& operator =(ParsePosition&& o) {
		_fileName = o._fileName;
		_line = o._line;
		_column = o._column;
		return *this;
	}

	bool operator ==(const ParsePosition& o) const {
		if (_line == o._line && _column == o._column) {
		if (_fileName == o._fileName ||
			(o._fileName && _fileName &&
			*o._fileName == *_fileName))
			return true;
		}
		return false;
	}

	bool operator !=(const ParsePosition& o) const {
		return !(*this == o);
	}

	friend std::ostream& operator << (std::ostream& yyo, const ParsePosition& l) {
		yyo << "(" << l._line << "," << l._column << ")";
		return yyo;
	}

	uint32 _column;
	uint32 _line;
	const String* _fileName;
};

class ParseLocation {
public:
	ParseLocation() {}
	ParseLocation(const ParseLocation& p) :
			begin(p.begin), end(p.end) {
	}
	ParseLocation(ParseLocation&& p) :
			begin(std::move(p.begin)), end(std::move(p.end)) {
	}

	ParseLocation& operator =(const ParseLocation& p) {
		begin = p.begin;
		end = p.end;
		return *this;
	}
	ParseLocation& operator =(ParseLocation&& p) {
		begin = std::move(p.begin);
		end = std::move(p.end);
		return *this;
	}

	bool operator ==(const ParseLocation& p) const {
		return p.begin == begin && p.end == end;
	}
	bool operator !=(const ParseLocation& p) const {
		return !(*this == p);
	}

	void Step() {
		begin = end;
	}
	void Columns(uint32 l) {
		end._column += l;
	}
	void Lines(uint32 l) {
		end._line += l;
	}

	String AsString() const {
		String value = begin._fileName ? *begin._fileName : "buffer";
		value += "(" + Convert::ToString(begin._line) + ":" + Convert::ToString(begin._column) + "-"
				+ Convert::ToString(end._line) + ":" + Convert::ToString(end._column) + ")";
		return value;
	}

	friend std::ostream& operator << (std::ostream& yyo, const ParseLocation& l) {
		yyo << "<" << l.begin << "-" << l.end << ">";
		return yyo;
	}

	ParsePosition begin;
	ParsePosition end;
};

}

#endif // PARSELOCATION_H_
