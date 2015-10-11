/**
 * Created on: 5th Oct 15
 * @author: obhi
**/
#ifndef RenderContext_Base_GL42_H_
#define RenderContext_Base_GL42_H_

#include <RenderOpenGL.h>
#include <RenderContext_4_1_GL.h>

namespace RenderOpenGL {

class RenderContext_4_2_GL : public RenderContext_4_1_GL,
  public CoreFuncTableVersion42 {
    typedef RenderContext_4_1_GL ParentContext;
    typedef CoreFuncTableVersion42 FunctionTable;
public:
  RenderContext_4_2_GL(RenderDriverGL*);
  virtual VersionGL GetContextVersion();

  virtual void AllocateTexture(GLenum target, GLint levels, GLenum format,
	  GLsizei width, GLsizei height, GLsizei depth);
protected:

  virtual void InitializeExtensions();
};

}

#endif // RenderContext_Base_GL40_H_
