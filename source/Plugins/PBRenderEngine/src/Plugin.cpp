/* 
 * File:   Plugin.cpp
 * Author: obhi
 */

#include <Plugin.h>
#include <Image.h>

NEX_IMPLEMENT_PLUGIN(PBRenderEngine, PBRenderEngine::Plugin);

namespace PBRenderEngine {

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
}

void Plugin::LicenseRenewed() {
}

bool Plugin::LicenseExpired() {
	return true;
}

void Plugin::Close() {
}

void Plugin::Dispose() {
}
}
