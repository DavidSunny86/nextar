

layout(location = 0) out vec4 oFragColor;

#ifdef HAS_DIFFUSE_MAP
	uniform sampler2D diffuseMap;
   in vec2 texCoord;
	vec4 getFragmentColor() {
		return ( texture( diffuseMap, texCoord ) * mixColor );
	}
#else
	vec4 getFragmentColor() {
		return mixColor;
	}
#endif

void main()
{
	oFragColor = getFragmentColor();
}
