
in vec2 texCoord;

layout(location = 0) out vec4 diffuseColor;

void main()
{
	diffuseColor = vec4(texCoord.x, texCoord.y, 1, 1);
}

