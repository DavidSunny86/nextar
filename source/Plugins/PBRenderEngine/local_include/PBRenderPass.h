#ifndef PBRENDERSYSTEM_H
#define PBRENDERSYSTEM_H

#include <BaseRenderPass.h>

namespace PBRenderEngine {

using namespace nextar;
  
class PBRenderPass : public nextar::BaseRenderPass
{
public:
  PBRenderPass(const Meta* );
  virtual ~PBRenderPass();
  
  
  virtual void Commit(CommitContext& context);
};

  
}

#endif // PBRENDERSYSTEM_H
