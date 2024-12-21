#include "OpenGLFunctions.h"

// WGL
PFNWGLGETEXTENSIONSSTRINGARBPROC    wglGetExtensionsStringARB;
PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC   wglCreateContextAttribsARB;

// OGL
PFNGLGETSTRINGPROC                  glGetString;
PFNGLGETSTRINGIPROC                 glGetStringi;
PFNGLGETINTEGERVPROC                glGetIntegerv;
PFNGLGETERRORPROC                   glGetError;
PFNGLVIEWPORTPROC                   glViewport;
PFNGLCLEARPROC                      glClear;
PFNGLCLEARCOLORPROC                 glClearColor;
PFNGLCLEARDEPTHPROC                 glClearDepth;
PFNGLDEPTHFUNCPROC                  glDepthFunc;
PFNGLDEPTHMASKPROC                  glDepthMask;
PFNGLBLENDFUNCPROC                  glBlendFunc;
PFNGLENABLEPROC                     glEnable;
PFNGLDISABLEPROC                    glDisable;
PFNGLFRONTFACEPROC                  glFrontFace;
PFNGLCULLFACEPROC                   glCullFace;
PFNGLPOLYGONMODEPROC                glPolygonMode;
PFNGLSAMPLECOVERAGEPROC             glSampleCoverage;

// Shaders
PFNGLCREATESHADERPROC               glCreateShader;
PFNGLSHADERSOURCEPROC               glShaderSource;
PFNGLCOMPILESHADERPROC              glCompileShader;
PFNGLGETSHADERIVPROC                glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog;
PFNGLDELETESHADERPROC               glDeleteShader;
PFNGLCREATEPROGRAMPROC              glCreateProgram;
PFNGLATTACHSHADERPROC               glAttachShader;
PFNGLLINKPROGRAMPROC                glLinkProgram;
PFNGLGETPROGRAMIVPROC               glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC                 glUseProgram;
PFNGLDETACHSHADERPROC               glDetachShader;
PFNGLDELETEPROGRAMPROC              glDeleteProgram;

// Vertex arrays
PFNGLGENVERTEXARRAYSPROC            glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC            glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC         glDeleteVertexArrays;

// Buffers
PFNGLGENBUFFERSPROC                 glGenBuffers;
PFNGLBINDBUFFERPROC                 glBindBuffer;
PFNGLBUFFERDATAPROC                 glBufferData;
PFNGLBUFFERSUBDATAPROC              glBufferSubData;
PFNGLDELETEBUFFERSPROC              glDeleteBuffers;

// Framebuffer
PFNGLGENFRAMEBUFFERSPROC            glGenFramebuffers;
PFNGLBINDFRAMEBUFFERPROC            glBindFramebuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC       glFramebufferTexture2D;
PFNGLCHECKFRAMEBUFFERSTATUSPROC     glCheckFramebufferStatus;
PFNGLDRAWBUFFERPROC                 glDrawBuffer;
PFNGLDRAWBUFFERSPROC                glDrawBuffers;

// Vertex attribs
PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer;
PFNGLBINDATTRIBLOCATIONPROC         glBindAttribLocation;
PFNGLBINDFRAGDATALOCATIONPROC       glBindFragDataLocation;
PFNGLGETATTRIBLOCATIONPROC          glGetAttribLocation;
PFNGLGETACTIVEATTRIBPROC            glGetActiveAttrib;
PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC   glDisableVertexAttribArray;
PFNGLGETVERTEXATTRIBIVPROC          glGetVertexAttribiv;

// Textures
PFNGLGENTEXTURESPROC                glGenTextures;
PFNGLDELETETEXTURESPROC             glDeleteTextures;
PFNGLBINDTEXTUREPROC                glBindTexture;
PFNGLACTIVETEXTUREPROC              glActiveTexture;
PFNGLTEXPARAMETERIPROC              glTexParameteri;
PFNGLTEXIMAGE2DPROC                 glTexImage2D;
PFNGLGENERATEMIPMAPPROC             glGenerateMipmap;

// Uniform Parameters
PFNGLGETACTIVEUNIFORMPROC           glGetActiveUniform;
PFNGLGETUNIFORMLOCATIONPROC         glGetUniformLocation;
PFNGLUNIFORM1FVPROC                 glUniform1fv;
PFNGLUNIFORM2FVPROC                 glUniform2fv;
PFNGLUNIFORM3FVPROC                 glUniform3fv;
PFNGLUNIFORM4FVPROC                 glUniform4fv;
PFNGLUNIFORM1IVPROC                 glUniform1iv;
PFNGLUNIFORM2IVPROC                 glUniform2iv;
PFNGLUNIFORM3IVPROC                 glUniform3iv;
PFNGLUNIFORM4IVPROC                 glUniform4iv;
PFNGLUNIFORM1IPROC                  glUniform1i;
PFNGLUNIFORMMATRIX3FVPROC           glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC           glUniformMatrix4fv;

// Drawing
PFNGLDRAWARRAYSPROC                 glDrawArrays;
PFNGLDRAWELEMENTSPROC               glDrawElements;
