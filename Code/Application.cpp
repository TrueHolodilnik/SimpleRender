#pragma once

#include "Application.h"
#include "Render\\OpenGLFunctions.h"

#define CHECKEXTENSION( x, y, z ) x = UtilsInstance->CheckExtension( #y, z )
#define GETFUNCTIONADDRESS( x, y ) y = (x)UtilsInstance->GetFunctionAddress( handle, #y )

Application::Application(HINSTANCE i_Instance, WNDPROC WndProc) {

	ApplicationInstance = i_Instance;

	// Create app and setup OpenGL state and prepare objects for drawing
	bool Result = true;

	HWND  window_handle;
	HDC   device_context;
	HGLRC rendering_context;

	// Register class of the application
	// Initialize structure describing application class
	WNDCLASSEX WindowClassEX;
	WindowClassEX.cbSize = sizeof(WNDCLASSEX);
	WindowClassEX.style = WindowStyle;
	WindowClassEX.hInstance = i_Instance;
	WindowClassEX.lpfnWndProc = WndProc;
	WindowClassEX.cbClsExtra = 0;
	WindowClassEX.cbWndExtra = 0;
	WindowClassEX.hIcon = nullptr;
	WindowClassEX.hIconSm = nullptr;
	WindowClassEX.hCursor = LoadWindowCursor;
	WindowClassEX.hbrBackground = nullptr;
	WindowClassEX.lpszMenuName = nullptr;
	WindowClassEX.lpszClassName = AppName;

	// Register application's class in operating system
	if (!RegisterClassEx(&WindowClassEX)) {
		UtilsInstance->ErrorMessage("Application Creation Error", "Failed to register application class.");
		Result = false;
	}

	// Create window
	if (!CreateApplicationWindow(i_Instance, AppName, window_handle)) {
		Result = false;
	}
	// Create OpenGL context using old, Windows functions
	if (!CreateOpenGLContext(true, window_handle, device_context, rendering_context)) {
		Result = false;
	}

	// Destroy OpenGL context created with old functions
	DestroyOpenGLContext(window_handle, device_context, rendering_context);
	// Destroy window - pixel format for window can be set only once so new window need to be created
	DestroyApplicationWindow(window_handle);

	// Create window again
	if (!CreateApplicationWindow(i_Instance, AppName, window_handle)) {
		Result = false;
	}
	// Create OpenGL context this time using more flexible functions (if available)
	if (!CreateOpenGLContext(false, window_handle, device_context, rendering_context)) {
		Result = false;
	}

	ShowWindow(window_handle, SW_SHOW);
	UpdateWindow(window_handle);

	GWindowHandle = window_handle;
	GDeviceContext = device_context;
	GRenderingContext = rendering_context;

	if (!Result) {
		this->~Application();
		UtilsInstance->ErrorMessage("Application Creation Error", "Could not create application.", true);
	}

	Render.reset(new RenderClass(&GDeviceContext, &GWidth, &GHeight));

};

Application::~Application() {

	// Destroy OpenGL rendering context
	DestroyOpenGLContext(GWindowHandle, GDeviceContext, GRenderingContext);

	// Destroy window
	DestroyApplicationWindow(GWindowHandle);

	// Unregisters class of the application
	if (ApplicationInstance != nullptr) {

		// Stop using class registered for the application
		if (!UnregisterClass(AppName, ApplicationInstance)) {
			UtilsInstance->ErrorMessage("Window Shutdown Error", "Could not unregister window class.");
		}
		ApplicationInstance = nullptr;
	}
}

void Application::Run() {
	// Main application loop - processes messages and draws scene
	MSG		message;
	while (true) {
		// Check queue for message and, if there is any, process it
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			if (message.message == WM_QUIT) {
				break;
			}
			else {
				// Process message
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
		else {
			isActive = true;
			Render->Render();
		}
	}
	isActive = false;
}

void Application::ForceRenderUpdate() {
	Render->Render();
}

void Application::WindowResize(const int i_Width, const int i_Height) {
	int w = i_Width;
	int h = i_Height;
	if (h <= 0) {
		h = 1;
	}

	// Store new dimensions
	GWidth = (float)w;
	GHeight = (float)h;

	Render->Resize(i_Width, i_Height);

}

// Create window for the application
bool Application::CreateApplicationWindow(const HINSTANCE i_ApplicationInstance, const char* i_ApplicationClassName, HWND& o_WindowHandle) {
	DEVMODE displayProperties;

	// Check the resolution of desktop
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &displayProperties);

	// Set the window size to be a little smaller than desktop
	int Top = 5;
	int Left = 5;
	int Width = displayProperties.dmPelsWidth - 10;
	int Height = displayProperties.dmPelsHeight - 50;

	// Default style for windows that are not fullscreen
	DWORD WinStyle = WS_OVERLAPPEDWINDOW;
	DWORD ExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	// Create window
	HWND window_handle = CreateWindowEx(ExtendedStyle, i_ApplicationClassName, i_ApplicationClassName, WinStyle,
		Left, Top, Width, Height, nullptr, nullptr, i_ApplicationInstance, nullptr);
	if (!window_handle) {
		UtilsInstance->ErrorMessage("Application Creation Error", "Could not create application window.");
		return 0;
	}

	o_WindowHandle = window_handle;
	return true;
}

// Destroys application window
void Application::DestroyApplicationWindow(HWND& io_WindowHandle) {
	if (io_WindowHandle) {
		// Destroy handle associated with window
		if (!DestroyWindow(io_WindowHandle)) {
			UtilsInstance->ErrorMessage("Window Shutdown Error", "Could not release hWnd.");
		}
		io_WindowHandle = 0;
	}
}

bool Application::CreateOpenGLContext(const bool i_FirstCreation, const HWND i_WindowHandle, HDC& o_DeviceContext, HGLRC& o_RenderingContext) {
	// Get context of a device associated with created window
	HDC device_context = GetDC(i_WindowHandle);
	if (!device_context) {
		UtilsInstance->ErrorMessage("OpenGL Creation Error", "Could not get the device context.");
		return false;
	}

	HGLRC rendering_context;

	// Create OpenGL's rendering context
	if (i_FirstCreation) {

		// Set the description of drawing buffers
		if (!SetUpBasePixelFormat(device_context)) {
			return false;
		}
		// Create rendering context
		if (!CreateBaseContext(device_context, rendering_context)) {
			return false;
		}
		// Check OpenGL's extensions and get addresses of OpenGL's functions beyond version 1.1
		if (!LoadOpenGLFunctions(device_context)) {
			return false;
		}

		o_RenderingContext = rendering_context;
	}
	else {

		// Check if new methods for setting pixel format are available
		if (ARB_pixel_format_present) {
			// Set pixel format with new methods
			if (!SetUpExtendedPixelFormat(device_context)) {
				return false;
			}
		}
		else {
			// Set pixel format with old methods
			if (!SetUpBasePixelFormat(device_context)) {
				return false;
			}
		}
		// Check if new methods for creating rendering context are available
		if (ARB_create_context_present) {
			// Create context with new methods
			if (!CreateExtendedContext(device_context, rendering_context)) {
				return false;
			}
		}
		else {
			// Create context the old way
			if (!CreateBaseContext(device_context, rendering_context)) {
				return false;
			}
		}
		// Check OpenGL's extensions and get addresses of OpenGL's functions beyond version 1.1
		// Newly created rendering context may have different addresses of the same functions!
		if (!LoadOpenGLFunctions(device_context)) {
			return false;
		}
	}
	if (!rendering_context) {
		return false;
	}

	o_DeviceContext = device_context;
	o_RenderingContext = rendering_context;
	return true;
}

bool Application::CreateBaseContext(const HDC i_DeviceContext, HGLRC& o_RenderingContext) {
	// Create rendering context
	HGLRC rendering_context = wglCreateContext(i_DeviceContext);
	if (!rendering_context) {
		UtilsInstance->ErrorMessage("OpenGL Creation Error", "Could not create rendering context.");
		return false;
	}
	// Set the created context as the current context used for OpenGL's drawing commands
	if (!wglMakeCurrent(i_DeviceContext, rendering_context)) {
		wglDeleteContext(rendering_context);
		UtilsInstance->ErrorMessage("OpenGL Creation Error", "Could not activate rendering context.");
		return false;
	}

	o_RenderingContext = rendering_context;
	return true;
}

void Application::DestroyOpenGLContext(const HWND i_WindowHandle, HDC& io_DeviceContext, HGLRC& io_RenderingContext) {
	//void DestroyGeometry();

	// Destroy rendering context
	if (io_RenderingContext) {
		// Stop using rendering context
		if (!wglMakeCurrent(nullptr, nullptr)) {
			UtilsInstance->ErrorMessage("OpenGL Shutdown Error", "Could not deactivate rendering context.");
		}
		// Delete context
		if (!wglDeleteContext(io_RenderingContext)) {
			UtilsInstance->ErrorMessage("OpenGL Shutdown Error", "Could not release rendering context.");
		}
		io_RenderingContext = 0;
	}

	if (io_DeviceContext) {
		// Release device context
		if (!ReleaseDC(i_WindowHandle, io_DeviceContext)) {
			UtilsInstance->ErrorMessage("OpenGL Shutdown Error", "Could not release device context.");
		}
		io_DeviceContext = 0;
	}
}

bool Application::CreateExtendedContext(const HDC i_DeviceContext, HGLRC& o_RenderingContext) {
	const GLint versions[][2] = { {4, 3}, {4, 2}, {4, 1}, {4, 0}, {3, 3}, {3, 2}, {3, 1}, {3, 0} };
	const int   major = 0;
	const int   minor = 1;
	const int   count = sizeof(versions) / sizeof(versions[0]);

	// Fill the array describing desired attributes of rendering context
	GLint context_attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB,	versions[0][major],                         // Major version of context
		WGL_CONTEXT_MINOR_VERSION_ARB,	versions[0][minor],                         // Minor version of context
		WGL_CONTEXT_FLAGS_ARB,			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,     // Forward compatible context
		WGL_CONTEXT_PROFILE_MASK_ARB,	WGL_CONTEXT_CORE_PROFILE_BIT_ARB,           // Only core functionality
		0                                                                           // End of list
	};

	// If "WGL_ARB_create_context_profile" extension is not set, "WGL_CONTEXT_PROFILE_MASK_ARB" flag cannot be used
	if (!ARB_create_context_profile_present) {
		context_attribs[6] = context_attribs[7] = 0;
	}

	HGLRC rendering_context;

	// Find and create context with the highest supported on this computer version
	for (int i = 0; i < count; ++i) {
		context_attribs[1] = versions[i][major];
		context_attribs[3] = versions[i][minor];
		rendering_context = wglCreateContextAttribsARB(i_DeviceContext, 0, context_attribs);
		if (rendering_context) {
			break;
		}
	}
	if (!rendering_context) {
		UtilsInstance->ErrorMessage("OpenGL Creation Error", "Could not create OpenGL 3.0+ rendering context.");
		return false;
	}
	// Set the created context as the current context used for OpenGL's drawing commands
	if (!wglMakeCurrent(i_DeviceContext, rendering_context)) {
		wglDeleteContext(rendering_context);
		UtilsInstance->ErrorMessage("OpenGL Creation Error", "Could not activate rendering context.");
		return false;
	}

	o_RenderingContext = rendering_context;
	return true;
}

bool Application::LoadOpenGLFunctions(const HDC i_DeviceContext) {
	// Get the handle of the OpenGL library from which functions will be loaded
	HMODULE handle = GetModuleHandle("OpenGL32.dll");

	if (!(GETFUNCTIONADDRESS(PFNWGLGETEXTENSIONSSTRINGARBPROC, wglGetExtensionsStringARB))) {
		return false;
	}

	// Get all extensions supported on this computer
	const char* wglExtensions = wglGetExtensionsStringARB(i_DeviceContext);

	if (!(CHECKEXTENSION(ARB_extensions_string_present, WGL_ARB_extensions_string, wglExtensions))) {
		return false;
	}
	if (CHECKEXTENSION(ARB_pixel_format_present, WGL_ARB_pixel_format, wglExtensions)) {
		GETFUNCTIONADDRESS(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB);
	}
	if (CHECKEXTENSION(ARB_create_context_present, WGL_ARB_create_context, wglExtensions)) {
		GETFUNCTIONADDRESS(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
	}

	CHECKEXTENSION(ARB_create_context_profile_present, WGL_ARB_create_context_profile, wglExtensions);

	if (!(GETFUNCTIONADDRESS(PFNGLGETINTEGERVPROC, glGetIntegerv))) {
		return false;
	}

	int MajorVersion, MinorVersion;
	glGetIntegerv(GL_MAJOR_VERSION, &MajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &MinorVersion);

	// Application is designed for 3.0+ OGL version and forward compatible mode
	// so if version is earlier than 3.0 there is no need to get entry points for other functions
	if (MajorVersion < 3) {
		return true;
	}

	// Acquire addresses of all core OpenGL functions
	if (!(GETFUNCTIONADDRESS(PFNGLGETSTRINGPROC, glGetString)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETSTRINGIPROC, glGetStringi)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETERRORPROC, glGetError)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLCLEARCOLORPROC, glClearColor)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLCLEARPROC, glClear)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLVIEWPORTPROC, glViewport)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLCLEARDEPTHPROC, glClearDepth)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDEPTHFUNCPROC, glDepthFunc)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDEPTHMASKPROC, glDepthMask)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLBLENDFUNCPROC, glBlendFunc)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLENABLEPROC, glEnable)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDISABLEPROC, glDisable)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLFRONTFACEPROC, glFrontFace)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLCULLFACEPROC, glCullFace)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLPOLYGONMODEPROC, glPolygonMode)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLSAMPLECOVERAGEPROC, glSampleCoverage)))
		return false;

	// Shaders
	if (!(GETFUNCTIONADDRESS(PFNGLCREATESHADERPROC, glCreateShader)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLSHADERSOURCEPROC, glShaderSource)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLCOMPILESHADERPROC, glCompileShader)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETSHADERIVPROC, glGetShaderiv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDELETESHADERPROC, glDeleteShader)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLCREATEPROGRAMPROC, glCreateProgram)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLATTACHSHADERPROC, glAttachShader)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLLINKPROGRAMPROC, glLinkProgram)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETPROGRAMIVPROC, glGetProgramiv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUSEPROGRAMPROC, glUseProgram)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDETACHSHADERPROC, glDetachShader)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDELETEPROGRAMPROC, glDeleteProgram)))
		return false;

	// Vertex arrays
	if (!(GETFUNCTIONADDRESS(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)))
		return false;

	// Buffers
	if (!(GETFUNCTIONADDRESS(PFNGLGENBUFFERSPROC, glGenBuffers)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLBINDBUFFERPROC, glBindBuffer)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLBUFFERDATAPROC, glBufferData)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLBUFFERSUBDATAPROC, glBufferSubData)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDELETEBUFFERSPROC, glDeleteBuffers)))
		return false;

	// Framebuffers
	if (!(GETFUNCTIONADDRESS(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDRAWBUFFERPROC, glDrawBuffer)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDRAWBUFFERSPROC, glDrawBuffers)))
		return false;


	// Vertex attribs
	if (!(GETFUNCTIONADDRESS(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLBINDFRAGDATALOCATIONPROC, glBindFragDataLocation)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETACTIVEATTRIBPROC, glGetActiveAttrib)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETVERTEXATTRIBIVPROC, glGetVertexAttribiv)))
		return false;

	// Textures
	if (!(GETFUNCTIONADDRESS(PFNGLGENTEXTURESPROC, glGenTextures)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDELETETEXTURESPROC, glDeleteTextures)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLBINDTEXTUREPROC, glBindTexture)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLACTIVETEXTUREPROC, glActiveTexture)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLTEXPARAMETERIPROC, glTexParameteri)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLTEXIMAGE2DPROC, glTexImage2D)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap)))
		return false;

	// Uniform paramters
	if (!(GETFUNCTIONADDRESS(PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORM1FVPROC, glUniform1fv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORM2FVPROC, glUniform2fv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORM3FVPROC, glUniform3fv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORM4FVPROC, glUniform4fv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORM1IVPROC, glUniform1iv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORM2IVPROC, glUniform2iv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORM3IVPROC, glUniform3iv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORM4IVPROC, glUniform4iv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORM1IPROC, glUniform1i)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv)))
		return false;

	// Drawing
	if (!(GETFUNCTIONADDRESS(PFNGLDRAWARRAYSPROC, glDrawArrays)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLDRAWELEMENTSPROC, glDrawElements)))
		return false;
	if (!(GETFUNCTIONADDRESS(PFNGLGETERRORPROC, glGetError)))
		return false;

	return true;
}

bool Application::SetUpBasePixelFormat(const HDC i_DeviceContext) {
	// Fill the structure describing our desired format of drawing buffers
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),		// Size of the Pixel Format Descriptor
		1,									// Version number
		PFD_DRAW_TO_WINDOW |				// Support for rendering in a window
		PFD_SUPPORT_OPENGL |				// Support for OpenGL
		PFD_DOUBLEBUFFER,					// Support for double buffering
		PFD_TYPE_RGBA,						// RGBA color format (no palette)
		32,									// Number of color bits
		0, 0, 0, 0, 0, 0,					// Color bits and bit shifts for all color components...
		0, 0,								// ... and for alpha channel (ignored here - general information about 32 color bits is enough)
		0,									// No accumulation buffer
		0, 0, 0, 0,							// Accumulation bits (ignored)
		24,									// 24 bit depth buffer (Z-Buffer)
		8,									// 8 bit stencil buffer (not used but some graphics cards have problems creating drawing buffer without stencil)
		0,									// No auxiliary buffer
		PFD_MAIN_PLANE,						// Main drawing layer
		0,									// Reserved
		0, 0, 0								// Layer masks (ignored)
	};

	// Choose the best match for drawing buffer's properties
	int pixel_format = ChoosePixelFormat(i_DeviceContext, &pfd);
	if (!pixel_format) {
		UtilsInstance->ErrorMessage("OpenGL Creation Error", "Could not find the requested pixel format.");
		return false;
	}
	// Set the selected pixel format
	if (!SetPixelFormat(i_DeviceContext, pixel_format, &pfd)) {
		UtilsInstance->ErrorMessage("OpenGL Creation Error", "Could not set the requested pixel format.");
		return false;
	}
	return true;
}

bool Application::SetUpExtendedPixelFormat(const HDC i_DeviceContext) {
	// Fill the array describing our desired format of drawing buffers
	int pixel_format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB,	GL_TRUE,                        // Support for rendering in a window
		WGL_SUPPORT_OPENGL_ARB,	GL_TRUE,                        // Support for OpenGL
		WGL_DOUBLE_BUFFER_ARB,	GL_TRUE,                        // Support for double buffering
		WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB,              // RGBA color type (no palette)
		WGL_COLOR_BITS_ARB,		32,                             // 32 color bits (8 for each RGBA channel)
		WGL_DEPTH_BITS_ARB,		24,                             // 24 bits for depth buffer
		WGL_STENCIL_BITS_ARB,	8,                              // 8 bits for stencil buffer
		WGL_ACCELERATION_ARB,	WGL_FULL_ACCELERATION_ARB,      // Hardware acceleration
		0                                                       // End of list
	};

	int pixel_format = 0;
	unsigned int num_formats_found = 0;
	PIXELFORMATDESCRIPTOR pfd;

	// Choose the best format of a drawing buffers
	if (!(wglChoosePixelFormatARB(i_DeviceContext, pixel_format_attribs, nullptr, 1, &pixel_format, &num_formats_found)) || (pixel_format == 0)) {
		UtilsInstance->ErrorMessage("OpenGL Creation Error", "Could not find requested pixel format.");
		return false;
	}
	// Set the selected pixel format
	if (!SetPixelFormat(i_DeviceContext, pixel_format, &pfd)) {
		UtilsInstance->ErrorMessage("OpenGL Creation Error", "Could not set the requested pixel format.");
		return false;
	}
	return true;
}

