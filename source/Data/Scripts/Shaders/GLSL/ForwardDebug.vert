



void main(void)
{
   gl_Position = matMvp * vec4(getPos(), 1.0);	
}

