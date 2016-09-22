#ifdef USE_COLOR
	in vec3 ex_Color;
#endif

#ifdef USE_TEXTURE
	in vec2 ex_TexCoord;
	uniform sampler2D u_Texture;
#endif

#ifdef USE_LIGHTING

	uniform vec3 u_AmbientLight;
	uniform vec4 u_LightPos[MAX_LIGHTS];
	uniform vec3 u_LightColor[MAX_LIGHTS];

	in vec3 frag_Position;
	in vec3 frag_Normal;

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

	#ifdef USE_LIGHTING
	
		vec3 effective_light = u_AmbientLight;

		for (int li = 0; li < MAX_LIGHTS; li++) {

			vec4 light_position = u_LightPos[li];
			vec3 light_color = u_LightColor[li];

			vec3 to_light;
			float falloff_factor;

			if (light_position.w == 0.0) {
				// DIRECTIONAL LIGHT

				to_light = normalize(vec3(light_position));
				falloff_factor = 1;
			} else {
				// POINT LIGHT

				to_light = light_position.xyz - frag_Position;

				float radius = light_position.w;

				falloff_factor = 1 - (min(length(to_light), radius) / radius);

				to_light = normalize(to_light);
			}

			vec3 to_eye = normalize(-frag_Position);

			vec3 reflection = normalize(-reflect(to_light, to_eye));

			vec3 diffuse_light = light_color * max(dot(frag_Normal, to_light), 0.0) * falloff_factor;

			// specular? 

			effective_light = effective_light + diffuse_light;
		}

		color = color * vec4(effective_light, 1.0);

	#endif

	#ifdef USE_TRANSPARENCY
		if (color.a == 0)
			discard; 
	#endif

	out_Color = color;
}