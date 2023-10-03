#version 330

uniform vec4 color;
uniform sampler2D tex;
uniform int invertX;

in vec2 texCoordFrag;
out vec4 outColor;

void main()
{
	// Discard fragment if texture sample has alpha < 0.5
	// otherwise compose the texture sample with the fragment's interpolated color
	vec4 texColor;

	if (invertX == 1) texColor = texture(tex, vec2(1 - texCoordFrag.x, texCoordFrag.y));
	else texColor = texture(tex, texCoordFrag);

	if(texColor.a < 0.5f)
		discard;
	outColor = color * texColor;
}

