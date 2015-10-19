/**
 * Created on: 18th Oct 15
 * @author: obhi
**/

#ifndef NEXTAR_NEOSCRIPT_CONFIG_1_01
#define NEXTAR_NEOSCRIPT_CONFIG_1_01

#ifdef NEXTAR_EXPORT_AS_DYNAMIC_LIB
#	ifdef NE_Core_NeoScript_EXPORTS
#		define _NexNeoScriptAPI	_NexExportSym
#	else
#		define _NexNeoScriptAPI	_NexImportSym
#	endif
#else
#	define _NexNeoScriptAPI
#endif


#endif //NEXTAR_NEOSCRIPT_CONFIG_1_01
