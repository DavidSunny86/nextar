/**
 * Created on: 18th Oct 15
 * @author: obhi
 **/
#ifndef  PARSERCONTEXT_H_
#define  PARSERCONTEXT_H_

#include <NeoBaseDecl.h>
#include <ParseLocation.h>

namespace nextar {

class ParserContext {
	NEX_LOG_HELPER(ParserContext);
public:
	enum ErrorType {
		BAD_ESCAPE_SEQ
	};

	ParserContext(InputStreamPtr input, const String& file,
			bool traceParsing, bool traceScan) :
			_input(input), _fileName(file), _traceParsing(traceParsing),
			_traceScanning(traceScan), _scanner(nullptr) {
	}

	void BeginScan();
	void EndScan();
	ASTDocumentPtr Parse();

	const String& GetFileName() const {
		return _fileName;
	}
	size_t Read(char* buffer, size_t maxSize);

	void StartContent() {
		_contents.clear();
	}

	void PutContent(int c) {
		_contents += (char) c;
	}
	void PutContent(const char* c) {
		_contents += c;
	}
	const String& GetContent() const {
		return _contents;
	}

	ParseLocation& Loc() {
		return _location;
	}
	const ParseLocation& Loc() const {
		return _location;
	}

	ASTDocumentPtr MakeDocument(ASTRegionPtr& region);
	ASTRegionPtr MakeBlockRegion(const String& name, ASTCommandList&& commands);
	ASTRegionPtr MakeTextRegion(const String& name, String&& contents);
	ASTBlockPtr MakeBlock(const String& name, ASTParameterList&& params,
			ASTCommandList&& commands);
	ASTCommandPtr MakeCommand(const String& name, ASTParameterList&& params);

	void Error(ErrorType type);
	void Error(const ParseLocation& l, const String& err);

	void* _scanner;

private:
	
	bool _traceParsing;
	bool _traceScanning;
	InputStreamPtr _input;
	ParseLocation _location;
	String _fileName;
	String _contents;
	ASTDocumentPtr _document;

};

}

#endif // PARSERCONTEXT_H_
