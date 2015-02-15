DECL_COND_START_VERSION(GLV_4_5)
DECL_EXTENSION(NAMEDFRAMEBUFFERDRAWBUFFER, NamedFramebufferDrawBuffer)
DECL_EXTENSION(NAMEDFRAMEBUFFERREADBUFFER, NamedFramebufferReadBuffer)
DECL_COND_END_VERSION(GLV_4_5)
/* GLE_4_3 */
/* GLE_4_3 */

/* GLE_3_3 */
DECL_COND_START_VERSION(GLV_3_3)
DECL_EXTENSION(SAMPLERPARAMETERF, SamplerParameterf)
DECL_EXTENSION(SAMPLERPARAMETERFV, SamplerParameterfv)
DECL_EXTENSION(SAMPLERPARAMETERI, SamplerParameteri)
DECL_EXTENSION(GENSAMPLERS, GenSamplers)
DECL_EXTENSION(BINDSAMPLER, BindSampler)
DECL_EXTENSION(DELETESAMPLERS, DeleteSamplers)
DECL_COND_END_VERSION(GLV_3_3)

/* GLE_3_2 */
DECL_COND_START_VERSION(GLV_3_2)
DECL_EXTENSION(CLIENTWAITSYNC, ClientWaitSync)
DECL_EXTENSION(FENCESYNC, FenceSync)
DECL_EXTENSION(DELETESYNC, DeleteSync)
DECL_EXTENSION(DRAWELEMENTSBASEVERTEX,DrawElementsBaseVertex)
DECL_EXTENSION(DRAWELEMENTSINSTANCEDBASEVERTEX, DrawElementsInstancedBaseVertex)
DECL_COND_END_VERSION(GLV_3_2)

DECL_COND_START_VERSION(GLV_3_1)
DECL_EXTENSION(VERTEXATTRIBIPOINTER, VertexAttribIPointer)
DECL_EXTENSION(DELETERENDERBUFFERS, DeleteRenderbuffers)
DECL_EXTENSION(DELETEFRAMEBUFFERS, DeleteFramebuffers)
DECL_EXTENSION(CHECKFRAMEBUFFERSTATUS, CheckFramebufferStatus)
DECL_EXTENSION(FRAMEBUFFERTEXTURE1D,FramebufferTexture1D)
DECL_EXTENSION(FRAMEBUFFERTEXTURE2D,FramebufferTexture2D)
DECL_EXTENSION(FRAMEBUFFERTEXTURELAYER,FramebufferTextureLayer)
DECL_EXTENSION(FRAMEBUFFERRENDERBUFFER, FramebufferRenderbuffer)
DECL_EXTENSION(RENDERBUFFERSTORAGE, RenderbufferStorage)
DECL_EXTENSION(RENDERBUFFERSTORAGEMULTISAMPLE, RenderbufferStorageMultisample)
DECL_EXTENSION(BINDRENDERBUFFER, BindRenderbuffer)
DECL_EXTENSION(BINDFRAMEBUFFER, BindFramebuffer)
DECL_EXTENSION(GENRENDERBUFFERS, GenRenderbuffers)
DECL_EXTENSION(GENFRAMEBUFFERS, GenFramebuffers)
DECL_EXTENSION(COPYBUFFERSUBDATA, CopyBufferSubData)
DECL_EXTENSION(DRAWELEMENTSINSTANCED, DrawElementsInstanced)
DECL_EXTENSION(DRAWARRAYSINSTANCED, DrawArraysInstanced)
DECL_EXTENSION(GETACTIVEUNIFORMBLOCKNAME, GetActiveUniformBlockName)
DECL_EXTENSION(GETACTIVEUNIFORMBLOCKIV, GetActiveUniformBlockiv)
DECL_EXTENSION(GETUNIFORMBLOCKINDEX, GetUniformBlockIndex)
DECL_EXTENSION(UNIFORMBLOCKBINDING, UniformBlockBinding)
DECL_EXTENSION(BINDBUFFERRANGE, BindBufferRange)
DECL_EXTENSION(BINDBUFFERBASE, BindBufferBase)
DECL_EXTENSION(MAPBUFFERRANGE, MapBufferRange)
DECL_COND_END_VERSION(GLV_3_1)

DECL_COND_START_VERSION(GLV_3_0)
DECL_EXTENSION(BLITFRAMEBUFFER, BlitFramebuffer)
DECL_COND_END_VERSION(GLV_3_0)

DECL_COND_START_VERSION(GLV_2_0)
DECL_EXTENSION(DRAWBUFFERS, DrawBuffers)
DECL_EXTENSION(GETUNIFORMLOCATION,GetUniformLocation)
DECL_EXTENSION(BLENDEQUATIONSEPARATE, BlendEquationSeparate)
DECL_EXTENSION(USEPROGRAM, UseProgram)
DECL_EXTENSION(VERTEXATTRIBPOINTER, VertexAttribPointer)
DECL_EXTENSION(ENABLEVERTEXATTRIBARRAY, EnableVertexAttribArray)
DECL_EXTENSION(DISABLEVERTEXATTRIBARRAY, DisableVertexAttribArray)
DECL_EXTENSION(STENCILMASKSEPARATE, StencilMaskSeparate)
DECL_EXTENSION(STENCILFUNCSEPARATE, StencilFuncSeparate)
DECL_EXTENSION(STENCILOPSEPARATE, StencilOpSeparate)
DECL_EXTENSION(CREATESHADER, CreateShader)
DECL_EXTENSION(SHADERSOURCE, ShaderSource)
DECL_EXTENSION(COMPILESHADER, CompileShader)
DECL_EXTENSION(GETSHADERIV, GetShaderiv)
DECL_EXTENSION(DELETESHADER, DeleteShader)
DECL_EXTENSION(ATTACHSHADER, AttachShader)
DECL_EXTENSION(DETACHSHADER, DetachShader)
DECL_EXTENSION(CREATEPROGRAM, CreateProgram)
DECL_EXTENSION(DELETEPROGRAM, DeleteProgram)
DECL_EXTENSION(GETPROGRAMIV, GetProgramiv)
DECL_EXTENSION(GETPROGRAMINFOLOG, GetProgramInfoLog)
DECL_EXTENSION(GETATTRIBLOCATION, GetAttribLocation)
DECL_EXTENSION(BINDATTRIBLOCATION, BindAttribLocation)
DECL_EXTENSION(GETACTIVEATTRIB, GetActiveAttrib)
DECL_EXTENSION(UNIFORM1F, Uniform1f)
DECL_EXTENSION(UNIFORM1I, Uniform1i)
DECL_EXTENSION(UNIFORM4F, Uniform4f)
DECL_EXTENSION(UNIFORM3F, Uniform3f)
DECL_EXTENSION(UNIFORMMATRIX4FV, UniformMatrix4fv)
DECL_EXTENSION(GETACTIVEUNIFORMSIV, GetActiveUniformsiv)
DECL_EXTENSION(GETACTIVEUNIFORMNAME, GetActiveUniformName)
DECL_EXTENSION(LINKPROGRAM, LinkProgram)
DECL_EXTENSION(VALIDATEPROGRAM, ValidateProgram)
DECL_EXTENSION(GETSHADERINFOLOG, GetShaderInfoLog)
DECL_COND_END_VERSION(GLV_2_0)

DECL_COND_START_VERSION(GLV_1_5)
DECL_EXTENSION(GENBUFFERS, GenBuffers)
DECL_EXTENSION(BINDBUFFER, BindBuffer)
DECL_EXTENSION(BUFFERDATA, BufferData)
DECL_EXTENSION(BUFFERSUBDATA, BufferSubData)
DECL_EXTENSION(DELETEBUFFERS, DeleteBuffers)
DECL_EXTENSION(MAPBUFFER, MapBuffer)
DECL_EXTENSION(UNMAPBUFFER, UnmapBuffer)
DECL_COND_END_VERSION(GLV_1_5)

DECL_COND_START_VERSION(GLV_1_4)
DECL_EXTENSION(BLENDFUNCSEPARATE, BlendFuncSeparate)
DECL_EXTENSION(ACTIVETEXTURE, ActiveTexture)
DECL_EXTENSION(TEXIMAGE3D, TexImage3D)
DECL_EXTENSION(GENERATEMIPMAP, GenerateMipmap)
DECL_COND_END_VERSION(GLV_1_4)

DECL_COND_START_VERSION(GLV_1_3)
DECL_EXTENSION(COMPRESSEDTEXIMAGE1D, CompressedTexImage1D)
DECL_EXTENSION(COMPRESSEDTEXIMAGE2D, CompressedTexImage2D)
DECL_EXTENSION(COMPRESSEDTEXIMAGE3D, CompressedTexImage3D)
DECL_EXTENSION(TEXSUBIMAGE3D, TexSubImage3D)
DECL_COND_END_VERSION(GLV_1_3)

DECL_COND_START_EXT(GL_,ARB_vertex_array_object)
DECL_EXTENSION(DELETEVERTEXARRAYS, DeleteVertexArrays)
DECL_EXTENSION(GENVERTEXARRAYS, GenVertexArrays)
DECL_EXTENSION(BINDVERTEXARRAY, BindVertexArray)
DECL_COND_END_EXT(ARB_vertex_array_object)

DECL_COND_START_EXT(GL_,ARB_depth_clamp)
DECL_COND_END_EXT(ARB_depth_clamp)

DECL_COND_START_EXT(GL_,ARB_multisample)
DECL_COND_END_EXT(ARB_multisample)

DECL_COND_START_EXT (GL_,ARB_vertex_attrib_binding)
DECL_EXTENSION(VERTEXATTRIBFORMAT, VertexAttribFormat)
DECL_EXTENSION(VERTEXATTRIBIFORMAT, VertexAttribIFormat)
DECL_EXTENSION(VERTEXATTRIBLFORMAT, VertexAttribLFormat)
DECL_EXTENSION(VERTEXATTRIBBINDING, VertexAttribBinding)
DECL_EXTENSION(BINDVERTEXBUFFER,BindVertexBuffer)
DECL_COND_END_EXT(ARB_vertex_attrib_binding)

/* GLE_4_2 */
DECL_COND_START_EXT (GL_,ARB_texture_storage)
DECL_EXTENSION(TEXSTORAGE1D, TexStorage1D)
DECL_EXTENSION(TEXSTORAGE2D, TexStorage2D)
DECL_EXTENSION(TEXSTORAGE3D, TexStorage3D)
DECL_COND_END_EXT(ARB_texture_storage)
/* GLE_4_2 */
