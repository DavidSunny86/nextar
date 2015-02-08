
smooth out vec2 texCoord;

void main(void)
{
	gl_Position = vec4(getPos(), 0, 1);
	texCoord = getPos() * vec2(0.5, 0.5) + vec2(0.5, 0.5);
}

