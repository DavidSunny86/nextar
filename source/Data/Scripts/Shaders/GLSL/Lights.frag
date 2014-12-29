#version 330

uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform sampler2D albedoAndGlossMap;

in vec2 texCoord;
out vec4 vFragColor;

vec3 getDecodedNormal(vec4 enc) {
	vec4 nn = enc*vec4(2,2,0,0) + vec4(-1,-1,1,-1);
	float l = dot(nn.xyz,-nn.xyw);
	nn.z = l;
	nn.xy *= sqrt(l);
	return nn.xyz * 2 + vec3(0,0,-1);
}

vec3 getPosition(vec2 texCoord) {
	float z = texture(depthMap, texCoord).r;  
	float x = texCoord.x * 2 - 1;
	float y = (1 - texCoord.y) * 2 - 1;
	vec4 vProjectedPos = vec4(x, y, z, 1.0f);
	vec4 vPositionVS = mul(vProjectedPos, invProjection);  
	// Divide by w to get the view-space position
	return vPositionVS.xyz / vPositionVS.w;  
}

vec3 getNormal(vec2 texCoord) {
	return getDecodedNormal(texture(normalMap, texCoord));
}

void main(void) {
	vec3 n = getNormal(texCoord);
	vec3 p = getPosition(texCoord);
	vec3 lightDir =  (p - lightPosAndRadius.xyz);
	float distanceToLight = dot(lightDir, lightDir);
	lightDir = normalize(lightDir);
	vec4 albedoAndGloss = texture(albedoAndGlossMap, texCoord);
	// Dot product gives us diffuse intensity
	float diff = max(0.0, dot(n, lightDir));
	// Multiply intensity by diffuse color, force alpha to 1.0
	vFragColor = diff * albedoAndGloss * lightColor;
	// Specular Light
	vec3 vReflection = reflect(-lightDir,	n);
	float spec = max(0.0, dot(n, vReflection));
	// If the diffuse light is zero, don’t even bother with the pow function
	if(diff != 0) {
		float fSpec = pow(spec, albedoAndGloss.a);
		vFragColor += vec4(fSpec, fSpec, fSpec, fSpec);
	}
	float atten = clamp(1 - distanceToLight/(lightPosAndRadius.w*lightPosAndRadius.w), 0.0, 1.0);
	vFragColor *= (atten*atten);
}


