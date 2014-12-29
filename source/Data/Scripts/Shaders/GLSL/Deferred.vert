#version 330


smooth out vec4 varyingNormal;
#ifdef HAS_DIFFUSE_MAP
	smooth out vec2 varyingUV;
#endif

void main(void)
{
	varyingNormal = matMv * vec4(getNorm(), 0.0);	
#ifdef HAS_DIFFUSE_MAP
	varyingUV = getDiffuseUV();
#endif
	gl_Position = matMvp * vec4(getPos(), 1.0);
}

