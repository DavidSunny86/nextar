/**
 * Created on: 5th Oct 15
 * @author: obhi
**/
#ifndef RENDERCONTEXTGL42_H_
#define RENDERCONTEXTGL42_H_

#include <RenderOpenGL.h>
#include <RenderContext_4_3_GL.h>

namespace RenderOpenGL {

class RenderContext_4_4_GL : public RenderContext_4_3_GL,
  public CoreFuncTableVersion44 {
    typedef RenderContext_4_3_GL ParentContext;
    typedef CoreFuncTableVersion44 FunctionTable;
public:
  RenderContext_4_4_GL(RenderDriverGL*);
  virtual VersionGL GetContextVersion();

protected:

  virtual void InitializeExtensions();
};

}

#endif // RENDERCONTEXTGL40_H_
