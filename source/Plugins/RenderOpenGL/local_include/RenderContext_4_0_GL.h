/**
 * Created on: 5th Oct 15
 * @author: obhi
**/
#ifndef RenderContext_Base_GL40_H_
#define RenderContext_Base_GL40_H_

#include <RenderOpenGL.h>
#include <RenderContext_Base_GL.h>

namespace RenderOpenGL {

class RenderContext_4_0_GL : public RenderContext_Base_GL,
  public CoreFuncTableVersion40 {
	typedef RenderContext_Base_GL ParentContext;
    typedef CoreFuncTableVersion40 FunctionTable;
public:
  RenderContext_4_0_GL(RenderDriverGL*);
  virtual VersionGL GetContextVersion();

protected:

  virtual void InitializeExtensions();
};

}

#endif // RenderContext_Base_GL40_H_
