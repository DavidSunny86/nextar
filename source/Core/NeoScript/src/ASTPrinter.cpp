/*
 * ASTPrinter.cpp
 *
 *  Created on: 27-Oct-2015
 *      Author: obhi
 */
#include <NeoScriptHeaders.h>
#include <ASTPrinter.h>
#include <NeoScriptHeaders.h>

namespace nextar {

ASTPrinter::ASTPrinter(OutputStreamPtr& p) :
		_outputStream(p), _indention(0) {
}

ASTPrinter::~ASTPrinter() {
	String work = _out.str();
	_outputStream->Write(work.c_str(), work.length());
}

void ASTPrinter::VisitDocumentBegin(const ASTDocument* document) {
	_indention = 0;
	//Indent() << "/* {{ ASTPrint.1.1 - " << document->GetName() << " */\n";
}

void ASTPrinter::VisitDocumentEnd(const ASTDocument* document) {
	//Indent() << "/* ASTPrint.1.1 - " << document->GetName() << " }} */\n";
}

void ASTPrinter::VisitCommandBegin(const ASTCommand* command) {
	Indent() << command->GetName() << " ";
}

void ASTPrinter::VisitCommandEnd(const ASTCommand* command) {
	_out << ";\n";
}

void ASTPrinter::VisitBlockBegin(const ASTBlock* block) {
	_out << "{\n";
	_indention++;
}

void ASTPrinter::VisitBlockEnd(const ASTBlock* block) {
	_indention--;
	Indent() << "}\n";
}

void ASTPrinter::VisitBlockRegionBegin(const ASTBlockRegion* blockRegion) {
	if (blockRegion->GetName().length() > 0)
		_out << "[[" << blockRegion->GetName() << ".script]]\n";
}

void ASTPrinter::VisitBlockRegionEnd(const ASTBlockRegion* blockRegion) {
	//_out << "/* [[" << blockRegion->GetName() << ".script]] */\n";
}

void ASTPrinter::VisitTextRegionBegin(const ASTTextRegion* textRegion) {
	_out << "[[" << textRegion->GetName() << ".text]]";
	_out << textRegion->GetContents();
}

void ASTPrinter::VisitTextRegionEnd(const ASTTextRegion* textRegion) {
	//_out << "/* [[" << textRegion->GetName() << ".text]] */\n";
}

void ASTPrinter::PrintString(const String& value) {
	if (StringUtils::ConstMultiString::IsSimpleString(value))
		_out << "\"" << value << "\" ";
	else {
		StringUtils::ConstMultiString cms(value);
		uint32 l = cms.Length();
		if (l > 1) {
			// print list
			String subValue;
			auto it = cms.Iterate();
			bool isFirst = true;
			_out << "[ ";
			while (it.HasNext(subValue)) {
				if (isFirst)
					isFirst = false;
				else {
					_out << ", ";
				}
				PrintString(subValue);
			}
			_out << "] ";
		} else {
			// print a param
			PrintString(cms.Get(0));
		}
	}
}

void ASTPrinter::VisitParameter(const ASTParameter* parameter) {
	PrintString(parameter->AsString());
}

} /* namespace nextar */
