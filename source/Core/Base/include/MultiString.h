#ifndef MULTISTRINGLIST_H_
#define MULTISTRINGLIST_H_

#include <NexBase.h>

namespace nextar {
namespace StringUtils {

namespace ESpecialValues {
	// x> string x<
	constexpr uint32 SOH_CHAR = 1;
	constexpr uint32 STX_CHAR = 2;
	constexpr uint32 EOT_CHAR = 4;
	constexpr uint32 HEAD_OFFSET = 5;
	constexpr uint32 LEN_BYTE_SZ = 4;
	constexpr uint32 STR_OFFSET = 5;
};

constexpr uint32 npos = -1;

template <typename _StringType>
class IteratorType {
	using StringType = const _StringType;
	using Type = typename std::remove_const<StringType>::type;
	using ConstType = const Type;
	using CharType = typename Type::value_type;

	public:

	IteratorType(StringType& object) :
			_location(0), _object(object) {
	}
	IteratorType(StringType& object, uint32 loc) :
			_location(loc), _object(object) {
	}
	IteratorType(const IteratorType& it) : _location(it._location),
			_object(it._object) {
	}
	IteratorType(IteratorType&& it) : _location(it._location),
			_object(it._object) {
	}

	IteratorType& operator = (const IteratorType& it) {
		NEX_ASSERT(&_object == &it._object);
		_location = it._location;
		return *this;
	}

	IteratorType& operator = (IteratorType&& it) {
		NEX_ASSERT(&_object == &it._object);
		_location = it._location;
		return *this;
	}

	bool operator ==(ConstType& _what) const {
		if (!_location)
			return false;
		if (_location == npos)
			return _object == _what;
		NEX_ASSERT(_location < _object.size());
		NEX_ASSERT(_object[_location] == ESpecialValues::STX_CHAR);
		if (_Len() != _what.size())
			return false;
		return _object.compare(_location + ESpecialValues::STR_OFFSET, _what.size(), _what)
				== 0;
	}

	operator bool() const {
		return _location != 0;
	}

	friend bool operator ==(ConstType& _what, const IteratorType& it) {
		return it == _what;
	}

	friend bool operator !=(ConstType& _what, const IteratorType& it) {
		return !(it == _what);
	}

	IteratorType& operator ++() {
		_Next();
		return *this;
	}

	bool operator ()(Type& store) {
		if (!_location)
			return false;
		_Store(store);
		return true;
	}

	bool HasNext(Type& store) {
		if (!_location)
			return false;
		_Store(store);
		_Next();
		return true;
	}

	Type operator *() const {
		Type ret;
		_Store(ret);
		return ret;
	}

	uint32 Location() const {
		return _location;
	}

private:
	void _Next() {
		if (_location != 0) {
			// condition where this was just one string
			// not yet constructred
			if (_location == npos)
				_location = 0;
			else {
				NEX_ASSERT(_location < _object.size());
				NEX_ASSERT(_object[_location] == ESpecialValues::STX_CHAR);
				_location += _Len() + ESpecialValues::STR_OFFSET;
				if ((_location >= _object.size())
					|| (_object[_location] != ESpecialValues::STX_CHAR))
					_location = 0;
			}
		}
	}

	void _Store(Type& store) const {
		if (!_location)
			return;
		if (_location == npos)
			store = _object;
		else {
			NEX_ASSERT(_object[_location] == ESpecialValues::STX_CHAR);
			uint32 l = _Len();
			store.reserve(l);
			NEX_ASSERT(_location + ESpecialValues::STR_OFFSET + l <= _object.size());
			store.assign(_object.c_str() + _location + ESpecialValues::STR_OFFSET, l);
		}
	}

	uint32 _Len() const {
		if (_location == npos)
			return (uint32)_object.size();
		NEX_ASSERT(_location + ESpecialValues::STR_OFFSET - 1 < (uint32)_object.size());

		return (((uint32) _object[_location + 1] << 24)
				| ((uint32) _object[_location + 2] << 16)
				| ((uint32) _object[_location + 3] << 8)
				| ((uint32) _object[_location + 4] << 0));
	}

	uint32 _location;
	StringType& _object;
};

template<typename StringType>
class MultiStringBase {

public:
	using ThisType = MultiStringBase<StringType>;
	using Type = typename std::remove_const<StringType>::type;
	using ConstType = const Type;
	using CharType = typename Type::value_type;
	using Iterator = IteratorType<ConstType>;

	static Iterator It(StringType& v) {
		ThisType t(v);
		return t.Iterate();
	}

	uint32 Length() const {
		if (!_object.size())
			return 0;
		// unconstructed string?
		if (_object[0] != ESpecialValues::SOH_CHAR)
			return 1;
		NEX_ASSERT(_object.size() >= ESpecialValues::HEAD_OFFSET);
		return (((uint32) _object[1] << 24) | ((uint32) _object[2] << 16)
				| ((uint32) _object[3] << 8) | ((uint32) _object[4] << 0));
	}

	static bool IsSimpleString(const String& value) {
		return !_IsCted(value);
	}

	uint32 IndexOf(ConstType& what) const {
		Iterator it = Iterate();
		uint32 i = 0;
		while (it) {
			if (it == what)
				return i;
			++it;
			++i;
		}
		return -1;
	}

	Iterator IteratorOf(ConstType& what) const {
		Iterator it = Iterate();
		while (it) {
			if (it == what)
				return it;
			++it;
		}
		return Iterator(_object);
	}

	Type& Get(Type& store, uint32 i) const {
		if (i < Length()) {
			Iterator it = Iterate();
			for (uint32 id = 0; id < i; ++id, ++it)
				;
			if (it) {
				it(store);
			}
		}
		return store;
	}

	Type Get(uint32 i) const {
		Type what;
		Get(what, i);
		return what;
	}

	Iterator Iterate() const {
		if (!_IsCted(_object) && _object.size() > 0)
			return Iterator(_object, (uint32)npos);
		if (Length() > 0)
			return Iterator(_object, ESpecialValues::HEAD_OFFSET);
		return Iterator(_object);
	}

	MultiStringBase(StringType& object) : _object(object) {}
protected:

	inline StringType& _Obj() const {
		return _object;
	}

	static bool _IsCted(ConstType& ob) {
		if (ob.size() >= ESpecialValues::HEAD_OFFSET) {
			if (ob[0] == ESpecialValues::SOH_CHAR)
				return true;
		}
		return false;
	}

	StringType& _object;
};

template<typename StringType, bool is_not_const>
class MultiStringTypeTmpl: public MultiStringBase<StringType> {
public:
	using BaseType = MultiStringBase<StringType>;
	MultiStringTypeTmpl(StringType& object) : BaseType(object) {}
};

template<typename StringType>
class MultiStringTypeTmpl<StringType, true> : public MultiStringBase<StringType> {

public:

	using BaseType = MultiStringBase<StringType>;
	using Type = typename BaseType::Type;
	using ConstType = const Type;
	using CharType = typename Type::value_type;
	using Iterator = typename BaseType::Iterator;

	MultiStringTypeTmpl(StringType& object) : BaseType(object) {}

	/*
	 * Combines strings within the range and stores them in the current string.
	 */
	template<typename _ContIter>
	void Combine(_ContIter beg, _ContIter end) {

		_ContIter it = beg;
		uint32 l = 0;
		uint32 n = 0;
		while (it != end) {
			l += (*it).length() + ESpecialValues::STR_OFFSET;
			++it;
			++n;
		}
		BaseType::_Obj().clear();
		_Ctor(BaseType::_Obj(), n, l);
		it = beg;
		while (it != end) {
			if ((*it).size()) {
				uint32 len = (uint32) ((*it).size());
				CharType v[ESpecialValues::STR_OFFSET];
				v[0] = ESpecialValues::STX_CHAR;
				_Len(len, v + 1);
				BaseType::_Obj().insert(BaseType::_Obj().size(), v, 5);
				BaseType::_Obj().insert(BaseType::_Obj().size(), (*it));
			}
			++it;
		}

	}

	void Insert(uint32 i, ConstType& what) {
		NEX_ASSERT(i <= BaseType::Length());
		Iterator it = BaseType::Iterate();
		uint32 p = 0;
		for (; it && p < i; ++it, ++p)
			;
		if (!it)
			PushBack(what);
		else {
			NEX_ASSERT(BaseType::_Obj().size() > 0);
			if (!_IsCted(BaseType::_Obj())) {
				StringType temp = std::move(BaseType::_Obj());
				BaseType::_Obj().clear();
				_Ctor(BaseType::_Obj(), 2,
						BaseType::_Obj().size() + what.size() + ESpecialValues::STR_OFFSET * 2);
				_Here(ESpecialValues::HEAD_OFFSET, i == 0 ? what : temp);
				PushBack(i == 0 ? temp : what);
			} else {
				_SetLen(BaseType::Length() + 1);
				_Here(it.Location(), what);
			}
		}
	}

	void Replace(uint32 i, ConstType& what) {
		if (!BaseType::_IsCted(BaseType::_Obj())) {
			BaseType::_Obj() = what;
		} else {
			Iterator it = BaseType::Iterate();
			uint32 p = 0;
			for (; it && p < i; ++it, ++p)
				;
			if (it) {

			}
		}
	}

	void PushBack(ConstType& what) {
		if (!BaseType::_IsCted(BaseType::_Obj())) {
			uint32 sz = BaseType::_Obj().size() > 0;
			_CtDump();
			_Ctor(BaseType::_Obj(), sz + 1, (uint32)BaseType::_Obj().size() + (uint32)what.size() + ESpecialValues::STR_OFFSET);
		} else {
			_SetLen(BaseType::Length() + 1);
		}
		_Here((uint32)BaseType::_Obj().size(), what);
	}

	void PushFront(ConstType& what) {
		if (!BaseType::_IsCted(BaseType::_Obj())) {
			uint32 sz = BaseType::_Obj().size() > 0;
			_CtDump();
			_Ctor(BaseType::_Obj(), sz + 1, (uint32)BaseType::_Obj().size() + (uint32)what.size() + ESpecialValues::STR_OFFSET);
		} else {
			_SetLen(BaseType::Length() + 1);
		}
		_Here(ESpecialValues::HEAD_OFFSET, what);
	}

private:

	void _CtDump() {
		if (BaseType::_Obj().size() > 0) {
			// ubsert tge size beforehand
			CharType off[ESpecialValues::STR_OFFSET];
			off[0] = ESpecialValues::STX_CHAR;
			_Len((uint32) BaseType::_Obj().size(), off + 1);
			BaseType::_Obj().insert(0, off, ESpecialValues::STR_OFFSET);
		}
	}

	void _Here(uint32 l, ConstType& what) {
		CharType off[ESpecialValues::STR_OFFSET];
		off[0] = ESpecialValues::STX_CHAR;
		_Len((uint32) what.size(), off + 1);
		BaseType::_Obj().insert(l, off, ESpecialValues::STR_OFFSET);
		BaseType::_Obj().insert(l + ESpecialValues::STR_OFFSET, what);
	}

	void _SetLen(uint32 l) {
		CharType s[ESpecialValues::LEN_BYTE_SZ];
		_Len(l, s);
		BaseType::_Obj().replace(1, ESpecialValues::LEN_BYTE_SZ, s, ESpecialValues::LEN_BYTE_SZ);
	}

	static void _Len(uint32 l, CharType s[]) {
		s[0] = (CharType) ((l >> 24) & 0xff);
		s[1] = (CharType) ((l >> 16) & 0xff);
		s[2] = (CharType) ((l >> 8) & 0xff);
		s[3] = (CharType) ((l >> 0) & 0xff);
	}

	static void _Ctor(Type& ob, uint32 n, uint32 l) {
		ob.reserve(l + ESpecialValues::HEAD_OFFSET);
		CharType v[ESpecialValues::HEAD_OFFSET];
		v[0] = ESpecialValues::SOH_CHAR;
		_Len(n, v + 1);
		ob.insert(0, v, ESpecialValues::HEAD_OFFSET);
	}
};

template<typename StringType>
class MultiStringType: public MultiStringTypeTmpl<StringType,
		std::is_same<StringType, typename std::remove_const<StringType>::type>::value> {

	using BaseType = MultiStringTypeTmpl<StringType,
			std::is_same<StringType, typename std::remove_const<StringType>::type>::value>;
public:
	MultiStringType(StringType& object) : BaseType(object) {}
};

using ConstMultiString = MultiStringType<const String>;
using MultiString = MultiStringType<String>;

}

typedef StringUtils::MultiString MultiStringHelper;
typedef StringUtils::ConstMultiString ConstMultiStringHelper;

}

#endif // MULTISTRINGLIST_H_
