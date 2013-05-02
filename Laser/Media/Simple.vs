
#version 150

in vec4 inPosition;
in vec3 inColor;
in vec2 inTexCoord;

out vec3 VertexColor;
out vec2 TexCoord;

uniform Transform
{
	float gMVPMatrix;
};

void main( void )
{

	gl_Position = inPosition;
	VertexColor = inColor;
	TexCoord = inTexCoord;

}
