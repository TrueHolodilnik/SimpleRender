#include "Render.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../tinyGLTF/tiny_gltf.h"

// G-Buffer buffer defs
GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

// Create render
RenderClass::RenderClass(HDC* inDeviceContext, float* iWidth, float* iHeight) {
    DeviceContext = inDeviceContext;
    Width = iWidth;
    Height = iHeight;

    // Get perspective projection matrix
    float AspectRatio = (*Width) / (*Height);
    GetPerspectiveProjectionMatrix(45.0f, 1.0f, 20.0f, AspectRatio, ProjectionMatrix);

    // Create shaders and program objects
    if (!CreateShaders()) UtilsInstance->ErrorMessage("Shader Initialization Error", "Could not create shaders.", true);

    // Create and configure render
	BindShaderUniformAdresses();
    CreateGBuffer();
	PrepareScene();
    BindTextures();
	CreateGBRenderTargets();

    // Set viewport dimensions
    glViewport(0, 0, (int)Width, (int)Height);

    // Creatre fullscreen quad mesh
    CreateFullscreenQuad(*Width, *Height);

    // Disable vao, vbo and attributes
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    // Initial first frame render
    Render();
};

RenderClass::~RenderClass() {

	// Destroy shaders
	DestroyShaders();

	// Destroy textures
	glDeleteTextures(1, &Textures->BasePassRT);
	glDeleteTextures(1, &Textures->ColorTexture);
	glDeleteTextures(1, &Textures->NormalTexture);
	glDeleteTextures(1, &Textures->PositionTexture);
	glDeleteTextures(1, &Textures->DepthTexture);

	// Destroy geometry
    DestroyGeometry();
}

// Creates shaders and program objects used for drawing
bool RenderClass::CreateShaders() {
    GLuint vshader, fshader;

    // Base pass shaders
    //
    // Create vertex and fragment shaders for base pass
    vshader = CreateShader("Shaders/BasePass.vp", GL_VERTEX_SHADER);
    fshader = CreateShader("Shaders/BasePass.fp", GL_FRAGMENT_SHADER);
    // Create program object for base pass
    RenderPassesV->BasePassProgram = glCreateProgram();

    // Attach shaders to base pass program
    glAttachShader(RenderPassesV->BasePassProgram, vshader);
    glAttachShader(RenderPassesV->BasePassProgram, fshader);

    // Link base pass program
    glLinkProgram(RenderPassesV->BasePassProgram);

    GLint status;

    // Check linking status
    glGetProgramiv(RenderPassesV->BasePassProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar linking_info[1000];
        // Check what went wrong and display message box with information
        glGetProgramInfoLog(RenderPassesV->BasePassProgram, 1000, nullptr, linking_info);
        UtilsInstance->ErrorMessage("Shader Initialization Error", linking_info, true);
    }

    // Bind streams of vertex data with proper shader attributes
    glBindAttribLocation(RenderPassesV->BasePassProgram, 0, "inPosition");
    glBindAttribLocation(RenderPassesV->BasePassProgram, 1, "inTexCoord");
    glBindAttribLocation(RenderPassesV->BasePassProgram, 2, "inNormal");

    // Bind outputs from base pass shader
    glBindFragDataLocation(RenderPassesV->BasePassProgram, 0, "oColor");
    glBindFragDataLocation(RenderPassesV->BasePassProgram, 1, "oNormal");
    glBindFragDataLocation(RenderPassesV->BasePassProgram, 2, "oPosition");

    // Perform once again BasePassProgram linking to confirm introduced changes
    glLinkProgram(RenderPassesV->BasePassProgram);

    // Lighting pass shaders
    //
    // Create vertex and fragment shaders for lighting pass
    vshader = CreateShader("Shaders/LightingPass.vp", GL_VERTEX_SHADER);
    fshader = CreateShader("Shaders/LightingPass.fp", GL_FRAGMENT_SHADER);

    // Create program object for lighting pass
    RenderPassesV->LightingPassProgram = glCreateProgram();

    // Attach shaders to lighting pass program
    glAttachShader(RenderPassesV->LightingPassProgram, vshader);
    glAttachShader(RenderPassesV->LightingPassProgram, fshader);
    // Link lighting pass program
    glLinkProgram(RenderPassesV->LightingPassProgram);

    // Check linking status
    glGetProgramiv(RenderPassesV->LightingPassProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar linking_info[1000];
        // Check what went wrong and display message box with information
        glGetProgramInfoLog(RenderPassesV->LightingPassProgram, 1000, nullptr, linking_info);
        UtilsInstance->ErrorMessage("Shader Initialization Error", linking_info, true);
    }

    // Bind streams of vertex data with proper shader attributes
    glBindAttribLocation(RenderPassesV->LightingPassProgram, 0, "inPosition"); // 0-th
    glBindAttribLocation(RenderPassesV->LightingPassProgram, 1, "inTexcoord"); // 1-st
    glBindAttribLocation(RenderPassesV->LightingPassProgram, 2, "inTexcoord2"); // 2-nd

    // Link lighting pass program again so the new configuration is confirmed
    glLinkProgram(RenderPassesV->LightingPassProgram);
    return true;
}

// Destroy shaders for each created render pass
void RenderClass::DestroyShaders() {
    glDeleteProgram(RenderPassesV->BasePassProgram);
    glDeleteProgram(RenderPassesV->LightingPassProgram);
}

// Creates shader object of a given type from given file
GLuint RenderClass::CreateShader(const std::string i_Filename, const GLenum i_Type) {
    std::string source_code;

    // Read source code from selected file
    UtilsInstance->GetTextfileContents(i_Filename, source_code);

    const char* code = source_code.c_str();
    int length = source_code.length();

    // Create shader object
    GLuint shader = glCreateShader(i_Type);
    // Store source code in shader object
    glShaderSource(shader, 1, &code, &length);
    // Compile shader
    glCompileShader(shader);

    GLint status;

    // Check compilation status of a shader
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar compilation_info[1000];
        // Check what went wrong and display message box with information
        glGetShaderInfoLog(shader, 1000, nullptr, compilation_info);
        UtilsInstance->ErrorMessage("Shader Initialization Error", compilation_info);
        // Delete created shader object
        glDeleteShader(shader);
    }
    return shader;
}

// Reset OpenGL to default state
void RenderClass::ResetOGLStateDefault() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

// Main render runtime
void RenderClass::Render() {

    // Update move variables
    Angle = Angle > 99333 ? 0 : Angle + 0.05f;
    LightDistance = LightDistance > 4.1415 ? 0 : LightDistance - 0.0f;

    // Update light position
    glUseProgram(RenderPassesV->LightingPassProgram);
    glUniform1fv(Handlers->LightDistanceHandle, 1, &LightDistance);

    ////////////////////
    //Base render pass//
    ////////////////////
    // 
    // Activate render target (framebuffer object) for base pass
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Textures->BasePassRT);

    // Target buffers into which drawing should be performed: Color, Normal, Position
    glDrawBuffers(3, buffers);

    // Clear all render targets
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Enable depth testing for base pass
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Active shader program for base pass
    glUseProgram(RenderPassesV->BasePassProgram );

    // Loaded model
    GetYRotationMatrix(Angle, ModelViewMatrix);
    Translate(-100.0f, -200.0f, -600.0f, ModelViewMatrix);
	Scale(0.0075f, 0.0075f, 0.0075f, ModelViewMatrix);
    // Calculate MVP matrix for it
    Multiply(ProjectionMatrix, ModelViewMatrix, ModelViewProjectionMatrix);
    // Set values of shader uniform variables - MVP and MV matrices
    glUniformMatrix4fv(Handlers->MVPMatrixHandle, 1, false, ModelViewProjectionMatrix);
    glUniformMatrix4fv(Handlers->MVMatrixHandle, 1, false, ModelViewMatrix);
   
    drawModel(vaoAndEbos, model);

    // Activate VAO for drawing plane
    glBindVertexArray(GPlaneVAO);
    // Plane
    // Place plane at proper position
    GetTranslationMatrix(0.0f, -2.0f, -5.0f, ModelViewMatrix);
    // Calculate MVP matrix
    Multiply(ProjectionMatrix, ModelViewMatrix, ModelViewProjectionMatrix);
    // Set values of shader uniform variables
    glUniformMatrix4fv(Handlers->MVPMatrixHandle, 1, false, ModelViewProjectionMatrix);
    glUniformMatrix4fv(Handlers->MVMatrixHandle, 1, false, ModelViewMatrix);
    // Draw plane using VAO
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //////////////////////////
    // Lighting render pass //
    //////////////////////////
    //
    // Deactivate render target (disable FBO) before postprocess lighting phase
    // From now rendering will be performed normally, to window
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // Set the back buffer as a target for drawing commands
    glDrawBuffer(GL_BACK);

    // Clear color and depth of a back buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disable depth testing - it is not necessary in postprocess phase since we are drawing on a fullscreen quad
    glDisable(GL_DEPTH_TEST);

    // Activate shader program for lighting pass
    glUseProgram(RenderPassesV->LightingPassProgram);

    // Set projection matrix for lighting pass
    glUniformMatrix4fv(Handlers->PMatrixHandle, 1, false, ProjectionMatrix);

    // Activate VAO for drawing fullscreen quad
    glBindVertexArray(GQuadVAO);

    // Draw quad using VAO and TRIANGLES primitive
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Disable VAO - it is always good to disable all OpenGL objects when they are not required
    glBindVertexArray( 0 );

    // Swap back and front buffers (SwapChain)
    SwapBuffers( *DeviceContext );
}
                                                                                                 
// Calculates aspect ratio and updates viewport size and fullscreen quad mesh                                
void RenderClass::Resize(const int i_Width, const int i_Height) {
    int w = i_Width;
    int h = i_Height;
    if (h <= 0) {
        h = 1;
    }

    // Update viewport dimensions
    glViewport(0, 0, w, h);
    // Update fullscreen quad mesh
    CreateFullscreenQuad((float)w, (float)h);
}

// Handle key messages and update
void RenderClass::UpdateParameters(WPARAM i_wParam, LPARAM i_lParam) {
	// Switch for key messages by keys defined in KeysConfiguration.h
    switch (i_wParam) {
		// Rotate mesh to the left
        case ButtonsDefinitions::ChangeMeshesRotationL: {
            Angle -= 1.0f;
            break;
        }
        // Rotate mesh to the right
        case ButtonsDefinitions::ChangeMeshesRotationR: {
            Angle += 1.0f;
            break;
        }
		// Move light source to the left
        case ButtonsDefinitions::ChangeLightPositionL: {
            glUseProgram(RenderPassesV->LightingPassProgram);
            LightDistance -= 0.1f;
            glUniform1fv(Handlers->LightDistanceHandle, 1, &LightDistance);
            break;
        }
		// Move light source to the right
        case ButtonsDefinitions::ChangeLightPositionR: {
            glUseProgram(RenderPassesV->LightingPassProgram);
            LightDistance += 0.1f;
            glUniform1fv(Handlers->LightDistanceHandle, 1, &LightDistance);
            break;
        }
    }
}

// Get uniform addresses of shader uniform parameters
void RenderClass::BindShaderUniformAdresses() {
    Handlers->MVPMatrixHandle = glGetUniformLocation(RenderPassesV->BasePassProgram, "uMVPMatrix");
    Handlers->MVMatrixHandle = glGetUniformLocation(RenderPassesV->BasePassProgram, "uModelViewMatrix");
    Handlers->DiffuseTextureHandle = glGetUniformLocation(RenderPassesV->BasePassProgram, "uTexture");
    Handlers->DiffuseNormalTextureHandle = glGetUniformLocation(RenderPassesV->BasePassProgram, "uNormalTexture");
    Handlers->DiffusePBRTextureHandle = glGetUniformLocation(RenderPassesV->BasePassProgram, "uPBRTexture");
    Handlers->ColorTextureHandle = glGetUniformLocation(RenderPassesV->LightingPassProgram, "uColor");
    Handlers->NormalTextureHandle = glGetUniformLocation(RenderPassesV->LightingPassProgram, "uNormal");
    Handlers->PositionTextureHandle = glGetUniformLocation(RenderPassesV->LightingPassProgram, "uPosition");
    Handlers->LightDistanceHandle = glGetUniformLocation(RenderPassesV->LightingPassProgram, "uLightDistance");
    Handlers->PMatrixHandle = glGetUniformLocation(RenderPassesV->LightingPassProgram, "uPMatrix");
}

// Activate and bind textures, configure handles for render passes and deactivate any texture units
void RenderClass::BindTextures() {

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Textures->DiffuseTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_RECTANGLE, Textures->ColorTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_RECTANGLE, Textures->NormalTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_RECTANGLE, Textures->PositionTexture);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, Textures->DiffuseNormalTexture);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, Textures->DiffusePBRTexture);

    // Set values for shader uniform parameters for base pass
    glUseProgram(RenderPassesV->BasePassProgram);
    glUniform1i(Handlers->DiffuseTextureHandle, 0);
    glUniform1i(Handlers->DiffuseNormalTextureHandle, 4);
    glUniform1i(Handlers->DiffusePBRTextureHandle, 5);

    // Set values for shader uniform parameters for lighting pass
    glUseProgram(RenderPassesV->LightingPassProgram);
    glUniform1i(Handlers->ColorTextureHandle, 1);
    glUniform1i(Handlers->NormalTextureHandle, 2);
    glUniform1i(Handlers->PositionTextureHandle, 3);

    // Deactivate any texture units
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);
}

// Create G-Buffer for all data stored in base pass
void RenderClass::CreateGBuffer() {
    Textures->ColorTexture = CreateRectTexture((size_t)*Width, (size_t)*Height, GL_RGBA, GL_RGBA16F, GL_HALF_FLOAT);
    Textures->NormalTexture = CreateRectTexture((size_t)*Width, (size_t)*Height, GL_RGB, GL_RGBA16F, GL_HALF_FLOAT);
    Textures->PositionTexture = CreateRectTexture((size_t)*Width, (size_t)*Height, GL_RGB, GL_RGBA16F, GL_HALF_FLOAT);
    Textures->DepthTexture = CreateRectTexture((size_t)*Width, (size_t)*Height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);
}

// Scene setup
void RenderClass::PrepareScene() {

    if (!loadModel(model, "../Resources/scene.gltf")) {
		UtilsInstance->ErrorMessage("Model Loading Error", "Could not load model", true);
    }

    vaoAndEbos = bindModel(model);
}

// Create render targets for each part of G-Buffer
void RenderClass::CreateGBRenderTargets() {
    std::pair<GLenum, GLuint> pairs[] = {
        std::make_pair(GL_COLOR_ATTACHMENT0, Textures->ColorTexture),
        std::make_pair(GL_COLOR_ATTACHMENT1, Textures->NormalTexture),
        std::make_pair(GL_COLOR_ATTACHMENT2, Textures->PositionTexture),
        std::make_pair(GL_DEPTH_ATTACHMENT, Textures->DepthTexture),
    };
    Textures->BasePassRT = CreateRenderTarget(std::vector<std::pair<GLenum, GLuint>>(pairs, pairs + 4));
}

// Rectangle texture creation
// Paremeters: Width, Heigh, Used channels (RGB/RGBA/...), Channels format (RGB16/...), Type (half/float/int/...)
GLuint RenderClass::CreateRectTexture(const size_t i_Width, const size_t i_Height, const GLenum i_Channels, const GLenum i_Format, const GLenum i_ChannelDataType) {
    GLuint texture;

    // Generate texture ID and bind it
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_RECTANGLE, texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);      // Set linear filtering for magnification
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);      // Set linear filtering for minification
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);   // Clamp texture coordinates in texture's X direction
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);   // Clamp texture coordinates in texture's Y direction
    // Allocate memory for texture but no data is loaded
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, i_Format, i_Width, i_Height, 0, i_Channels, i_ChannelDataType, nullptr);

    // Deactivate texture
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);

    return texture;
};


// CreateRenderTarget                                                                                        
// Creates framebuffer object with attachments provided through vector of pairs:                             
// type and number of attachment - texture ID that should be attached to it                                  
GLuint RenderClass::CreateRenderTarget(const std::vector<std::pair<GLenum, GLuint>> i_Textures) {

    GLuint framebuffer;

    // Generate and framebuffer object (FBO)
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

    // Attach all provided textures to specified framebuffer attachments (places textures in specified slots)
    for (size_t i = 0; i < i_Textures.size(); ++i) {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, i_Textures[i].first, GL_TEXTURE_RECTANGLE, i_Textures[i].second, 0);
    }

    // Check status of created framebuffer
    GLenum error = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    switch (error) {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            UtilsInstance->ErrorMessage("RT Creation Error", "Incomplete attachement");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            UtilsInstance->ErrorMessage("RT Creation Error", "Missing attachement");
            break;
        default:
            UtilsInstance->ErrorMessage("RT Creation Error", "Could not create render target");
            break;
    }

    // Disable FBO for normal, on-screen rendering
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    return framebuffer;
}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

bool RenderClass::loadModel(tinygltf::Model& model, const char* filename) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty()) {
		UtilsInstance->ErrorMessage("Model Loading Warning", warn.c_str());
    }

    if (!err.empty()) {
        UtilsInstance->ErrorMessage("Model Loading Error", err.c_str(), true);
    }

    if (!res)
        UtilsInstance->ErrorMessage("Model Loading Failed", filename, true);
    else
        std::cout << "Loaded glTF: " << filename << std::endl;

    return res;
}

void RenderClass::bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh) {
    for (size_t i = 0; i < model.bufferViews.size(); ++i) {
        const tinygltf::BufferView& bufferView = model.bufferViews[i];
        if (bufferView.target == 0) { 
            UtilsInstance->ErrorMessage("WARN: bufferView.target is zero", "");
            continue;  // Unsupported bufferView.
        }

        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
        std::cout << "bufferview.target " << bufferView.target << std::endl;

        GLuint vbo;
        glGenBuffers(1, &vbo);
        vbos[i] = vbo;
        glBindBuffer(bufferView.target, vbo);

        std::cout << "buffer.data.size = " << buffer.data.size() << ", bufferview.byteOffset = " << bufferView.byteOffset << std::endl;

        glBufferData(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
    }

    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        for (auto& attrib : primitive.attributes) {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                size = accessor.type;
            }

            int vaa = -1;
            if (attrib.first.compare("POSITION") == 0) vaa = 0;
            if (attrib.first.compare("NORMAL") == 0) vaa = 2;
            if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 1;
            if (vaa > -1) {
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType, accessor.normalized ? GL_TRUE : GL_FALSE, byteStride, BUFFER_OFFSET(accessor.byteOffset));
            }
            //else
            //    UtilsInstance->ErrorMessage("vaa missing: ", attrib.first.c_str());
        }

        if (model.textures.size() > 0) {
            // fixme: Use material's baseColor
            tinygltf::Texture& tex = model.textures[0];

            GLuint* TexturesTemp[] = {&Textures->DiffuseTexture, &Textures->DiffusePBRTexture, &Textures->DiffuseNormalTexture};
            
			int iterations = min(sizeof(TexturesTemp) / sizeof(TexturesTemp[0]), model.textures.size());

			for (int i = 0; i < iterations; i++) {
        
                glGenTextures(1, TexturesTemp[i]);
                
                tinygltf::Image& image = model.images[i];

                if (image.bits == -1) {
					UtilsInstance->ErrorMessage("Texture Loading Error", "Could not load texture");
                }
        
                CreateRectTexture(image.width, image.height, GL_RGBA, GL_RGBA16F, GL_HALF_FLOAT);
        
                glBindTexture(GL_TEXTURE_2D, *TexturesTemp[i]);
                // Set texture parameters
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);     // Set linear filtering for magnification
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);     // Set linear filtering for minification
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);         // Repeat X texture coordinates around object
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);         // Repeat Y texture coordinates around object
        
				const GLenum numToFormat[] = { GL_RGBA, GL_RED, GL_RG, GL_RGB, GL_RGBA };
                GLenum format = numToFormat[image.component];
                
                GLenum type = GL_UNSIGNED_BYTE;
                if (image.bits == 16) type = GL_UNSIGNED_SHORT;    
        
                // Fill texture with data loaded from file
                glTexImage2D(GL_TEXTURE_2D,        // 2D texture
                    0,                    // 0 MIP level
                    GL_RGBA,             // 8 bits per channel
                    image.width,       // Texture width
                    image.height,      // Texture height
                    0,                    // No border
                    format,                  // 4-component texture
                    type,                   // Data type for each component
                    &image.image.at(0));      // Pointer to texture data that should be loaded onto graphics card memory
               
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
    }
}

// bind models
void RenderClass::bindModelNodes(std::map<int, GLuint>&vbos, tinygltf::Model & model, tinygltf::Node & node) {
    if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
        bindMesh(vbos, model, model.meshes[node.mesh]);
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
        bindModelNodes(vbos, model, model.nodes[node.children[i]]);
    }
}

std::pair<GLuint, std::map<int, GLuint>> RenderClass::bindModel(tinygltf::Model& model) {
    std::map<int, GLuint> vbos;
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
        bindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);
    // cleanup vbos but do not delete index buffers yet
    for (auto it = vbos.cbegin(); it != vbos.cend();) {
        tinygltf::BufferView bufferView = model.bufferViews[it->first];
        if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
            glDeleteBuffers(1, &vbos[it->first]);
            vbos.erase(it++);
        }
        else {
            ++it;
        }
    }

    return { vao, vbos };
}

// Draw mesh (per each primitive)
void RenderClass::drawMesh(const std::map<int, GLuint>& vbos, tinygltf::Model& model, tinygltf::Mesh& mesh) {
    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));
        glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType, BUFFER_OFFSET(indexAccessor.byteOffset));
    }
}
// Recursively draw node and children nodes of model
void RenderClass::drawModelNodes(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model, tinygltf::Node& node) {
    if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
        drawMesh(vaoAndEbos.second, model, model.meshes[node.mesh]);
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        drawModelNodes(vaoAndEbos, model, model.nodes[node.children[i]]);
    }
}

// Draw model per each node
void RenderClass::drawModel(const std::pair<GLuint, std::map<int, GLuint>>& vaoAndEbos, tinygltf::Model& model) {
    glBindVertexArray(vaoAndEbos.first);

    // Calculate MVP matrix for it
    Multiply(ProjectionMatrix, ModelViewMatrix, ModelViewProjectionMatrix);
    // Set values of shader uniform variables - MVP and MV matrices
    glUniformMatrix4fv(Handlers->MVPMatrixHandle, 1, false, ModelViewProjectionMatrix);
    glUniformMatrix4fv(Handlers->MVMatrixHandle, 1, false, ModelViewMatrix);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        drawModelNodes(vaoAndEbos, model, model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);
}

// Generic plane and quad data
float plane_vertices[][3] = {
    { -10.0f, 0.0f, -10.0f },
    { -10.0f, 0.0f, 10.0f },
    { 10.0f, 0.0f, 10.0f },
    { 10.0f, 0.0f, -10.0f },
};

float plane_normals[][3] = {
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
};

float plane_texcoords[][2] = {
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
};

int plane_indices[][3] = {
    { 0, 0, 0}, { 1, 1, 1}, { 2, 2, 2},
    { 0, 0, 0}, { 2, 2, 2}, { 3, 3, 3},
};

float quad_texcoords[][2] = {
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 1.0f },
    { 1.0f, 0.0f },
};

int quad_indices[][3] = {
    { 0, 0, 0}, { 1, 1, 1}, { 2, 2, 2},
    { 2, 2, 2}, { 1, 1, 1}, { 3, 3, 3},
};

float quad_vertices[][3] = {
    { -1.0f, 1.0f, 0.0f },
    { -1.0f, -1.0f, 0.0f },
    { 1.0f, 1.0f, 0.0f },
    { 1.0f, -1.0f, 0.0f },
};

void RenderClass::GenerateMesh(const size_t  i_Count,
    const int* i_Indices,
    const float* i_Vertices,
    const float* i_Texcoords,
    const float* i_Normals,
    unsigned int& io_VAO,
    unsigned int& io_VBO) {
    // Prepare array for all vertex data
    std::vector<float> vertex_buffer_data;
    vertex_buffer_data.resize(i_Count * 8);

    // Copy vertex data into one table
    for (size_t i = 0; i < i_Count; ++i) {
        size_t ind_n = 3 * i_Indices[3 * i + 0];
        size_t ind_t = 2 * i_Indices[3 * i + 1];
        size_t ind_v = 3 * i_Indices[3 * i + 2];

        vertex_buffer_data[8 * i + 0] = i_Normals[ind_n + 0];
        vertex_buffer_data[8 * i + 1] = i_Normals[ind_n + 1];
        vertex_buffer_data[8 * i + 2] = i_Normals[ind_n + 2];
        vertex_buffer_data[8 * i + 3] = i_Texcoords[ind_t + 0];
        vertex_buffer_data[8 * i + 4] = i_Texcoords[ind_t + 1];
        vertex_buffer_data[8 * i + 5] = i_Vertices[ind_v + 0];
        vertex_buffer_data[8 * i + 6] = i_Vertices[ind_v + 1];
        vertex_buffer_data[8 * i + 7] = i_Vertices[ind_v + 2];
    }

    // Generate vertex array object and vertex buffer object
    glGenVertexArrays(1, &io_VAO);
    glGenBuffers(1, &io_VBO);

    // Bind vao, vbo and load vertex data onto graphics card memory
    glBindVertexArray(io_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, io_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(float), &vertex_buffer_data[0], GL_STATIC_DRAW);

    // Bind vertex attributes with proper attribute indices
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 32, (float*)(0 * sizeof(float)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (float*)(3 * sizeof(float)));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (float*)(5 * sizeof(float)));

    // Enable vertex attributes associated with loaded vertex data
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(0);

    // Disable vao, vbo and attributes
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

unsigned int    quad_VBO;
unsigned int    plane_VBO;
void RenderClass::CreateFullscreenQuad(const float i_Width, const float i_Height) {
    // If window was resized data is already created and it needs to be deleted
    glDeleteBuffers(1, &quad_VBO);
    glDeleteVertexArrays(1, &GQuadVAO);

    // Prepare texture coordinates for RECT textures
    float quad_texcoords2[][3] = {
        { 0.0f,    i_Height, 0.0f },
        { 0.0f,    0.0f,     0.0f },
        { i_Width, i_Height, 0.0f },
        { i_Width, 0.0f,     0.0f },
    };

    // Generate fullscreen quad mesh
    GenerateMesh(sizeof(quad_indices) / sizeof(quad_indices[0]), &quad_indices[0][0], &quad_vertices[0][0], &quad_texcoords[0][0], &quad_texcoords2[0][0], GQuadVAO, quad_VBO);

    // Generate plane
    GenerateMesh(sizeof(plane_indices) / sizeof(plane_indices[0]), &plane_indices[0][0], &plane_vertices[0][0], &plane_texcoords[0][0], &plane_normals[0][0], GPlaneVAO, plane_VBO);
}

void RenderClass::DestroyGeometry() {
    glDeleteBuffers(1, &quad_VBO);
    glDeleteVertexArrays(1, &GQuadVAO);
    glDeleteBuffers(1, &plane_VBO);
    glDeleteVertexArrays(1, &GPlaneVAO);
}
