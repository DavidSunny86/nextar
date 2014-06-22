#include <BaseHeaders.h>

#if defined(NEX_WINDOWS)

#include <WmInputListener.h>

namespace nextar {

	NEX_DEFINE_SINGLETON_PTR(WmInputListener);
	WmInputListener::WmInputListener() {
	}

	WmInputListener::~WmInputListener() {
	}
}

#endif

