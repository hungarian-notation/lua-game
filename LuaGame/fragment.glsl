#define LIGHTING

#ifdef USE_COLOR
	in vec3 ex_Color;
#endif

#ifdef USE_TEXTURE
	in vec2 ex_TexCoord;
	uniform sampler2D u_Texture;
#endif

#ifdef LIGHTING
	in vec3 frag_Position;
	in vec3 frag_Normal;
	uniform mat4 u_View;
#endif

out vec4 out_Color;

void main(void) {
	vec4 color = vec4(1, 1, 1, 1);
	
	#ifdef USE_COLOR
		color = color * vec4(ex_Color, 1);
	#endif

	#ifdef USE_TEXTURE
		color = color * texture(u_Texture, ex_TexCoord);
	#endif

	#ifdef LIGHTING

		vec3 ambient_light = vec3(0.2, 0.2, 0.2);

		vec4 light_position = vec4(0.0, 0.0, 0.0, 30.0);
		vec3 light_color = vec3(0.6, 0.6, 0.6);

		vec3 to_light;
		float falloff_factor;

		if (light_position.w == 0.0) {
			// DIRECTIONAL LIGHT

			to_light = normalize(vec3(u_View * light_position));
			falloff_factor = 1;
		} else {
			// POINT LIGHT

			to_light = vec3(u_View * vec4(light_position.xyz, 1.0)) - frag_Position;

			float radius = light_position.w;

			falloff_factor = 1 - (min(length(to_light), radius) / radius);

			to_light = normalize(to_light);
		}

		vec3 to_eye = normalize(-frag_Position);

		vec3 reflection = normalize(-reflect(to_light, to_eye));

		vec3 diffuse_light = light_color * max(dot(frag_Normal, to_light), 0.0) * falloff_factor;

		// specular? 

		vec3 effective_light = ambient_light + diffuse_light;

		color = color * vec4(effective_light, 1.0);
	#endif

	#ifdef USE_TRANSPARENCY
		if (color.a == 0)
			discard; 
	#endif

	out_Color = color;
}