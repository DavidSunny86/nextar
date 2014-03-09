/* 
 * File:   Property.cpp
 * Author: obhi
 * 
 * Created on March 5, 2011, 7:26 PM
 */
#include "BaseHeaders.h"
#include "PropertyInterface.h"

namespace nextar {

	/****************************************************************
	 * PropertyDictionaryManager
	 ****************************************************************/
	class PropertyDictionaryManager : public AllocGeneral {

	public:
		static PropertyDictionaryManager propertyDictionaryManager;

		PropertyDictionary* AsyncGetDictionary(PropertyDictionary* parent, const String& name) {
#if !defined(NEX_HAS_CONCURRENT_CONTAINERS)
			NEX_THREAD_LOCK_GUARD_MUTEX(dictionaryMtx);
#endif // !defined(NEX_HAS_CONCURRENT_CONTAINERS)

			PropertyDictionaryMap::iterator it = dictionaryMap.find(name);
			if (it != dictionaryMap.end()) {
				return (*it).second;
			}
			PropertyDictionary* dictionary = NEX_NEW PropertyDictionary(name, parent);
			dictionaryMap.insert(
					PropertyDictionaryMap::value_type(name, dictionary));
			return dictionary;
		}

		void RemoveDictionary(const String& name) {
#if !defined(NEX_HAS_CONCURRENT_CONTAINERS)
			NEX_THREAD_LOCK_GUARD_MUTEX(dictionaryMtx);
#endif // !defined(NEX_HAS_CONCURRENT_CONTAINERS)

			// none should be left by now
			PropertyDictionaryMap::iterator it = dictionaryMap.find(name);
			PropertyDictionaryMap::iterator end = dictionaryMap.end();
			if (it != end) {
				// erase
				NEX_DELETE(*it).second;
#if !defined(NEX_HAS_CONCURRENT_CONTAINERS)
				dictionaryMap.erase(it);
#else
				dictionaryMap.unsafe_erase(it);
#endif
			}
		}

		void DestroyDictionaries() {
#if !defined(NEX_HAS_CONCURRENT_CONTAINERS)
			NEX_THREAD_LOCK_GUARD_MUTEX(dictionaryMtx);
#endif // !defined(NEX_HAS_CONCURRENT_CONTAINERS)

			PropertyDictionaryMap::iterator it = dictionaryMap.begin();
			PropertyDictionaryMap::iterator end = dictionaryMap.end();
			while (it != end) {
				NEX_DELETE(*it).second;
				++it;
			}
			dictionaryMap.clear();
		}

	protected:
#if defined(NEX_HAS_CONCURRENT_CONTAINERS)
		/* Dictionary map */
		typedef concurrent_unordered_map<String, PropertyDictionary*>::type PropertyDictionaryMap;
		PropertyDictionaryMap dictionaryMap;
#else // defined(NEX_HAS_CONCURRENT_CONTAINERS)
		/* Dictionary map */
		typedef unordered_map<String, PropertyDictionary*>::type PropertyDictionaryMap;
		PropertyDictionaryMap dictionaryMap;
		NEX_THREAD_MUTEX(dictionaryMtx);
#endif // defined(NEX_HAS_CONCURRENT_CONTAINERS)
	};

	PropertyDictionaryManager PropertyDictionaryManager::propertyDictionaryManager;

	/****************************************************************
	 * PropertyDictionary
	 ****************************************************************/
	PropertyDictionary::PropertyDictionary(const String& _name,
			PropertyDictionary* pdict)
			: name(_name), parent(pdict) {

	}

	PropertyDictionary::~PropertyDictionary() {
	}
		
	/****************************************************************
	 * PropertyInterface
	 ****************************************************************/
	String PropertyInterface::GetParamValue(const String& name) const {
		const PropertyDictionary* dict = dictionary;
		while (dict) {
			const PropertyAccessorMap& m = dict->accessors;
			PropertyAccessorMap::const_iterator it = m.find(name);
			if (it != m.end()) {
				return (*it).second->GetStringValue(this);
			}
			dict = dict->parent;
		}
		return StringUtils::Null;
	}

	void PropertyInterface::SetParamValue(const String& param, const String& value) {
		PropertyDictionary* dict = dictionary;
		while (dict) {
			PropertyAccessorMap& m = dict->accessors;
			PropertyAccessorMap::iterator it = m.find(param);
			if (it != m.end()) {
				(*it).second->SetStringValue(this, value);
				return;
			}
			dict = dict->parent;
		}
	}

	void PropertyInterface::SetParamValues(const NameValueMap* nvm) {
		if (!dictionary || !nvm)
			return;
		NameValueMap::const_iterator it = nvm->begin();
		NameValueMap::const_iterator end = nvm->end();
		while (it != end) {
			SetParamValue((*it).first, (*it).second);
			++it;
		}
	}


	bool PropertyInterface::OverrideDictionary(const String& name) {
		if (dictionary && dictionary->name == name)
			return false;
		dictionary = PropertyDictionaryManager::propertyDictionaryManager.AsyncGetDictionary(dictionary, name);
		return true;
	}

	void PropertyInterface::RemoveDictionary(const String& name) {
		PropertyDictionaryManager::propertyDictionaryManager.RemoveDictionary(name);
	}

	void PropertyInterface::DestroyDictionaries() {
		PropertyDictionaryManager::propertyDictionaryManager.DestroyDictionaries();
	}
}
