/**
 * ParserTests.cpp
 * Created on: Mon, Oct 26, 2015 12:13:11 AM
 * @author: obhi
 */
 
#include <NeoScriptHeaders.h>
#include <UTDefs.h>

NEX_IMPLEMENT_PLATFORM_APP()
;
using namespace nextar;
class UTApplicationParserTest: public ACBaseImpl,
	public Archive::ScanCallback {
	String suffix;
	NEX_LOG_HELPER(UTApplicationParserTest);
public:
	UTApplicationParserTest()  {
	}

	void TestParser(const String& dir, const String& suff, const String& pattern) {
		suffix = suff;
		ArchivePtr arch = FileSystem::Instance().OpenArchive("EngineData");
		if (arch) {
			arch->Scan(this, dir + "/" + pattern);
		}
	}

	virtual void FoundFile(const FileAttribute& attribute, Archive* arch) {
		InputStreamPtr input = FileSystem::Instance().OpenRead(attribute.fileName);
		if (input) {
			try {
				Trace("Parsing: " + attribute.fileName.ToString());
				ASTDocumentPtr doc =
						NeoScript::AsyncParse(input, attribute.fileName.GetComputedName(), "");
				if (doc) {
					URL outUrl(attribute.fileName.ToString() + suffix);
					OutputStreamPtr out = FileSystem::Instance().OpenWrite(outUrl, false);
					NeoScript::AsyncPrint(doc, out);
				}
			} catch(GracefulErrorExcept& e) {
				Error(e.GetMsg());
			}
		}

	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationParserTest impl;
	nextar::ApplicationContext application("ParserTest", impl);
	application.InitializeContext(argc, argv);
	String inputDir("Scripts/NeoScriptUnits"), fileSuffix(".1.out"), searchPattern("*.neoscript");
	for(int i = 1; i < argc; ++i) {
		char* suffix = std::strstr(argv[i], "--suffix=");
		if (suffix) {
			suffix += sizeof("--suffix");
			fileSuffix = suffix;
		} else {
			char* out = std::strstr(argv[i], "--out=");
			if (out) {
				out += sizeof("--out");
				inputDir = out;
			} else {
				char* pattern = std::strstr(argv[i], "--pattern=");
				if (pattern) {
					pattern += sizeof("--pattern");
					searchPattern = pattern;
				}
			}
		}
	}
	impl.TestParser(inputDir, fileSuffix, searchPattern);
	application.DestroyContext();
	std::cin.get();
	return 0;
}
