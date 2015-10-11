/**
 * Created on: 11th Oct 15
 * @author: obhi
**/

#ifndef NEXTAR_SCRIPTPARSER_H_
#define NEXTAR_SCRIPTPARSER_H_

#include <BaseHeaders.h>

namespace nextar {

class ASTCommand;
class ASTParameter;
class ASTBlock;
class ASTRegion;
class ASTTextRegion;

typedef list<ASTCommand*>::type ASTCommandList;
typedef list<ASTRegion*>::type ASTRegionList;
typedef list<ASTBlock*>::type ASTBlockList;
}


#endif // NEXTAR_SCRIPTPARSER_H_
