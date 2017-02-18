#ifndef ASTPARAMETER_H_
#define ASTPARAMETER_H_

#include <ASTNode.h>

namespace nextar {

class _NexNeoScriptAPI ASTParameter : public AllocGeneral {
public:
	
	class _NexNeoScriptAPI Resolver {
	public:
		inline Resolver() {}

		inline void SetParamNames(StringUtils::WordList&& names) {
			templateParamNames = std::move(names);
		}

		inline void SetParamValues(StringUtils::WordList&& values) {
			templateParamValues = std::move(values);
		}

		inline void Resolve(String& param) const {
			if (param.length() > 0 && param[0] == '$') {
				const char* name = param.c_str() + 1;
				String value;
				auto nameIt = ConstMultiStringHelper::It(templateParamNames);
				auto valueIt = ConstMultiStringHelper::It(templateParamValues);
				while (nameIt.HasNext(value)) {
					if (value == name) {
						valueIt.HasNext(param);
						return;
					}
					else
						++valueIt;
				}
			}
		}
	protected:
		StringUtils::WordList templateParamValues;
		StringUtils::WordList templateParamNames;
	};

	class _NexNeoScriptAPI DefaultResolver {
	public:
		static DefaultResolver _instance;
		inline void Resolve(String& param) const {}
	};

	template <typename ResolverType>
	class Iterator {
	public:

		typedef Iterator<ResolverType> Type;

		Iterator(const Iterator& it) : _res(it._res), _ref(it._ref) {}
		Iterator(const Iterator&& it) : _res(it._res), _ref(it._ref) {}
		Iterator(const ConstMultiStringHelper::Iterator& ref, const ResolverType& res) : _res(&res), _ref(ref) {}

		String GetTaggedVal(const String& tag) {
			return StringUtils::GetTaggedVal(tag, _ref);
		}

		bool HasNext(StringPair& nameValue) {
			String value;
			if (_ref.HasNext(value)) {
				StringUtils::Split(nameValue, value);
				_res->Resolve(nameValue.second);
				return true;
			}
			return false;
		}

		bool HasNext(String& value) {
			if (_ref.HasNext(value)) {
				_res->Resolve(value);
				return true;
			}
			return false;
		}

		inline Type& operator = (const Type& it) {
			_res = it._res;
			_ref = it._ref;
			return *this;
		}

	protected:
		const ResolverType* _res;
		ConstMultiStringHelper::Iterator _ref;
	};

	ASTParameter() {}
	ASTParameter(const String& value) : _value(value) {}
	ASTParameter(const ASTParameter& p) : _value(p._value) {}
	ASTParameter(ASTParameter&& p) : _value(std::move(p._value)) {}

	ASTParameter& operator = (const String& p) {
		_value = p;
		return *this;
	}

	ASTParameter& operator = (const ASTParameter& p) {
		_value = p._value;
		return *this;
	}
	ASTParameter& operator = (ASTParameter&& p) {
		_value = std::move(p._value);
		return *this;
	}

	Iterator<DefaultResolver> Iterate() const {
		return Iterator<DefaultResolver>(ConstMultiStringHelper::It(_value), DefaultResolver::_instance);
	}

	template<typename Resolver>
	Iterator<Resolver> Iterate(const Resolver& res) const {
		return Iterator<Resolver>(ConstMultiStringHelper::It(_value), res);
	}

	/*
	String& AsStringRef() {
		return _value;
	}

	const String& AsString() const {
		return _value;
	}*/

	void SetName(const String& name);
	
	void Append(const String& value);
	void Append(const ASTParameter& value);
	void AppendList(const ASTParameter& value);
	void AppendExpanded(const ASTParameter& value);
	bool Find(const String& name, String& oValue, const Resolver* pRes = nullptr) const;

protected:
	String _value;

};

}

#endif // ASTPARAMETER_H_
