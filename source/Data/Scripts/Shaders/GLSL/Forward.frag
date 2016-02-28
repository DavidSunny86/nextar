
layout(location = 0) out vec4 oFragColor;

smooth in vec4 varyingNormal;
smooth in vec4 posInEyeSpace;

#ifdef HAS_TCOORD0
	smooth in vec2 varyingUV;
#endif
 
#ifdef HAS_DIFFUSE_MAP
	uniform sampler2D diffuseMap;
#endif

vec4 getDiffuseColor() {
#ifdef HAS_DIFFUSE_MAP
	return (texture(diffuseMap, varyingUV) * diffuseColor);
#else
	return diffuseColor;
#endif
}


void main(void) {
	vec3 N = normalize(varyingNormal.xyz);
	vec3 L = normalize(omniLightPosition.xyz - posInEyeSpace.xyz);
	vec3 E = normalize(-posInEyeSpace.xyz); // we are in Eye Coordinates, so EyePos is (0,0,0)  
	vec3 R = normalize(-reflect(L,N));  
 
	//calculate Ambient Term:  
	vec4 Iamb = ambientColor;    

	//calculate Diffuse Term:  
	vec4 Idiff = omniLightIntensity * max(dot(N,L), 0.0);
	Idiff = clamp(Idiff, 0.0, 1.0) * vec4(getDiffuseColor().rgb, 0);     
	
	// calculate Specular Term:
	vec4 Ispec = omniLightIntensity
					 * pow(max(dot(R,E),0.0),0.3*specularIntensity);
	Ispec = clamp(Ispec, 0.0, 1.0); 
	
	oFragColor = vec4(N, 1) * (Idiff + Ispec + Iamb);
}

