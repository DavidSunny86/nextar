/**
 * Created on: 5th Oct 15
 * @author: obhi
**/
#ifndef RENDERCONTEXTGL40_H_
#define RENDERCONTEXTGL40_H_

#include <RenderOpenGL.h>
#include <RenderContext_3_3_GL.h>

namespace RenderOpenGL {

class RenderContext_4_0_GL : public RenderContext_3_3_GL,
  public CoreFuncTableVersion40 {
    typedef RenderContext_3_3_GL ParentContext;
    typedef CoreFuncTableVersion40 FunctionTable;
public:
  RenderContext_4_0_GL(RenderDriverGL*);
  virtual VersionGL GetContextVersion();

protected:

  virtual void InitializeExtensions();
};

}

#endif // RENDERCONTEXTGL40_H_
