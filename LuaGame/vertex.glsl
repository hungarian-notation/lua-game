uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

#define LIGHTING

#ifdef USE_POSITION
	in vec3 attr_Position;
#else
	#error this shader requires position
#endif

#ifdef LIGHTING
	#ifdef USE_NORMAL

		uniform mat4 u_InvTrans;

		in vec3 attr_Normal;

		out vec3 frag_Position;
		out vec3 frag_Normal;

	#else
		#error lighting enabled without normals
	#endif
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
	vec4 coordinate = vec4(attr_Position.xyz, 1.0);

	gl_Position = u_Proj * u_View * u_Model * coordinate;
	
	#ifdef USE_COLOR
		ex_Color = attr_Color;
	#endif

	#ifdef USE_TEXTURE
		ex_TexCoord = attr_TexCoord;
	#endif

	#ifdef LIGHTING
		frag_Position = vec3(u_View * u_Model * coordinate);
		frag_Normal =  normalize(vec3(u_InvTrans * vec4(attr_Normal, 0.0)));
	#endif
}