#include <BaseHeaders.h>

#if defined(NEX_WINDOWS)

#include <Win32InputListener.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(Win32InputListener);
Win32InputListener::Win32InputListener() {
}

Win32InputListener::~Win32InputListener() {
}

}

#endif

