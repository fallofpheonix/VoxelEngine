#include "OpenGLRenderer.h"

#include "Foundation/Assert.h"
#include "Foundation/Log.h"
#include "Platform/Window.h"

#include <GL/glew.h>

namespace Engine {

// --- Step 7: Camera helper ---

Camera Camera::Perspective(const Vector3& eye, const Vector3& target, float fovYRadians,
                           float aspect, float nearZ, float farZ) {
    Camera cam;
    cam.projection = Matrix4::Perspective(fovYRadians, aspect, nearZ, farZ);
    cam.view = Matrix4::LookAt(eye, target, Vector3::Up());
    return cam;
}

// --- Step 2: Internal shader helpers ---

namespace {

unsigned int CompileShader(unsigned int type, const std::string& source) {
    const unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        int length = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::string error(length, ' ');
        glGetShaderInfoLog(id, length, &length, error.data());
        ENGINE_LOG_ERROR(std::string("Shader compilation failed: ") + error);
        glDeleteShader(id);
        return 0;
    }
    return id;
}

} // namespace

// --- OpenGLRenderer lifetime ---

OpenGLRenderer::OpenGLRenderer(Window* window) : m_window(window) {
    ENGINE_ASSERT(m_window != nullptr, "OpenGLRenderer: window must not be null");

    const auto* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    ENGINE_ASSERT(version != nullptr, "OpenGLRenderer: failed to query GL_VERSION -- is a GL context current?");
    ENGINE_LOG_INFO(std::string("OpenGLRenderer initialized: ") + version);

    glEnable(GL_DEPTH_TEST);
}

OpenGLRenderer::~OpenGLRenderer() {
    for (unsigned int s : m_shaders) {
        glDeleteProgram(s);
    }
    for (auto& mesh : m_meshes) {
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
    }
    for (unsigned int t : m_textures) {
        glDeleteTextures(1, &t);
    }
}

// --- Step 2: Shader ---

ShaderHandle OpenGLRenderer::CreateShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
    const unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    if (!vs) return {};

    const unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!fs) {
        glDeleteShader(vs);
        return {};
    }

    const unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        int length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::string error(length, ' ');
        glGetProgramInfoLog(program, length, &length, error.data());
        ENGINE_LOG_ERROR(std::string("Shader link failed: ") + error);
        glDeleteProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return {};
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    m_shaders.push_back(program);
    return {static_cast<uint32_t>(m_shaders.size())};
}

// --- Steps 3+4: Mesh (VBO/EBO/VAO) ---

MeshHandle OpenGLRenderer::CreateMesh(const MeshData& data) {
    MeshGL mesh{};

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.vertices.size() * sizeof(Vertex)),
                 data.vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.indices.size() * sizeof(uint32_t)),
                 data.indices.data(), GL_STATIC_DRAW);

    // position: location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));

    // normal: location 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));

    // uv: location 2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glBindVertexArray(0);

    mesh.indexCount = static_cast<unsigned int>(data.indices.size());
    m_meshes.push_back(mesh);
    return {static_cast<uint32_t>(m_meshes.size())};
}

// --- Step 5: Texture ---

TextureHandle OpenGLRenderer::CreateTexture(int width, int height, const uint8_t* rgbaPixels) {
    unsigned int texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaPixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_textures.push_back(texture);
    return {static_cast<uint32_t>(m_textures.size())};
}

// --- Step 8: Render Queue ---

void OpenGLRenderer::BeginFrame(const Camera& camera) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_currentShader = 0;
    (void)camera;
}

void OpenGLRenderer::Submit(const RenderCommand& command) {
    if (!command.mesh.IsValid()) return;

    const MeshGL& mesh = m_meshes[command.mesh.id - 1];

    if (command.material.shader.IsValid()) {
        m_currentShader = m_shaders[command.material.shader.id - 1];
        glUseProgram(m_currentShader);
    }

    if (command.material.texture.IsValid()) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textures[command.material.texture.id - 1]);
    }

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indexCount), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void OpenGLRenderer::EndFrame() {
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_currentShader = 0;
}

// --- Factory ---

std::unique_ptr<IRenderer> IRenderer::Create(Window* window) {
    return std::make_unique<OpenGLRenderer>(window);
}

} // namespace Engine
