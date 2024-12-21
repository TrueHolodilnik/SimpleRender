#ifndef _OPENGL_FUNCTIONS_HEADER_
#define _OPENGL_FUNCTIONS_HEADER_

#include <Windows.h>
#include <GL\glcorearb.h>
#include <GL\wglext.h>

// WGL
extern PFNWGLGETEXTENSIONSSTRINGARBPROC		wglGetExtensionsStringARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC		wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC	wglCreateContextAttribsARB;

// OGL
extern PFNGLGETSTRINGPROC                   glGetString;
extern PFNGLGETSTRINGIPROC                  glGetStringi;
extern PFNGLGETINTEGERVPROC                 glGetIntegerv;
extern PFNGLGETERRORPROC                    glGetError;
extern PFNGLVIEWPORTPROC                    glViewport;
extern PFNGLCLEARPROC                       glClear;
extern PFNGLCLEARCOLORPROC                  glClearColor;
extern PFNGLCLEARDEPTHPROC                  glClearDepth;
extern PFNGLDEPTHFUNCPROC                   glDepthFunc;
extern PFNGLDEPTHMASKPROC                   glDepthMask;
extern PFNGLBLENDFUNCPROC                   glBlendFunc;
extern PFNGLENABLEPROC                      glEnable;
extern PFNGLDISABLEPROC                     glDisable;
extern PFNGLFRONTFACEPROC                   glFrontFace;
extern PFNGLCULLFACEPROC                    glCullFace;
extern PFNGLPOLYGONMODEPROC                 glPolygonMode;
extern PFNGLSAMPLECOVERAGEPROC              glSampleCoverage;

// Shaders
extern PFNGLCREATESHADERPROC                glCreateShader;
extern PFNGLSHADERSOURCEPROC                glShaderSource;
extern PFNGLCOMPILESHADERPROC               glCompileShader;
extern PFNGLGETSHADERIVPROC                 glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC            glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC                glDeleteShader;
extern PFNGLCREATEPROGRAMPROC               glCreateProgram;
extern PFNGLATTACHSHADERPROC                glAttachShader;
extern PFNGLLINKPROGRAMPROC                 glLinkProgram;
extern PFNGLGETPROGRAMIVPROC                glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC           glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC                  glUseProgram;
extern PFNGLDETACHSHADERPROC                glDetachShader;
extern PFNGLDELETEPROGRAMPROC               glDeleteProgram;

// Vertex arrays
extern PFNGLGENVERTEXARRAYSPROC             glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC             glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC          glDeleteVertexArrays;

// Buffers
extern PFNGLGENBUFFERSPROC                  glGenBuffers;
extern PFNGLBINDBUFFERPROC                  glBindBuffer;
extern PFNGLBUFFERDATAPROC                  glBufferData;
extern PFNGLBUFFERSUBDATAPROC               glBufferSubData;
extern PFNGLDELETEBUFFERSPROC               glDeleteBuffers;

// Framebuffer
extern PFNGLGENFRAMEBUFFERSPROC             glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC             glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC        glFramebufferTexture2D;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC      glCheckFramebufferStatus;
extern PFNGLDRAWBUFFERPROC                  glDrawBuffer;
extern PFNGLDRAWBUFFERSPROC                 glDrawBuffers;

// Vertex attribs
extern PFNGLVERTEXATTRIBPOINTERPROC         glVertexAttribPointer;
extern PFNGLBINDATTRIBLOCATIONPROC          glBindAttribLocation;
extern PFNGLBINDFRAGDATALOCATIONPROC        glBindFragDataLocation;
extern PFNGLGETATTRIBLOCATIONPROC           glGetAttribLocation;
extern PFNGLGETACTIVEATTRIBPROC             glGetActiveAttrib;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC     glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC    glDisableVertexAttribArray;
extern PFNGLGETVERTEXATTRIBIVPROC           glGetVertexAttribiv;

// Textures
extern PFNGLGENTEXTURESPROC                 glGenTextures;
extern PFNGLDELETETEXTURESPROC              glDeleteTextures;
extern PFNGLBINDTEXTUREPROC                 glBindTexture;
extern PFNGLACTIVETEXTUREPROC               glActiveTexture;
extern PFNGLTEXPARAMETERIPROC               glTexParameteri;
extern PFNGLTEXIMAGE2DPROC                  glTexImage2D;
extern PFNGLGENERATEMIPMAPPROC              glGenerateMipmap;

// Uniform Parameters
extern PFNGLGETACTIVEUNIFORMPROC            glGetActiveUniform;
extern PFNGLGETUNIFORMLOCATIONPROC          glGetUniformLocation;
extern PFNGLUNIFORM1FVPROC                  glUniform1fv;
extern PFNGLUNIFORM2FVPROC                  glUniform2fv;
extern PFNGLUNIFORM3FVPROC                  glUniform3fv;
extern PFNGLUNIFORM4FVPROC                  glUniform4fv;
extern PFNGLUNIFORM1IVPROC                  glUniform1iv;
extern PFNGLUNIFORM2IVPROC                  glUniform2iv;
extern PFNGLUNIFORM3IVPROC                  glUniform3iv;
extern PFNGLUNIFORM4IVPROC                  glUniform4iv;
extern PFNGLUNIFORM1IPROC                   glUniform1i;
extern PFNGLUNIFORMMATRIX3FVPROC            glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC            glUniformMatrix4fv;

// Drawing
extern PFNGLDRAWARRAYSPROC                  glDrawArrays;
extern PFNGLDRAWELEMENTSPROC                glDrawElements;

#endif // _OPENGL_FUNCTIONS_HEADER_