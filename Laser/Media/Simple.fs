
#version 150

in vec3 inColor;
out vec4 outColor;

void main( void )
{
	outColor = vec4( inColor, 1.0f );
}
