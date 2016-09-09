#ifdef USE_COLOR
	in vec3 ex_Color;
#endif

#ifdef USE_TEXTURE
	in vec2 ex_TexCoord;
	uniform sampler2D u_Texture;
#endif

out vec4 out_Color;

void main(void) {
	vec4 diffuse = vec4(1, 1, 1, 1);
	
	#ifdef USE_COLOR
		diffuse = diffuse * vec4(ex_Color, 1);
	#endif

	#ifdef USE_TEXTURE
		diffuse = diffuse * texture(u_Texture, ex_TexCoord);
	#endif

	#ifdef USE_TRANSPARENCY
		if (diffuse.a == 0)
			discard; 
	#endif

	out_Color = diffuse;
}