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
class UTApplicationParserTest: public ApplicationContext,
	public Archive::ScanCallback {
	String suffix;
	NEX_LOG_HELPER(UTApplicationParserTest);
public:
	UTApplicationParserTest(const String& name) : ApplicationContext(name) {
	}

	void TestParser(const String& dir, const String& suff) {
		suffix = suff;
		ArchivePtr arch = FileSystem::Instance().OpenArchive("EngineData");
		if (arch) {
			arch->Scan(this, dir + "/*.neoscript");
		}
	}

	virtual void FoundFile(const FileAttribute& attribute, Archive* arch) {
		InputStreamPtr input = FileSystem::Instance().OpenRead(attribute.fileName);
		if (input) {
			try {
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
	UTApplicationParserTest application("ParserTest");
	application.InitializeContext(argc, argv);
	String inputDir("Scripts/NeoScriptUnits"), fileSuffix("1.out");
	for(int i = 1; i < argc; ++i) {
		char* suffix = std::strstr(argv[i], "--suffix=");
		if (suffix) {
			suffix += sizeof("--suffix=");
			fileSuffix = suffix;
		} else {
			char* out = std::strstr(argv[i], "--out=");
			if (out) {
				out += sizeof("--out=");
				inputDir = out;
			}
		}
	}
	application.TestParser(inputDir, fileSuffix);
	application.DestroyContext();
	std::cin.get();
	return 0;
}
