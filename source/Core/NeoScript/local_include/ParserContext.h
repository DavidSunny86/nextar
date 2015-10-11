#ifndef PARSERCONTEXT_H_
#define PARSERCONTEXT_H_`

#include <MemoryArena.h>
#include <ScriptParser.h>

namespace nextar {

class ParserContext {

private:
  ASTDocument* _document;
  
  MemoryArena<ASTTextRegion> _textRegionAllocator;
  MemoryArena<ASTBlockRegion> _blockRegionAllocator;
  MemoryArena<ASTParameter> _parameterAllocator;
  MemoryArena<ASTCommand> _commandAllocator;
  MemoryArena<ASTBlock> _blockAllocator;
};

}

#endif // PARSERCONTEXT_H_
