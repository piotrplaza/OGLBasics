#version 440

in vec3 vPos;

out vec3 fragColor;

void main()
{
	fragColor = vec3(((vPos.xy + 1.0) / 2.0), 0.0 );
}
