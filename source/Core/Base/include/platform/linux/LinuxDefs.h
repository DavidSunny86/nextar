//@ Obi De
//@ Jan 1 08
#ifndef NEXTAR_LINUX_OSDEFS_H
#define NEXTAR_LINUX_OSDEFS_H

#include <BaseTypes.h>
#include <dlfcn.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>    /* X11 constant (e.g. TrueColor) */
#include <X11/keysym.h>
#include <unistd.h>
//---------------------------------------
//@ Implement application
//---------------------------------------
#define NEX_PLATFORM_APP()                              				\
int NextarMain(int argc,char* argv[]);                                  \
int main(int argc,char* argv[]) {										\
	NEX_PLATFORM_APP_BEGIN();											\
	int ret;															\
	NEX_PLATFORM_APP_END(argc, argv, ret);								\
	return ret;                                  						\
}

#define DYN_LIB_TYPE void*
#define NEX_DYN_LOAD(name)                 dlopen(name, RTLD_NOW)
#define NEX_DYN_ERR()                      dlerror()
#define NEX_DYN_FREE(libptr)               dlclose(libptr)
#define NEX_DYN_LOAD_SYMBOL(module,name)   dlsym(module,name)
#define NEX_MODULE_NAME(name)			   "lib" + name + ".so"

#define NEX_PLUGIN_OS_DEFINITION				

#endif //NEXTAR_LINUX_OSDEFS_H