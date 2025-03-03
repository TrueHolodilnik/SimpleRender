#ifndef RENDER_H
#define RENDER_H

#include <vector>
#include <Windows.h>
#include <GL\glcorearb.h>
#include "OpenGLFunctions.h"
#include "RenderStructs.h"
#include "..\\MatrixAlgebra.h"
#include "..\\Utils\\Utils.h"
#include "..\\tinyGLTF\\tiny_gltf.h"
#include "..\\Configs\\KeysConfiguration.h"

static unsigned int GQuadVAO = 0;
static unsigned int GPlaneVAO = 0;

//Render predifinitions
#define DefaultFOV 45.0f
#define DefaultNearClipPlane 1.0f
#define DefaultFarClipPlane 20.0f

class RenderClass {

private:
	float* Width;
	float* Height;
	
	float           Angle = 0;                                  // Rotation angle for scene objects
	float           LightDistance = 0;                          // Light position distance from camera
	float           ProjectionMatrix[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };                   // Projection matrix
	float           ModelViewMatrix[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };;                    // Model view matrix
	float           ModelViewProjectionMatrix[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };;          // Model view projection matrix


	HDC*			DeviceContext;

public:

    std::unique_ptr<GLHandlers> Handlers = std::make_unique<GLHandlers>();
	std::unique_ptr<GLTextures> Textures = std::make_unique<GLTextures>();
	std::unique_ptr<RenderPasses> RenderPassesV = std::make_unique<RenderPasses>();

	tinygltf::Model model;
	std::pair<GLuint, std::map<int, GLuint>> vaoAndEbos;

	RenderClass(HDC* inDeviceContext, float* iWidth, float* iHeight);
	~RenderClass();

	void Render();

	void Resize(const int i_Width, const int i_Height);

	HDC GetDeviceContext() { return *DeviceContext; }

	void UpdateParameters(WPARAM i_wParam, LPARAM i_lParam);

	void drawModel(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model);

	// Load and draw function based on tinyGLTF library
	// TODO: separate to different class
	void bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh);
	void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Node& node);
	std::pair<GLuint, std::map<int, GLuint>> bindModel(tinygltf::Model& model);
	bool loadModel(tinygltf::Model& model, const char* filename);
	void drawMesh(const std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh);
	void drawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, tinygltf::Node& node);

	void ResetOGLStateDefault();
	void BindShaderUniformAdresses();
	void CreateGBuffer();
	void PrepareScene();
	void CreateGBRenderTargets();
	void BindTextures();

	static GLuint CreateRectTexture(const size_t, const size_t, const GLenum, const GLenum, const GLenum);
	static GLuint CreateRenderTarget(const std::vector<std::pair<GLenum, GLuint>>);

	static void GenerateMesh(const size_t, const int*, const float*, const float*, const float*, unsigned int&, unsigned int&);
	void CreateFullscreenQuad(const float i_Width, const float i_Height);
	void DestroyGeometry();

	static GLuint RenderClass::CreateShader(const std::string i_Filename, const GLenum i_Type);
	void RenderClass::DestroyShaders();
	bool RenderClass::CreateShaders();

};




#endif // !RENDER_H