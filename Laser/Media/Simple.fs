
#version 150

in vec3 VertexColor;
in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D DecalMap;

void main( void )
{
	vec4 DecalColor = texture( DecalMap, TexCoord );
	outColor = vec4( DecalColor.xyz, 1.0f );
}
