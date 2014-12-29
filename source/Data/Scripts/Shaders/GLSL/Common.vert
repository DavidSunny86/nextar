

#ifdef HAS_2DVERT
	in vec2 vert2DPos;
	vec2 getPos() { return vert2DPos; }
#else
	in vec3 vertPos;
	vec3 getPos() { return vertPos; }
#endif

#ifdef HAS_NORMAL
	in vec3 vertNorm;
	vec3 getNorm() { return vertNorm; }
#endif

#ifdef HAS_BINORMAL
	in vec3 vertBiNorm;
	vec3 getBiNorm() { return vertBiNorm; }
#endif

#ifdef HAS_TANGENT
	in vec3 vertTangent;
	vec3 getTangent() { return vertTangent; }
#endif

#ifdef HAS_COLOR
	in vec4 vertColor;
	vec4 getColor() { return vertColor; }
#endif

#ifdef HAS_SEC_COLOR
	in vec4 vertSecColor;
	vec4 getSecondaryColor() { return vertSecColor; }
#endif


#ifdef HAS_TCOORD0
	in vec2 texCoord0;
	vec2 getTexCoord0() { return texCoord0; }
#endif

#ifdef HAS_TCOORD1
	in vec2 texCoord1;
	vec2 getTexCoord1() { return texCoord1; }
#endif

