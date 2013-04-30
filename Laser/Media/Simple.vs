
#version 150

in vec4 inPosition;
in vec3 inColor;

out vec3 VertexColor;

layout( row_major, shared ) uniform Transform
{
	mat4x4 gMVPMatrix;
};

void main( void )
{

	gl_Position = inPosition;
	VertexColor = inColor;

}
