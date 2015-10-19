/**
 * Created on: 11th Oct 15
 * @author: obhi
 **/

#ifndef NEXTAR_SCRIPTPARSER_H_
#define NEXTAR_SCRIPTPARSER_H_

#include <BaseHeaders.h>
#include <NeoScriptConfig.h>

namespace nextar {

class ASTNode;
class ASTCommand;
class ASTBlock;
class ASTRegion;
class ASTTextRegion;
class ASTBlockRegion;
class ASTDocument;
class ASTParameter;
class ASTParameterList;

NEX_DEFINE_REFPTR(ASTNode);
NEX_DEFINE_REFPTR(ASTCommand);
NEX_DEFINE_REFPTR(ASTBlock);
NEX_DEFINE_REFPTR(ASTRegion);
NEX_DEFINE_REFPTR(ASTTextRegion);
NEX_DEFINE_REFPTR(ASTBlockRegion);
NEX_DEFINE_REFPTR(ASTDocument);

typedef list<ASTCommandPtr>::type ASTCommandList;
typedef list<ASTRegionPtr>::type ASTRegionList;
typedef list<ASTBlockPtr>::type ASTBlockList;

}

#endif // NEXTAR_SCRIPTPARSER_H_
