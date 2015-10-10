/**
 * Created on: 5th Oct 15
 * @author: obhi
**/
#ifndef RENDERCONTEXTGL42_H_
#define RENDERCONTEXTGL42_H_

#include <RenderOpenGL.h>
#include <RenderContext_4_4_GL.h>

namespace RenderOpenGL {

class RenderContext_4_5_GL : public RenderContext_4_4_GL,
  public CoreFuncTableVersion45 {
    typedef RenderContext_4_4_GL ParentContext;
    typedef CoreFuncTableVersion45 FunctionTable;
public:
  RenderContext_4_5_GL(RenderDriverGL*);
  virtual VersionGL GetContextVersion();

protected:
  virtual void BindNamedFBO(FrameBufferObjectGL& fbo, bool readDraw, FrameBuffer fb);
  virtual void UnbindNamedFBO(bool readDraw);

  virtual void InitializeExtensions();
};

}

#endif // RENDERCONTEXTGL40_H_
