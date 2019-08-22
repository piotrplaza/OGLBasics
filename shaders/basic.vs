#version 440

in vec3 bPos;
in vec3 bCol;

out vec3 vCol;

void main()
{
	vCol = bCol;
	gl_Position = vec4(bPos, 1);
}
