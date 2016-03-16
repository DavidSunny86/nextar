/*
 * NeoCommandInterpreter.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#include <NeoCmd.h>
#include <CommandDictionaryArchive.h>
#include <NeoCommandInterpreter.h>
#include <CommandContext.h>
#include <CommandVisitor.h>

namespace nextar {

void NeoCommandInterpreter::Execute(const String& dictionaryName,
		InputStreamPtr& stream, CommandContext* context) {

	context->SetDictionary(
			CommandDictionaryArchive::Instance().Get(dictionaryName)
		);

	try {
		ASTDocumentPtr document = NeoScript::AsyncParse(stream, dictionaryName, StringUtils::Null);
		if (document) {
			context->SetDocument(document);
			CommandVisitor visitor(context);
			document->Accept(&visitor);
		}
	} catch (Exception& e) {
		Error("Failed to parse render pass script!");
		throw e;
	}
}

} /* namespace nextar */
