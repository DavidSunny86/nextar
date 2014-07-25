//@ Obi De
//@ Nov 25 2007

#ifndef NEXTAR_PROJECTMODEL_CONFIG_1_01
#define NEXTAR_PROJECTMODEL_CONFIG_1_01

#ifdef NEXTAR_EXPORT_AS_DYNAMIC_LIB
#	ifdef NE_Core_ProjectModel_EXPORTS
#		define _NexProjectAPI	_NexExportSym
#	else
#		define _NexProjectAPI	_NexImportSym
#	endif
#else
#	define _NexProjectAPI
#endif

#endif //NEXTAR_PROJECTMODEL_CONFIG_1_01
