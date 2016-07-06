
#include <ParserContext.h>
#include <NeoScriptHeaders.h>

namespace nextar {

size_t ParserContext::Read(char* buffer, size_t max) {
	size_t read = _input->Read(buffer, max-1);
	NEX_ASSERT(read < max);
	buffer[read] = 0;
	return read;
}

ASTDocumentPtr ParserContext::MakeDocument(ASTRegionPtr& region) {
	if (!_document) {
		_document = Assign(NEX_NEW(ASTDocument(GetFileName())));
	}
	_document->AddRegion(region);
	return _document;
}

ASTRegionPtr ParserContext::MakeBlockRegion(const String& name,
		ASTCommandList&& block) {
	ASTRegionPtr region = Assign<ASTRegion>(NEX_NEW(ASTBlockRegion(name)));
	region->AddCommands(std::move(block));
	return region;
}

ASTRegionPtr ParserContext::MakeTextRegion(const String& name,
		String&& contents) {
	ASTRegionPtr region = Assign<ASTRegion>(NEX_NEW(ASTTextRegion(name)));
	region->SetContents(std::move(contents));
	return region;
}

ASTBlockPtr ParserContext::MakeBlock(const String& name,
		ASTParameter&& params, ASTCommandList&& commands) {
	return Assign(NEX_NEW(ASTBlock(name, std::move(params), std::move(commands))));
}

ASTCommandPtr ParserContext::MakeCommand(const String& name,
		ASTParameter&& params) {
	return Assign(NEX_NEW(ASTCommand(name, std::move(params))));
}

void ParserContext::Error(const ParseLocation& l, const String& err) {
	Error(l.AsString() + ":" + err);
}

void ParserContext::Error(ParserContext::ErrorType type) {
	switch(type) {
	case BAD_ESCAPE_SEQ:
		Error(_location.AsString() + ":" + "BAD_ESCAPE_SEQ");
		break;
	}
}

}
