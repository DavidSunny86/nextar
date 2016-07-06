
#ifndef NEXTAR_APPLICATIONTYPE_H__
#define NEXTAR_APPLICATIONTYPE_H__

#include <NexBase.h>

namespace nextar {

enum EApplicationContextType {
	// Unknown, plugins will be rejected from loading
	CONTEXT_UNKNOWN = -1,
	// Base context, bare services registered
	CONTEXT_BASE = 0,
	// Engine context, full simulation
	CONTEXT_ENGINE = 1,
	// Project context, used for 
	CONTEXT_PROJECT = 2,
	

};


struct _NexBaseAPI ApplicationContextType {
	EApplicationContextType _type;

	ApplicationContextType() : _type(CONTEXT_UNKNOWN) {}
	ApplicationContextType(const ApplicationContextType& c) : _type(c._type) {}
	ApplicationContextType(EApplicationContextType t) : _type(t) {}

	bool IsAccepted(EApplicationContextType type);
	String ToString() const;
	void FromString(const String& name);
};

}

#endif