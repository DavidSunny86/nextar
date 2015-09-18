/**
 * Created on: 17th Sep 15
 * @author: obhi
**/
#ifndef BUGGY_H_
#define BUGGY_H_

#include <EngineHeaders.h>
#include <UTApplication.h>

using namespace nextar;

class Buggy : public UTApplication {
protected:
  Component* cameraEntity;
public:
  virtual void _SetupScene(SceneAssetPtr& scene);
  virtual void Execute(const FrameTimer& frameTimer) override;
};

#endif // BUGGY_H_
