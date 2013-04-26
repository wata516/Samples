
#version 150

uniform mat4 MVPMatrix;
in vec4 inPosition;
in vec3 inColor;

out vec3 outColor;
void main( void )
{

	gl_Position = MVPMatrix * inPosition;
	outColor = inColor;

}
