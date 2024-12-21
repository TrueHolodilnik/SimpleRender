// LightingPass.fp'24
#version 140 // compatible with any GLSL shader
precision highp float; // high precision float operations for PC

in vec2 Texcoord2;

out vec4 oColor;

uniform mat4 uPMatrix;

uniform sampler2DRect uColor; 
uniform sampler2DRect uNormal;
uniform sampler2DRect uPosition;
uniform float uLightDistance;

vec4 inAmbient = vec4(0.05f);
vec3 specularColor = vec3(1.0f, 1.0f, 1.0f);

// Project to screen space
vec4 ProjectToScreen(vec4 proj) {
	vec4 screen = proj;
	screen.x = (proj.x + proj.w);
	screen.y = (proj.w - proj.y);
	screen.xy *= 0.5;
	screen.z = proj.z;
	screen.w = proj.w;
	return screen;
}

// Pseudo random number generator. 
float hash( vec2 a ) {return fract( sin( a.x * 3433.8 + a.y * 3843.98 ) * 45933.8 );}

// Pseudo random noise function
float noise( vec2 U ) {
    vec2 id = floor( U );
          U = fract( U );
    U *= U * ( 3. - 2. * U );  

    vec2 A = vec2( hash(id)            , hash(id + vec2(0,1)) ),  
         B = vec2( hash(id + vec2(1,0)), hash(id + vec2(1,1)) ),  
         C = mix( A, B, U.x);

    return mix( C.x, C.y, U.y );
}

// Activision colored AO
vec3 ComputeColoredAO(float ao, vec3 albedo) {
    vec3 a = 2.0404 * albedo - 0.3324;
    vec3 b = -4.7951 * albedo + 0.6417;
    vec3 c = 2.7552 * albedo + 0.6903;

    return max(vec3(ao), ((ao * a + b) * ao + c) * ao);
}

// SSDO - screen space directional occlusion
float CalcSSDO(vec3 P, vec3 N) {
	float SSAO_DEPTH_THRESHOLD = 2.5;
	float SSDO_RADIUS = 2.01 ;
	float SSDO_BLEND_FACTOR = 1;
	float SSDO_SAMPLE = 8;
	
	vec3 hemisphere[10];
	hemisphere[0] = vec3(-0.134, 0.044, -0.825);
	hemisphere[1] = vec3(0.525, -0.397, 0.713);
	hemisphere[2] = vec3(0.307, 0.822, 0.169);
	hemisphere[3] = vec3(-0.006, -0.103, -0.035);
	hemisphere[4] = vec3(0.526, -0.183, 0.424);
	hemisphere[5] = vec3(-0.214, 0.288, 0.188);
	hemisphere[6] = vec3(0.053, -0.863, 0.054);
	hemisphere[7] = vec3(-0.488, 0.473, -0.381);
	hemisphere[8] = vec3(-0.638, 0.319, 0.686);
	hemisphere[9] = vec3(0.164, -0.710, 0.086);

	float noise = noise(Texcoord2 + vec2(0.5,0.5));
	float occ = 0.0;	

	for (int i = 0; i < SSDO_SAMPLE; i++) {
		vec3 reflection_sample = reflect(hemisphere[i], hemisphere[i]) + N.xyz;
		
		vec3 occ_pos_view = P.xyz + reflection_sample * SSDO_RADIUS;
		vec4 occ_pos_screen = ProjectToScreen(uPMatrix * vec4(occ_pos_view, 1.0));
		occ_pos_screen.xy /= occ_pos_screen.w;

		float screen_occ = texture(uPosition, occ_pos_screen.xy + i).z;	

		float is_occluder = step(occ_pos_view.z, screen_occ);
	
		float occ_coeff = clamp(is_occluder + step(SSAO_DEPTH_THRESHOLD, abs(P.z-screen_occ)), 0.0f, 1.0f);
		occ += occ_coeff;
	}
	occ /= SSDO_SAMPLE;
	occ = clamp(occ, 0.0f, 1.0f);
	return pow(occ, SSDO_BLEND_FACTOR);
}

void main()
{
	oColor = vec4(0f, 0f, 0f, 0f);

	// Read gbuffer
	vec4 color = texture(uColor, Texcoord2);
	vec4 normal = texture(uNormal, Texcoord2); //Alpha channel is metalness
	vec4 position = texture(uPosition, Texcoord2);

	// Compute SS colored AO
	vec3 coloredAO = ComputeColoredAO(1 - CalcSSDO(position.rgb, normal.rgb), color.rgb);

	// Light position update
	vec3 lightDir = normalize(vec3(sin(uLightDistance)*5 -5.0f, 5.0f, cos(uLightDistance)*5) - position.rgb + 5.0f);
	vec3 V = -normalize(position.rgb);
  	vec3 H = normalize(lightDir+V);
	
	// Get PBR values from alpha channels
	float roughness = color.a;
	float metalness = position.a;
	float occlusion = normal.a; // Baked occlusion from texture

	// Max for prevent back face lighting
	// Light terms for 
	vec3 reflection = reflect(normalize(-lightDir),normalize(normal.rgb));
	float nDotL = max(0, dot(normal.rgb, -lightDir));
	float nDotH = max(0, dot(normal.rgb, H));
	float nDotV = max(0, dot(normal.rgb, V));
	float lDotH = max(0, dot(-lightDir, H));

	// Calculate diffuse, specular and ambient
	float diffuseTerm = dot(normalize(normal.rgb), lightDir);

	//float specularTerm = pow(max( dot(normalize(normal.rgb),reflection), 0), specExp)*(0.1 + 8*position.a);

	// BRDF specular
	float specExp = 8.0f;
	float F0 = pow((1 - metalness)/sqrt(roughness), specExp);
	float specularTerm = 
		pow(
			max( dot( reflection, V ), 0.f ),
			clamp( F0 * (1.f - nDotL) / 2.f, 0.001f, 255.0f) + specExp + specExp*metalness 
		) * (F0 + 2.f) * 1.f/(2.f*3.14159267);

	// Final light combine
	oColor.rgb = diffuseTerm*color.rgb*occlusion*coloredAO + specularTerm*specularColor + inAmbient.a;

	// Fog - simple depth based exponential fog
	const vec4 fogColor = vec4(0.345098f,0.545098f,0.6627450f,1);
	const float fogPowBase = 2.71828;
	const float fogDensity = 0.9;
	float fogExp = fogDensity * length(position.z) / 20;
	float fogFactor = clamp(pow(fogPowBase,-fogExp*fogExp), 0, 1);

	oColor.rgb = mix(fogColor.rgb, oColor.rgb, fogFactor);

}
