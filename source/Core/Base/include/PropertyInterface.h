/* 
 * File:   Property.h
 * Author: obhi
 *
 * Created on March 5, 2011, 7:26 PM
 */

#ifndef PROPERTY_H
#define	PROPERTY_H

#include <NexBase.h>
#include <StringUtils.h>
#include <NameValueMap.h>

namespace nextar {

class PropertyInterface;

enum PropertyType {
	PROP_TYPE_BOOL,
	PROP_TYPE_INT,
	PROP_TYPE_UINT,
	PROP_TYPE_ENUM,
	PROP_TYPE_FLOAT,
	PROP_TYPE_STRING,
	PROP_TYPE_VECTOR3,
	PROP_TYPE_VECTOR4,
	PROP_TYPE_MATRIX4x4,
};
/**
 * @remarks
 * Objects deriving from property should initialize
 * accessors into the mutator object supporting it.
 */
class PropertyAccessor {
public:
	/**
	 * Set a value as a string
	 * @param intf The property interface
	 * @param value The value of the parameter.
	 */
	virtual void SetStringValue(PropertyInterface* intf,
			const String& value) = 0;
	/**
	 * Gets a value as a string
	 * @param intf The property interface
	 * @param instance The instance of the parameter to use, for configs.
	 * This is generally Null.
	 * @return
	 */
	virtual const String GetStringValue(
			const PropertyInterface* intf) const = 0;
protected:
	~PropertyAccessor() {}
};

typedef unordered_map<String, PropertyAccessor*>::type PropertyAccessorMap;

struct PropertyDesc {
	String name;
	String description;
	PropertyType type;

	PropertyDesc(const String& pname, const String& desc, PropertyType ptype) :
			name(pname), description(desc), type(ptype) {
	}
};

typedef list<PropertyDesc>::type PropertyList;

class PropertyDictionary: public AllocGeneral {
	String name;
	PropertyDictionary* parent;
	PropertyList properties;
	PropertyAccessorMap accessors;

	friend class PropertyInterface;

public:
	PropertyDictionary(const String&, PropertyDictionary*);
	~PropertyDictionary();

	_NexInline const String& GetName() {
		return name;
	}

	_NexInline void AddParam(const String& name, PropertyAccessor* accessor,
			PropertyType type, const String& desc = StringUtils::Null) {
		accessors.insert(PropertyAccessorMap::value_type(name, accessor));
		PropertyDesc propDesc(name, desc, type);
		properties.push_back(propDesc);
	}

};

/**
 * Classes supporitng property accessors should
 * inherit from this class.
 */
class _NexBaseAPI PropertyInterface {
	PropertyDictionary* dictionary;

public:

	PropertyInterface() :
			dictionary(0) {
	}

	virtual ~PropertyInterface() {
	}

	PropertyDictionary* GetDictionary() {
		return dictionary;
	}

	const PropertyDictionary* GetDictionary() const {
		return dictionary;
	}

	/**
	 * Override the existing dictionary with this new one.
	 * If there is no dictionary create a new one. If this is
	 * a new dictionary true will be returned, upon which it
	 * must be ensured that the dictionary is populated.
	 * This function can be called from class constructor and
	 * is generally done so, If the dictionary is not populated
	 * right after this call then there is a chance it will get
	 * overriden somewhere else (like in an inheriting constructor).
	 * So doing an assertion that the requested dictionary is the
	 * same as name suggests is mandatory.
	 * @par
	 * The property interface can even override variables that are already
	 * registered via the base class.
	 * @param name The name of the new dictionary.
	 * @returns true if a new dictionary was created.
	 */
	bool OverrideDictionary(const String& name);

	virtual String GetParamValue(const String& name) const;
	virtual void SetParamValue(const String& param, const String& value);
	virtual void SetParamValues(const NameValueMap*);

	/**
	 * This should be called by every plugin before it quits in case
	 * it has registered any new dictionaries.
	 */
	static void RemoveDictionary(const String&);
	static void DestroyDictionaries();
};
}
#endif	/* PROPERTY_H */

