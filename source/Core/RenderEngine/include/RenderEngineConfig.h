//@ Obi De
//@ Nov 25 2007

#ifndef NEXTAR_RENDERENGINE_CONFIG_1_01
#define NEXTAR_RENDERENGINE_CONFIG_1_01

#ifdef NEXTAR_EXPORT_AS_DYNAMIC_LIB
#	ifdef NE_Core_RenderEngine_EXPORTS
#		define _NexRenderAPI	_NexExportSym
#	else
#		define _NexRenderAPI	_NexImportSym
#	endif
#else
#	define _NexRenderAPI
#endif

#endif //NEXTAR_RENDERENGINE_CONFIG_1_01
