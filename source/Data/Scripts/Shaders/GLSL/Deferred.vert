

#ifndef HAS_NORMAL_MAP
smooth out vec4 varyingNormal;
#endif

#ifdef HAS_TCOORD0
	smooth out vec2 varyingUV;
#endif

void main(void)
{
#ifndef HAS_NORMAL_MAP
	varyingNormal = matMv * vec4(getNorm(), 0.0);
#endif
	
#ifdef HAS_TCOORD0
	varyingUV =  getTexCoord0();
#endif
	gl_Position = matMvp * vec4(getPos(), 1.0);
}

