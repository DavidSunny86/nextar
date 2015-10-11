/**
 * Created on: 5th Oct 15
 * @author: obhi
**/
#ifndef RenderContext_Base_GL41_H_
#define RenderContext_Base_GL41_H_

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

#endif // RenderContext_Base_GL40_H_
