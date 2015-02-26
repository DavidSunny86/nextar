

out vec4 varyingColor;

void main(void)
{
	gl_Position = modelViewProj * vec4(getPos(), 1);
	varyingColor = getColor() * diffuseColor;
}
