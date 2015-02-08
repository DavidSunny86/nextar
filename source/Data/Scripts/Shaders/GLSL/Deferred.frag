
smooth in vec4 varyingNormal;
layout(location = 0) out vec4 oAlbedoAndGlossMap;
layout(location = 1) out vec4 oNormalMap;

#ifdef HAS_DIFFUSE_MAP
	uniform sampler2D diffuseMap;
	smooth in vec2 varyingUV;
#endif

#ifdef HAS_SPECULAR_MAP
	uniform sampler2D specularMap;
#endif

#ifdef HAS_NORMAL_MAP
	uniform sampler2D normalMap;
#endif

vec3 getDiffuseColor() {
#ifdef HAS_DIFFUSE_MAP
	return (texture(diffuseMap, varyingUV).rgb * diffuseAndGloss.rgb);
#else
	return diffuseAndGloss.rgb;
#endif
}

float getGloss() {
	return diffuseAndGloss.a;
}

vec2 getEncodedNormal(vec3 n) {
	vec2 enc = normalize(n.xy) * (sqrt(n.z*0.5+0.5));
	enc = enc*0.5+0.5;
	return enc;
}

vec2 getNormal() {
	return getEncodedNormal(normalize(varyingNormal.xyz));
}

void main(void) {
	oAlbedoAndGlossMap = vec4(getDiffuseColor(), getGloss());
	oNormalMap = vec4(getNormal(), 0, 0);
}

