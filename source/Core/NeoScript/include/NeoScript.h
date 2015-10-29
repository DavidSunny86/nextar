/*
 * NeoScript.h
 *
 *  Created on: 18-Oct-2015
 *      Author: obhi
 */

#ifndef CORE_NEOSCRIPT_INCLUDE_NEOSCRIPT_H_
#define CORE_NEOSCRIPT_INCLUDE_NEOSCRIPT_H_

#include <NeoBaseDecl.h>

namespace nextar {

class _NexNeoScriptAPI NeoScript {
	NEX_LOG_HELPER(NeoScript);
public:
	/**
	 * @returns a valid document or throws error EXCEPT_COMPILATION_FAILED
	 **/
	static ASTDocumentPtr AsyncParse(InputStreamPtr input, const String& sourceName, const String& options);
	static void AsyncPrint(ASTDocumentPtr doc, OutputStreamPtr out);
};

}
/* namespace nextar */

#endif /* CORE_NEOSCRIPT_INCLUDE_NEOSCRIPT_H_ */
