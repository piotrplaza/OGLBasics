#version 440

in vec3 vPos;

out vec3 fragColor;

uniform int iterations = 1;
uniform float zoom = 0.5;
uniform vec2 translation = vec2(0.0, 0.0);

vec2 complexAdd(vec2 c1, vec2 c2)
{
	return vec2(c1.x + c2.x, c1.y + c2.y);
}

vec2 complexMul(vec2 c1, vec2 c2)
{
	return vec2(c1.x * c2.x - c1.y * c2.y, c1.x * c2.y + c1.y * c2.x);
}

vec2 mandelbrot(vec2 p, int iterations)
{
	vec2 result = vec2(0.0, 0.0);
	for (int i = 0; i < iterations; ++i)
	{
		result = complexAdd(complexMul(result, result), p);
	}
	return result;
}

void main()
{
	fragColor =
	length(mandelbrot(vPos.xy / zoom + translation, iterations)) < 2.0
	? vec3(1.0, 1.0, 1.0)
	: vec3(0.0, 0.0, 0.0);
}
