/**
 * Created on: 5th Oct 15
 * @author: obhi
**/
#ifndef RenderContext_Base_GL44_H_
#define RenderContext_Base_GL44_H_

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

#endif // RenderContext_Base_GL44_H_
