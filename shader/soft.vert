
varying vec3 texCoord;
varying vec3 vertex;
varying vec4 vertex_screen;

void main(void)
{
	texCoord			= gl_MultiTexCoord0.xyz;
    gl_Position		= gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz,1.0);
	vertex					= gl_Vertex.xyz;
	//gl_Position.z =1;//gl_Position.z/100;
	//gl_Position.w=1/gl_Position.z;	
	vertex_screen	= gl_Position;

}