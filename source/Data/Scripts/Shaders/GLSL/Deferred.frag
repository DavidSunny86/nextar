

layout(location = 0) out vec4 oAlbedoMap;
layout(location = 1) out vec4 oNormalMap;
layout(location = 2) out vec4 oSpecularAndGlossMap;

#ifdef HAS_TCOORD0
	smooth in vec2 varyingUV;
#endif
 
#ifdef HAS_DIFFUSE_MAP
	uniform sampler2D diffuseMap;
#endif

#ifdef HAS_NORMAL_MAP
	uniform sampler2D normalMap;
	vec4 getNormal() {
		return (matMv * vec4(texture(normalMap, varyingUV).rgb, 0));
	}
#else
	smooth in vec4 varyingNormal;
	vec4 getNormal() {
		return varyingNormal;
	}
#endif

vec4 getDiffuseColor() {
#ifdef HAS_DIFFUSE_MAP
	return (texture(diffuseMap, varyingUV) * diffuseColor);
#else
	return diffuseColor;
#endif
}

#ifdef HAS_SPECULAR_AND_GLOSS_MAP
	uniform sampler2D specularAndGlossMap;
	vec2 getSpecularAndGloss() {
		return texture(specularAndGlossMap, varyingUV).rg;
	}
#else
	vec2 getSpecularAndGloss() {
		return vec2(specularIntensity, gloss);
	}
#endif

vec2 getEncodedNormal(vec3 n) {
	vec2 enc = normalize(n.xy) * (sqrt(n.z*0.5+0.5));
	enc = enc*0.5+0.5;
	return enc;
}

vec2 getEncodedNormal() {
	return getEncodedNormal(normalize(getNormal().xyz));
}

void main(void) {
	oAlbedoMap = vec4(getDiffuseColor().rgb, 0) + vec4(1, 0, 0, 1);
	oNormalMap = vec4(getEncodedNormal(), 0, 0);
	oSpecularAndGlossMap = vec4(getSpecularAndGloss(), vec2(0, 0));
}

