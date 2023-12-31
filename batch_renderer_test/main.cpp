#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#pragma pack(push, 1)
struct Vec2f {
    float x, y;
};

struct Vec4f {
    float x, y, z, w;
};

struct Vec4i {
    int x, y, z, w;
};

struct Index {
    int x, y, z, a, b, c;
};

struct Color : Vec4f {
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
    {
        x = (float)r / 255.0f;
        y = (float)g / 255.0f;
        z = (float)b / 255.0f;
        w = (float)a / 255.0f;
    }
};

struct Rectf {
    float x, y, w, h;
};

struct Vertex {
    Vec2f position;
    Vec4f color;
};

struct Quad {
    Vertex v[4];

    Quad(Rectf rect, Color color) {
        v[0] = { .position = { rect.x, rect.y },                         .color = color };
        v[1] = { .position = { (rect.x + rect.w), rect.y },              .color = color };
        v[2] = { .position = { (rect.x + rect.w), (rect.y + rect.h) },   .color = color };
        v[3] = { .position = { rect.x, (rect.y + rect.h) },              .color = color };
    }
};
#pragma pack(pop)

const char* vertex_shader = R"(#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;

out vec4 o_color;

uniform mat4 proj;
uniform mat4 view;

void main() {
	gl_Position = view * proj * vec4(position, 0.0, 1.0);
	o_color = color;
})";


const char* fragment_shader = R"(#version 330 core

                                in vec4 o_color;
                                out vec4 f_Color;

                                void main() {
                                    f_Color = o_color;
                                } )";

    ;

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei length,
        const GLchar* msg, const void* data)
    {
        const char* _source;
        const char* _type;
        const char* _severity;

        switch (source) {
        case GL_DEBUG_SOURCE_API:
            _source = "API";
            break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            _source = "WINDOW SYSTEM";
            break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            _source = "SHADER COMPILER";
            break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
            _source = "THIRD PARTY";
            break;

        case GL_DEBUG_SOURCE_APPLICATION:
            _source = "APPLICATION";
            break;

        case GL_DEBUG_SOURCE_OTHER:
            _source = "UNKNOWN";
            break;

        default:
            _source = "UNKNOWN";
            break;
        }

        switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            _type = "ERROR";
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            _type = "DEPRECATED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            _type = "UDEFINED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_PORTABILITY:
            _type = "PORTABILITY";
            break;

        case GL_DEBUG_TYPE_PERFORMANCE:
            _type = "PERFORMANCE";
            break;

        case GL_DEBUG_TYPE_OTHER:
            _type = "OTHER";
            break;

        case GL_DEBUG_TYPE_MARKER:
            _type = "MARKER";
            break;

        default:
            _type = "UNKNOWN";
            break;
        }

        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            _severity = "HIGH";
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            _severity = "MEDIUM";
            break;

        case GL_DEBUG_SEVERITY_LOW:
            _severity = "LOW";
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            _severity = "NOTIFICATION";
            break;

        default:
            _severity = "UNKNOWN";
            break;
        }

        printf("%d: %s of %s severity, raised from %s: %s\n",
            id, _type, _severity, _source, msg);
    }

int main() {
    glfwInit();

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);


    GLFWwindow* window = glfwCreateWindow(1024, 768, "game", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

#ifdef _WIN32
    // https://stackoverflow.com/a/16303811
    // Turn on vertical screen sync under Windows.
    // (I.e. it uses the WGL_EXT_swap_control extension)
    typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int interval);
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT)
        wglSwapIntervalEXT(1);
#else
    glfwSwapInterval(1);
#endif

    // =============== INIT DEBUG OUTPUT ================
    // The following function calls should be made directly after OpenGL
    // initialization.

    // Enable the debugging layer of OpenGL
    //
    // GL_DEBUG_OUTPUT - Faster version but not useful for breakpoints
    // GL_DEBUG_OUTPUT_SYNCHRONUS - Callback is in sync with errors, so a breakpoint
    // can be placed on the callback in order to get a stacktrace for the GL error.
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    // Set the function that will be triggered by the callback, the second parameter
    // is the data parameter of the callback, it can be useful for different
    // contexts but isn't necessary for our simple use case.
    glDebugMessageCallback(GLDebugMessageCallback, NULL);

    glViewport(0, 0, 1024, 768);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    uint32_t vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    uint32_t vbo;
    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    uint32_t ibo;
    glCreateBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    std::vector<Quad> quads;
    std::vector<Index> indices;
    int i = 0;
    for (int x = 0; x < 30; x++) {
        for (int y = 0; y < 20; y++) {
            quads.push_back({ Rectf { (float)x * 32.f, (float)y * 32.f, 32.0f, 32.0f }, Color(100 + x, 100 + y, 100) });
            
            {
                int _i = i;
                int i = _i * 4;
                indices.push_back({ i + 0, i + 1, i + 2, i + 0, i + 2, i + 3 });
            }

            i++;
        }
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(Quad) * 1'000'0, nullptr, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index) * 1'000'0, nullptr, GL_DYNAMIC_DRAW);

    uint32_t vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vertex_shader, nullptr);
    glCompileShader(vshader);

    auto CompileSuccess = [](int obj) -> bool {
        int status;
        glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
        char info[1024];
        glGetShaderInfoLog(obj, 1024, nullptr, info);

        if (status != GL_TRUE)
            printf("got info: %s\n", info);

        return status == GL_TRUE;
    };

    uint32_t fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fragment_shader, nullptr);
    glCompileShader(fshader);

    if (!CompileSuccess(vshader) || !CompileSuccess(fshader)) {
        printf("shader compile fail\n");
        return 1;
    }

    uint32_t program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    glUseProgram(program);

    glBufferSubData(GL_ARRAY_BUFFER, 0, quads.size() * sizeof(Quad), quads.data());
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(Index), indices.data());

    glm::mat4 projectionMatrix = glm::ortho(0.0f, 1024.0f, 768.0f, 0.0f);

    uint32_t location = glGetUniformLocation(program, "proj");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    //glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    //glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    //// Create a view matrix
    //glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);

    struct UserPtr {
        uint32_t program;
        int width, height;
    };

    UserPtr ptr = { program, 768, 1024 };
    glfwSetWindowUserPointer(window, &ptr);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

        UserPtr* program = (UserPtr*)glfwGetWindowUserPointer(window);
        uint32_t location = glGetUniformLocation(program->program, "proj");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glViewport(0, 0, width, height);

        program->width = width;
        program->height = height;
    });

    glm::vec2 cameraPosition(0.0f, 0.0f);
    glm::vec2 targetPosition(0.0f, 0.0f);
    // float cameraSpeed = 2.5f;

    // Time-related variables
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    uint32_t view_location = glGetUniformLocation(program, "view");

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Initialize ImGui for GLFW and OpenGL3
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    (void)ImGui::GetIO();

    float zoomLevel = 1.0f;
    float cameraSpeed = 2.5f;

    glm::vec2 viewpoint(0.0f, 0.0f);

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Handle user input to update the target camera position
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            targetPosition.y += 10.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            targetPosition.y -= 10.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            targetPosition.x -= 10.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            targetPosition.x += 10.0f * deltaTime;

        // Smoothly interpolate camera position towards the target position
        float smoothingFactor = 5.f; // Adjust as needed

        // cameraPosition.x = glm::mix(cameraPosition.x, targetPosition.x, smoothingFactor * deltaTime);
        // cameraPosition.y = glm::mix(cameraPosition.y, targetPosition.y, smoothingFactor * deltaTime);

        // cameraSpeed = 2.5f / zoomLevel;
        // targetPosition *= zoomLevel;
        cameraPosition = glm::mix(cameraPosition, targetPosition, smoothingFactor * deltaTime);

        viewpoint *= zoomLevel;

        // Create a view matrix (assuming 2D orthographic projection)
        glm::mat4 viewMatrix = glm::lookAt(glm::vec3(cameraPosition.x, cameraPosition.y, 0.0f), glm::vec3(cameraPosition.x, cameraPosition.y, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, quads.size() * 6, GL_UNSIGNED_INT, nullptr);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();
        ImGui::Begin("Tools");
        ImGui::DragFloat2("Camera target position", (float*)&targetPosition);

        // Zoom control
        ImGui::Text("Zoom Level: %.2f", zoomLevel);
        ImGui::SliderFloat("Zoom", &zoomLevel, 0.1f, 3.0f);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
