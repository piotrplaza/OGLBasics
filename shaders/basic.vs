#version 440

in vec3 bPos;

out vec3 vPos;

uniform mat4 mvp = mat4(1.0);

void main()
{
	vPos = bPos;
	gl_Position = mvp * vec4(bPos, 1.0);
}
