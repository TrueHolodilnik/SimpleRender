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

static unsigned int GQuadVAO = 0;
static unsigned int GPlaneVAO = 0;

void drawMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh);
void drawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, tinygltf::Node& node);

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

	GLHandlers* Handlers = new GLHandlers();
	GLTextures* Textures = new GLTextures();
	RenderPasses* RenderPassesV = new RenderPasses();

	tinygltf::Model model;
	std::pair<GLuint, std::map<int, GLuint>> vaoAndEbos;

	RenderClass(HDC* inDeviceContext, float* iWidth, float* iHeight);

	void Render();

	void Resize(const int i_Width, const int i_Height);

	HDC GetDeviceContext() { return *DeviceContext; }

	void UpdateParameters(WPARAM i_wParam, LPARAM i_lParam);

	void drawModel(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model);

	// TinyGLTF mesh loading
	void bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh);
	void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Node& node);
	std::pair<GLuint, std::map<int, GLuint>> bindModel(tinygltf::Model& model);
	bool loadModel(tinygltf::Model& model, const char* filename);

	void PrepareScene();

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