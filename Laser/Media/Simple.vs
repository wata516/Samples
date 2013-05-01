
#version 150

in vec4 inPosition;
in vec3 inColor;

out vec3 VertexColor;

uniform Transform
{
	float gMVPMatrix;
};

void main( void )
{

	gl_Position = inPosition;
//	VertexColor = inColor;
	VertexColor = vec3(0.0F);
	VertexColor.x = gMVPMatrix;

}
