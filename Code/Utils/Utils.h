#pragma once
#include <Windows.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

// Utils singleton
class Utils {

private:
    

public:
    static Utils& instance()
    {
        static Utils INSTANCE;
        return INSTANCE;
    }

    void ErrorMessage(const char* i_Title, const char* i_Message, bool critical = false) {
        // Display message box
        MessageBox(nullptr, i_Message, i_Title, MB_OK | MB_ICONINFORMATION);
		if (critical) {
			exit(1);
		}
    }

    // Stores contents of a text file in a given variable
    static void GetTextfileContents(const std::string i_Filename, std::string& o_FileContents) {
        std::stringstream contents_stream;

        // Open selected file
        std::ifstream file(i_Filename);
        if (file.fail()) {
            return;
        }

        // Read data from file
        contents_stream << file.rdbuf();
        file.close();

        // Store file contents
        o_FileContents = contents_stream.str();
    }

	// Check for OpenGL errors
    void CheckGLError(const char* stmt, const char* fname, int line) {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL error " << err << " at " << fname << ":" << line << " - for " << stmt << std::endl;
            exit(1);
        }
    }

	// Check if given extension is present in the list of all extensions
    bool CheckExtension(const char* i_Extension, const char* i_AllExtensions) {
        // Check if given extension string is present
        const char* start = strstr(i_AllExtensions, i_Extension);
        if (start == nullptr) {
            return false;
        }

        size_t i = 0;
        size_t length = strlen(i_Extension);

        // Make sure that string matches exactly the one provided
        for (; i < length; ++i) {
            if (i_Extension[i] != start[i]) {
                return false;
            }
        }
        // And check if it is not another extension which name contains the one we are searching for
        if ((start[i] != ' ') && (start[i] != '\0')) {
            return false;
        }
        return true;
    }

    void* GetFunctionAddress(const HMODULE i_Handle, const char* i_ProcedureName) {
        // Get the address of a given function (for OpenGL versions 1.2+)
        PROC address = wglGetProcAddress(i_ProcedureName);
        if (!address) {
            // Get the address of a function from OpenGL32.dll (versions 1.0 and 1.1)
            address = GetProcAddress(i_Handle, i_ProcedureName);
            if (!address) {
                ErrorMessage("Error Getting Function Address", i_ProcedureName);
            }
        }
        return address;
    }

	void CheckLinkingStatus(GLuint i_Program) {
		GLint status;
		// Check linking status
		glGetProgramiv(i_Program, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			GLchar linking_info[1000];
			// Check what went wrong and display message box with information
			glGetProgramInfoLog(i_Program, 1000, nullptr, linking_info);
			this->ErrorMessage("Shader Initialization Error", linking_info, true);
		}
	}
    
};

static Utils* UtilsInstance = &(Utils::instance());