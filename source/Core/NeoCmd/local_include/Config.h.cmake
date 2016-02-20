/**
 * Created on: 18th Oct 15
 * @author: obhi
**/

#ifndef NEXTAR_NEOCMD_CONFIG_1_01
#define NEXTAR_NEOCMD_CONFIG_1_01

#ifdef NEXTAR_EXPORT_AS_DYNAMIC_LIB
#	ifdef @NEXTAR_LIBRARY_DEFINE@_EXPORTS
#		define _NexNeoCmdAPI	_NexExportSym
#	else
#		define _NexNeoCmdAPI	_NexImportSym
#	endif
#else
#	define _NexNeoCmdAPI
#endif


#endif //NEXTAR_NEOCMD_CONFIG_1_01
