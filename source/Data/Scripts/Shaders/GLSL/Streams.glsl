

#if VertexStage

#ifdef eIn2DPos
	in vec2 _v2DPos;
	vec2 vGet2DPos() { return _v2DPos; }
#endif

#ifdef eInPos
	in vec3 _vPos;
	vec3 vGetPos() { return _vPos; }
#endif

#ifdef eInNormal
	in vec3 _vNormal;
	vec3 vGetNormal() { return _vNormal; }
#endif

#ifdef eInBinormal
	in vec3 _vBinormal;
	vec3 vGetBinormal() { return _vBinormal; }
#endif

#ifdef eInTangent
	in vec3 _vTangent;
	vec3 vGetTangent() { return _vTangent; }
#endif

#ifdef eInColor
	in vec4 _vColor;
	vec4 vGetColor() { return _vColor; }
#endif

#ifdef eInTexCoord0
	in vec2 _vTexCoord0;
	vec2 vGetTexCoord0() { return _vTexCoord0; }
#endif

#ifdef eInTexCoord1
	in vec2 _vTexCoord1;
	vec2 vGetTexCoord1() { return _vTexCoord1; }
#endif

#endif


#ifdef DestBlockName
out _DestBlock {

#ifdef eOutNormal
	vec3 s_vNormal;
#endif

}DestBlockName;

#ifdef eOutNormal
void vSetNormal(in vec3 vNormal) {
	s_vNormal = vNormal;
}
#endif

#endif

#ifdef SrcBlockName
in _SrcBlock {

#ifdef eInNormal
	vec3 s_vNormal;
#endif

}SrcBlockName;


#ifdef eInNormal
	vec3 vGetNormal() { return s_vNormal; }
#endif

#endif



#if VertexStage
#define DestBlockName vsout
#elif VertexToHull
#define SrcBlockName vsout[]
#define DestBlockName hsout
#elif VertexToDomain
#define SrcBlockName vsout[]
#define DestBlockName dsout
#elif HullToDomain
#define SrcBlockName hsout[]
#define DestBlockName dsout[]
#elif DomainToGeometry
#define SrcBlockName dsout[]
#define DestBlockName gsout
#elif VertexToGeometry
#define SrcBlockName vsout[]
#define DestBlockName gsout
#elif GeometryToFragment
#define SrcBlockName gsout
#elif VertexToFragment
#define SrcBlockName vsout
#endif
