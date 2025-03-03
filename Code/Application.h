#pragma once

#include <Windows.h>
#include <GL\glcorearb.h>
#include "Render\\Render.h"
#include "Utils\\Utils.h"

// Application predifinitions
#define ApplicationName "Some simple render"
#define DefaultWindowHeight 1920
#define DefaultWindowWidth 1080
// Application window predifinitions
#define WindowStyle CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
#define LoadWindowCursor LoadCursor(nullptr, IDC_HAND)

class Application {

private:

	// Main render
	std::unique_ptr<RenderClass>	Render;

	HWND            GWindowHandle;                          // Handle to window
	HDC             GDeviceContext;                         // Operating system's device context used for drawing in a window
	HGLRC           GRenderingContext;                      // Rendering context for OpenGL
	HINSTANCE		ApplicationInstance;

	float           GWidth = DefaultWindowHeight;                                 // Window's width
	float           GHeight = DefaultWindowWidth;                                // Window's height

	bool			isActive = true;

	const char*     AppName = ApplicationName;

	// OpenGL extensions
	bool            ARB_extensions_string_present;          // Extension indicating that all extensions can be check
	bool            ARB_pixel_format_present;               // Extension that allows setting pixel format with more flexible functions
	bool            ARB_create_context_present;             // Extension that allows creating rendering context with more flexible functions
	bool            ARB_create_context_profile_present;     // Extension that allows creating core rendering context


public:
	Application(HINSTANCE i_Instance, WNDPROC WndProc);

	~Application();

	void Run();
	void WindowResize(const int i_Width, const int i_Height);
	void ForceRenderUpdate();

	static bool CreateApplicationWindow(const HINSTANCE i_ApplicationInstance, const char* i_ApplicationClassName, HWND& o_WindowHandle);
	static void Application::DestroyApplicationWindow(HWND& io_WindowHandle);

	bool CreateOpenGLContext(const bool i_FirstCreation, const HWND i_WindowHandle, HDC& o_DeviceContext, HGLRC& o_RenderingContext);
	bool CreateBaseContext(const HDC i_DeviceContext, HGLRC& o_RenderingContext);
	bool Application::CreateExtendedContext(const HDC i_DeviceContext, HGLRC& o_RenderingContext);
	void Application::DestroyOpenGLContext(const HWND i_WindowHandle, HDC& io_DeviceContext, HGLRC& io_RenderingContext);

	bool Application::LoadOpenGLFunctions(const HDC i_DeviceContext);

	static bool Application::SetUpBasePixelFormat(const HDC i_DeviceContext);
	static bool SetUpExtendedPixelFormat(const HDC i_DeviceContext);


	RenderClass* GetRender() {
		return Render.get();
	}

	float* GetGWidth() {
		return &GWidth;
	}

	float* GetGHeight() {
		return &GHeight;
	}

	void SetGWidth(float* width) {
		GWidth = *width;
	}

	void SetGHeight(float* height) {
		GHeight = *height;
	}

	const bool GetIsActive() {
		return isActive;
	}

};