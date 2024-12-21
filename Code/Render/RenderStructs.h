#pragma once
#include <GL/glcorearb.h>

// Uniform handler adresses
struct GLHandlers {
	GLint           MVPMatrixHandle = -1;                        // Base pass model view projection matrix parameter handle
	GLint           MVMatrixHandle = -1;                         // Base pass model view matrix parameter handle
	GLint           DiffuseTextureHandle = -1;                   // Base pass diffuse texture parameter handle
	GLint           DiffuseNormalTextureHandle = -1;             // Base pass diffuse normal texture parameter handle
	GLint           DiffusePBRTextureHandle = -1;				 // Base pass diffuse PBR parameter handle
	GLint           ColorTextureHandle = -1;                     // Lighting pass color texture parameter handle
	GLint           NormalTextureHandle = -1;                    // Lighting pass normal texture parameter handle
	GLint           PositionTextureHandle = -1;                  // Lighting pass position texture parameter handle
	GLint           LightDistanceHandle = -1;                    // Lighting pass light distance parameter handle
	GLint           PMatrixHandle = -1;							 // Lighting pass projection matrix handle
};

// Uniform texture adresses
struct GLTextures {
	GLuint          DiffuseTexture = -1;                         // Diffuse texture ID for geometry
	GLuint          DiffuseNormalTexture = -1;                   // Diffuse normal texture ID for geometry
	GLuint          DiffusePBRTexture = -1;						 // Diffuse PBR texture ID for geometry
	GLuint          ColorTexture = -1;                           // Texture containing colors of scene objects
	GLuint          NormalTexture = -1;                          // Texture containing normal vectors of scene objects
	GLuint          PositionTexture = -1;                        // Texture containing positions of scene objects
	GLuint          DepthTexture = -1;                           // Texture used as a depth buffer
	GLuint          BasePassRT = -1;                             // Render target texture for base pass
};

// Render passes
struct RenderPasses {
	unsigned int    BasePassProgram = 0;                        // Shader program used for drawing base pass
	unsigned int    LightingPassProgram = 0;                    // Shader program used for drawing lighting pass
};