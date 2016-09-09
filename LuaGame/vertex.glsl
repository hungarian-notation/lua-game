uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

#ifdef USE_POSITION
	in vec3 attr_Position;
#else
	#error this shader requires position
#endif

#ifdef USE_NORMAL
	#error this shader does not support normals
#endif

#ifdef USE_COLOR
	in vec3 attr_Color;
	out vec3 ex_Color;
#endif

#ifdef USE_TEXTURE
	in vec2 attr_TexCoord;
	out vec2 ex_TexCoord;
#endif

void main(void) {
	gl_Position = u_Proj * u_View * u_Model * vec4(attr_Position.xyz, 1.0);
	
	#ifdef USE_COLOR
		ex_Color = attr_Color;
	#endif

	#ifdef USE_TEXTURE
		ex_TexCoord = attr_TexCoord;
	#endif
}