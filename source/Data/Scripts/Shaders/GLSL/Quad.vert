
#ifdef HAS_DIFFUSE_MAP
	smooth out vec2 texCoord;
#endif

void main(void)
{
	gl_Position = vec4((getPos() * translationAndScale.zw) + translationAndScale.xy, -0.9f, 1);
#ifdef HAS_DIFFUSE_MAP
	texCoord = ((getPos() * vec2(0.5, 0.5) + vec2(0.5, 0.5)) * textureOffsetAndRepeat.zw) + textureOffsetAndRepeat.xy;
#endif

}
