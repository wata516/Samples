
#version 150

in vec3 VertexColor;
out vec4 outColor;

void main( void )
{
	outColor = vec4( VertexColor, 1.0f );
}
