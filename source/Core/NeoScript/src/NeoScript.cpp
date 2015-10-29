/*
 * NeoScript.cpp
 *
 *  Created on: 18-Oct-2015
 *      Author: obhi
 */

#include <NeoScript.h>
#include <ParserContext.h>
#include <NeoScriptHeaders.h>
#include <ASTPrinter.h>

namespace nextar {

ASTDocumentPtr NeoScript::AsyncParse(InputStreamPtr input, const String& sourceName,
  const String& options) {
  bool traceParsing = false;
  bool traceScanning = false;
  if (options.find("--trace-scan") != String::npos)
    traceScanning = true;
  else if (options.find("--trace-parse") != String::npos)
      traceParsing = true;
  else if (options.find("--trace") != String::npos)
    traceParsing = traceScanning = true;
  ParserContext p(input, sourceName, traceParsing, traceScanning);
  ASTDocumentPtr ret = p.Parse();
  if(!ret) {
    NEX_THROW_GracefulError(EXCEPT_COMPILATION_FAILED);
  }
  return ret;
}

void NeoScript::AsyncPrint(ASTDocumentPtr doc, OutputStreamPtr out) {
	ASTPrinter o(out);
	doc->Accept(&o);
}

} /* namespace nextar */
