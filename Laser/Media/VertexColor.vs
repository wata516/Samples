
#version 150

in vec4 inPosition;
in vec3 inColor;

out vec3 VertexColor;

void main( void )
{

	gl_Position = inPosition;
	VertexColor = inColor;

}
