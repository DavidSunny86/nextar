#ifndef PBRENDERSYSTEM_H
#define PBRENDERSYSTEM_H

#include <RenderPass.h>

namespace PBRenderEngine {

using namespace nextar;
  
class PBRenderPass : public nextar::RenderPass
{
public:
  PBRenderPass(const Config& cfg);
  virtual ~PBRenderPass();
  
  
  virtual void Commit(CommitContext& context);
};

  
}

#endif // PBRENDERSYSTEM_H
