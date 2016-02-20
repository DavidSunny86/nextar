/*
 * NeoCommandInterpreter.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#include <CommandDictionaryArchive.h>
#include <NeoCommandInterpreter.h>

namespace nextar {

void NeoCommandInterpreter::Execute(const String& dictionaryName,
		InputStreamPtr& stream, CommandContext* context) {

	context->SetDictionary(
			CommandDictionaryArchive::Instance().Get(dictionaryName)
		);

	try {
		ASTDocumentPtr document = NeoScript::AsyncParse(stream, "PassScript", StringUtils::Null);
		if (document) {
			CommandVisitor visitor(context);
			document->Accept(&visitor);
		}
	} catch (Exception& e) {
		Error("Failed to parse render pass script!");
		throw e;
	}
}

} /* namespace nextar */
