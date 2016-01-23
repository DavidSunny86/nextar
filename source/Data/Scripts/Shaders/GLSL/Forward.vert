


smooth out vec4 varyingNormal;
smooth out vec4 posInEyeSpace;

#ifdef HAS_TCOORD0
	smooth out vec2 varyingUV;
#endif



void main(void)
{


   posInEyeSpace = matMv * vec4(getPos(), 1.0);
	varyingNormal = matMv * vec4(getNorm(), 0.0);
#ifdef HAS_TCOORD0
	varyingUV =  getTexCoord0();
#endif
   gl_Position = matMvp * vec4(getPos(), 1.0);
	
}

