/**
 * Created on: 5th Oct 15
 * @author: obhi
**/
#ifndef RENDERCONTEXT33_H_
#define RENDERCONTEXT33_H_

#include <RenderOpenGL.h>
#include <RenderContext_Base_GL.h>

namespace RenderOpenGL {


class RenderContext_3_3_GL : public RenderContext_Base_GL,
  public CoreFuncTableVersion33 {
    typedef RenderContext_Base_GL ParentContext;
    typedef CoreFuncTableVersion33 FunctionTable;
public:

    RenderContext_3_3_GL(RenderDriverGL*);
    virtual VersionGL GetContextVersion();

protected:

  virtual void InitializeExtensions();
};

}

#endif // RENDERCONTEXT33_H_
