// BasePass
#version 140 // compatible with any GLSL shader
precision highp float; // high precision float operations for PC

// Texture coordinates from vertex shader
in vec2 TexCoord;
// Normal from vertex shader
in vec3 Normal;
// Position from vertex shader
in vec3 Position;

// Output GBuffer data
// Output color for next render passes
// Alpha for roughness
out vec4 oColor;
// Normal including texture normals, alpha for occlusion
out vec4 oNormal;
// Position, alpha for metalness
out vec4 oPosition;

// Mesh textures used for base pass
uniform sampler2D uTexture; // Diffuse color
uniform sampler2D uNormalTexture; // Normal maps (must be OpenGL format)
uniform sampler2D uPBRTexture; // PBR texture: Occlusion, roughness , metalness

void main()
{
	vec4 PBR = texture(uPBRTexture, TexCoord);
	oColor = vec4(texture(uTexture, TexCoord).rgb, PBR.g);

	vec3 normalMap = texture(uNormalTexture, TexCoord).rgb;
    vec3 normal = normalize(normalMap * 2.0 - 1.0);
	
	oNormal = vec4(normalize(Normal) - normal/1.1, PBR.r);
	oPosition = vec4(Position, PBR.b);
}
