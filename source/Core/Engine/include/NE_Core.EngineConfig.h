//@ Obi De
//@ Nov 25 2007

#ifndef NEXTAR_ENGINE_CONFIG_1_01
#define NEXTAR_ENGINE_CONFIG_1_01

#ifdef NEXTAR_EXPORT_AS_DYNAMIC_LIB
#	ifdef NE_Core_Engine_EXPORTS
#		define _NexEngineAPI	_NexExportSym
#	else
#		define _NexEngineAPI	_NexImportSym
#	endif
#else
#	define _NexEngineAPI
#endif

#endif //NEXTAR_ENGINE_CONFIG_1_01
