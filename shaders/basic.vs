#version 440

in vec3 bPos;
in vec3 bCol;

out vec3 vCol;

uniform mat4 mvp = mat4(1.0);

void main()
{
	vCol = bCol;
	gl_Position = mvp * vec4(bPos, 1);
}
