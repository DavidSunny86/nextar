/**
 * Created on: 5th Oct 15
 * @author: obhi
**/
#ifndef RENDERCONTEXTGL41_H_
#define RENDERCONTEXTGL41_H_

#include <RenderOpenGL.h>
#include <RenderContext_4_0_GL.h>

namespace RenderOpenGL {

class RenderContext_4_1_GL : public RenderContext_4_0_GL,
  public CoreFuncTableVersion41 {
    typedef RenderContext_4_0_GL ParentContext;
    typedef CoreFuncTableVersion41 FunctionTable;
public:
  RenderContext_4_1_GL(RenderDriverGL*);
  virtual VersionGL GetContextVersion();

protected:

  virtual void InitializeExtensions();
};

}

#endif // RENDERCONTEXTGL40_H_
