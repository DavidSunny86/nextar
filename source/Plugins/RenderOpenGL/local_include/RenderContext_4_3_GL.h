/**
 * Created on: 5th Oct 15
 * @author: obhi
**/
#ifndef RENDERCONTEXTGL42_H_
#define RENDERCONTEXTGL42_H_

#include <RenderOpenGL.h>
#include <RenderContext_4_2_GL.h>

namespace RenderOpenGL {

class RenderContext_4_3_GL : public RenderContext_4_2_GL,
  public CoreFuncTableVersion43 {
    typedef RenderContext_4_2_GL ParentContext;
    typedef CoreFuncTableVersion43 FunctionTable;
public:
  RenderContext_4_3_GL(RenderDriverGL*);
  virtual VersionGL GetContextVersion();

protected:

  virtual void InitializeExtensions();
};

}

#endif // RENDERCONTEXTGL40_H_
