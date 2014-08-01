#ifndef COMMONTYPES_H_
#define COMMONTYPES_H_

#include <ProjectModelHeaders.h>
#include <ScriptParser.h>
#include <DepthStencilState.h>
#include <BlendState.h>
#include <RasterState.h>
#include <TextureUnitState.h>
#include <ShaderAsset.h>
#include <MaterialTemplate.h>

using namespace nextar;
namespace MaterialCompiler {

class CommandDelegate {
public:
	virtual void Execute(void* parentParam, ScriptParser::StatementContext& statement) = 0;
protected:
	~CommandDelegate() {}
};

struct CommandNamePair {
	const char* name;
	CommandDelegate* command;
};

struct CommandNameCompare {
	bool operator ()(const CommandNamePair& enp, const String& name) const {
		return name.compare(enp.name) > 0;
	}

	bool operator ()(const String& name, const CommandNamePair & enp) const {
		return name.compare(enp.name) < 0;
	}
};

class Helper {
public:
	static CommandDelegate* FindCommand(CommandNamePair cmdMap[],
			size_t arraySize, const String& name);
};

}

#endif //COMMONTYPES_H_
