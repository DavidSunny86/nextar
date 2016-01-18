#ifndef PBRENDERSYSTEM_H
#define PBRENDERSYSTEM_H

#include <RenderSystem.h>

namespace PBRenderEngine {

using namespace nextar;
  
class PBRenderSystem : public nextar::RenderSystem
{
public:
  PBRenderSystem(const Config& cfg);
  virtual ~PBRenderSystem();
  
  
  virtual void Commit(CommitContext& context);
};

  
}

#endif // PBRENDERSYSTEM_H
