/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */
#include <EngineHeaders.h>
#include <SimpleBehaviours.h>
#include <Plugin.h>

NEX_IMPLEMENT_PLUGIN(SimpleBehaviours, SimpleBehaviours::Plugin);

namespace SimpleBehaviours {

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loaded simple behaviors 1.0.");
}

void Plugin::Close() {
	Trace("Unloading simple behaviours 1.0.");
}

void Plugin::LicenseRenewed() {
	if (ComponentFactoryArchive::InstancePtr())
		ComponentFactoryArchive::Instance().AsyncAddFactory(FPSControlBehaviour::CLASS_ID,
			NEX_NEW(FPSControlBehaviour::Factory(StringUtils::DefaultID)));
}

bool Plugin::LicenseExpired() {
	if (ComponentFactoryArchive::InstancePtr())
		ComponentFactoryArchive::Instance().AsyncRemoveFactory(FPSControlBehaviour::CLASS_ID,
			StringUtils::DefaultID, true);
	return true;
}

void Plugin::Dispose() {
}
} /* namespace ShaderCompiler */
