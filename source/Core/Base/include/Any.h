#ifndef NEXTAR_NEXANY_H
#define NEXTAR_NEXANY_H

#include <NexBase.h>

/**
 * @remarks Concept from: Valued Conversions
 *			@par By: Kevlin Hennely
 *			@par http://www.two-sdg.demon.co.uk/curbralan/papers/ValuedConversions.pdf
 **/
namespace nextar {

/**
 * @class	class Any
 *
 * @date	01/02/2011
 * @author	Abhishek Dey
 *
 * @remarks This is the typesafe version for Any. Under debug build it will do checking for typesafety.
 *			Not so-much under normal build.
 **/
class _NexBaseAPI Any: public AllocGeneral {
private:

	class PlaceHolder {
	public:
		/** @remarks Produce a copy **/
		virtual PlaceHolder* Clone() const = 0;
		/** @remarks Dispose of this copy **/
		virtual void Dispose() = 0;
		/** @remarks Get type **/
		virtual debug::ConstTypeInfo GetTypeInfo() = 0;
	};

	template<typename ValueType, typename AllocatorBase = AllocGeneral>
	class Holder: public PlaceHolder, public AllocGeneral {
	public:

		ValueType value;
	public:

		inline Holder(const ValueType& val) :
				value(val) {
		}
		;

		virtual PlaceHolder* Clone() const {
			return NEX_NEW(Holder(value));
		}

		virtual void Dispose() {
			NEX_DELETE(this);
		}

		virtual debug::ConstTypeInfo GetTypeInfo() {
			return NEX_TYPE_ID(value);
		}
	};

private:
	PlaceHolder* contents;

public:

	inline Any() :
			contents(0) {
	}

	inline ~Any() {
		if (contents)
			contents->Dispose();
	}

	inline Any(const Any& other) :
			contents(other.contents ? other.contents->Clone() : 0) {
	}

	template<typename ValueType>
	inline Any(ValueType& v) :
			contents(NEX_NEW(Holder<ValueType>(v))) {
	}

	inline Any& Swap(Any& rhs) {
		std::swap(contents, rhs.contents);
		return *this;
	}

	inline Any & operator=(const Any& rhs) {
		Any t(rhs);
		return Swap(t);
	}

	template<typename ValueType>
	inline Any & operator=(const ValueType &rhs) {
		Any rhsany(rhs);
		return Swap(rhsany);
	}

	inline debug::ConstTypeInfo GetTypeInfo() const {
		return contents ? contents->GetTypeInfo() : NEX_TYPE_ID(void);
	}

	template<typename ValueType>
	inline bool CopyTo(ValueType &value) const {
		const ValueType *copyable = ToPtr<ValueType>();
		if (copyable)
			value = *copyable;
		return copyable;
	}

#ifdef NEX_DEBUG

	template<typename ValueType>
	inline const ValueType* ToPtr() const {
		return GetTypeInfo() == NEX_TYPE_ID(ValueType) ?
				&static_cast<Holder<ValueType>*>(contents)->value : 0;
	}
#else

	template<typename ValueType>
	inline const ValueType* ToPtr() const {
		return contents ? & static_cast<Holder<ValueType>*> (contents)->value : 0;
	}
#endif
};

template<typename value_type>
inline value_type any_cast(const Any &operand) {
	const value_type *result = operand.ToPtr<value_type>();
	if (result)
		return *result;
	NEX_THROW_FatalError(EXCEPT_BAD_CAST);
	return value_type();
}

template<typename value_type>
inline value_type any_cast(const Any* operand) {
	const value_type *result = operand->ToPtr<value_type>();
	if (result)
		return *result;
	NEX_THROW_FatalError(EXCEPT_BAD_CAST);
	return value_type();
}
}

#endif //NEXTAR_NEXANY_H
