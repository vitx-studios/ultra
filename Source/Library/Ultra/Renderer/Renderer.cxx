﻿module;

#pragma warning(push, 0)
//https://github.com/nothings/stb/issues/334
#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
#endif
#define STB_IMAGE_STATIC
#include <stb/stb_image.h>
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma warning(pop)

module Ultra.Renderer;

import <glad/gl.h>;
import <glm/glm.hpp>;
import <glm/gtc/matrix_transform.hpp>;
import <glm/gtc/type_ptr.hpp>;

import Ultra.Graphics.Context;
import Ultra.Platform.DXRenderer;
import Ultra.Platform.GLRenderer;
import Ultra.Platform.VKRenderer;
import Ultra.Platform.Renderer.DXRenderDevice;
import Ultra.Platform.Renderer.GLRenderDevice;
import Ultra.Platform.Renderer.VKRenderDevice;

import Ultra.Asset;
import Ultra.Asset.Model;
import Ultra.Renderer.Buffer;
import Ultra.Renderer.PipelineState;
import Ultra.Renderer.Texture;
import Ultra.System.Input;

namespace Ultra {

Scope<Renderer> Renderer::Create() {
    Scope<Renderer> renderer;
    Scope<RenderDevice> device;

    auto api = Context::GetAPI();
    switch (api) {
        case GraphicsAPI::DirectX: {
            renderer = CreateScope<DXRenderer>();
            device = CreateScope<DXRenderDevice>();
            break;
        }
        case GraphicsAPI::OpenGL: {
            renderer = CreateScope<GLRenderer>();
            device = CreateScope<GLRenderDevice>();
            break;
        }
        case GraphicsAPI::Vulkan: {
            renderer = CreateScope<VKRenderer>();
            device = CreateScope<VKRenderDevice>();
            break;
        }
        default: {
        #if APP_MODE_DEBUG
            throw std::runtime_error("Renderer: RenderAPI not supported!");
        #else
            LogFatal("RenderAPI not supported!");
        #endif
        }
    }
    renderer->mRenderDevice = std::move(device);
    renderer->Load();
    return renderer;
}

void Renderer::Load() {
    mRenderDevice->Load();
    mRenderDevice->SetLineThickness(3.0f);
    mCommandBuffer = CommandBuffer::Create();
    mCommandBuffer->SetViewport(0, 0, 1280, 1024);
    Renderer2D::Load();
}

void Renderer::RenderFrame() {
    // Create render states
    //auto renderState = RenderState::Create();
    // Begin recording commands
    //commandBuffer->Begin();
    mCommandBuffer->Clear(0.1f, 0.1f, 0.1f, 1.0f);;     // Clear the framebuffer
    //commandBuffer->BindRenderState(renderState);      // Set up the render state
    Renderer2D::ResetStatistics();

    //Renderer::EndScene();
    //commandBuffer->End();                             // End recording commands
    //commandBuffer->Execute();                         // Execute the command buffer
    //swapchain->Present();                             // Present the rendered image to the screen
}

void Renderer::Dispose() {
    Renderer2D::Dispose();
    mRenderDevice->Dispose();
}

struct CameraData {
    glm::mat4 ViewProjection = {};
    glm::mat4 Projection = {};
    glm::mat4 View = {};
    float NearClip = {};
    float FarClip = {};
};

void Renderer::DrawGrid(const DesignerCamera &camera) {
    static unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
    static float vertices[] = {
        -1.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  0.0f,
         1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,
    };
    static CameraData cameraUniformData;

    static Reference<Shader> BasicShader = Shader::Create("Assets/Shaders/Grid.glsl");
    static Reference<PipelineState> BasicPipeline = PipelineState::Create({
        .Layout = {
            { ShaderDataType::Float3, "aPosition" },
        }
    });
    static Reference<Buffer> BasicVertex = Buffer::Create(BufferType::Vertex, vertices, sizeof(vertices));
    static Reference<Buffer> BasicIndex = Buffer::Create(BufferType::Index, indices, sizeof(indices));
    static Reference<Buffer> BasicUniform = Buffer::Create(BufferType::Uniform, nullptr, sizeof(CameraData));
    BasicUniform->Bind(0);

    // Renderer
    BasicShader->Bind();
    cameraUniformData.Projection = camera.GetProjection();
    cameraUniformData.View = camera.GetViewMatrix();
    cameraUniformData.ViewProjection = camera.GetViewProjection();
    cameraUniformData.NearClip = camera.GetNearPoint();
    cameraUniformData.FarClip = camera.GetFarPoint();
    BasicUniform->UpdateData(&cameraUniformData, sizeof(CameraData));
    BasicVertex->Bind();
    BasicPipeline->Bind();
    BasicIndex->Bind();
    mCommandBuffer->DrawIndexed(6, PrimitiveType::Triangle, true);
}

#pragma region Test

#define TEST_ABSTRACT_RENDER_CALLS 1

static inline CommandBuffer *sCommandBuffer = nullptr;

struct Components {
    float CubeVertices[24] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
    };
    unsigned int CubeIndices[36] = {
         0,  1,  2,  2,  3,  0, // Front
         4,  5,  6,  6,  7,  4, // Back
         3,  2,  6,  6,  7,  3, // Top
         0,  1,  5,  5,  4,  0, // Bottom
         0,  3,  7,  7,  4,  0, // Left
         1,  2,  6,  6,  5,  1, // Right
    };
    int CubeComponents = 36;

    float TriangleVertices[27] = {
        // Positions            // Colors                   // Texture Coords
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f, 1.0f,     0.0f, 0.0f,         // Left
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,     1.0f, 0.0f,         // Right
         0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, 1.0f,     0.5f, 1.0f,         // Top
    };
    unsigned int TriangleIndices[3] = {
        0, 1, 2,
    };
    int TriangleComponents = 3;

    float RectangleVertices[36] = {
        // DrawArrays (specify every rectangle)
        //// First Triangle
        // 0.5f,  0.5f, 0.0f, // Top-Right
        // 0.5f, -0.5f, 0.0f, // Bottom-ight
        //-0.5f,  0.5f, 0.0f, // Top-Left

        //// Second Triangle
        // 0.5f, -0.5f, 0.0f, // Bottom-Right
        //-0.5f, -0.5f, 0.0f, // Bottom-Left
        //-0.5f,  0.5f, 0.0f, // Top-Left

        // DrawIndex (specify only the vertices once)
        // Positions                // Colors           // Texture Coords
         0.5f,  0.5f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Top-Right
         0.5f, -0.5f, 0.0f, 0.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f, // Bottom-Right
        -0.5f, -0.5f, 0.0f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Bottom-Left
        -0.5f,  0.5f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, // Top-Left 
    };
    unsigned int RectangleIndices[6] = {
        0, 1, 3,    // First Triangle
        1, 2, 3,    // Second Triangle
    };
    int RectangleComponents = 6;

    float TriangleTexCoords[6] = {
        // Triangle
        0.0f, 0.0f,  // Lower-Left corner  
        1.0f, 0.0f,  // Lower-Right corner
        0.5f, 1.0f   // Top-Center corner
    };

    struct CameraData {
        glm::mat4 ViewProjectionMatrix = {};
        glm::mat4 Projection = {};
        glm::mat4 View = {};
        float NearClip = {};
        float FarClip = {};
    } Camera;
    Reference<Buffer> CameraUniformBuffer;

    struct CameraData2 {
        glm::mat4 Model = {};
        glm::mat4 View = {};
        glm::mat4 Projection = {};
    } Camera2;
    Reference<Buffer> CameraUniformBuffer2;

    struct UTranslation {
        glm::mat4 Transform = glm::mat4(1.0f);
    } Translation;

    struct UProperties {
        glm::vec4 Color = glm::vec4(1.0f);
    } Properties;

    struct UMaterial {
        glm::vec3 uDiffuseColor {};
        glm::vec3 uSpecularColor {};
        glm::vec3 uAmbientColor {};
        float Shininess;
    } Material;
    Reference<Buffer> MaterialBuffer;

    struct ULight {
        alignas(16) glm::vec3 LightColor {};
        alignas(16) glm::vec3 LightPosition {};
        alignas(16) glm::vec3 Ambient {};
        alignas(16) glm::vec3 Diffuse {};
        alignas(16) glm::vec3 Specular {};
    } Light;
    Reference<Buffer> LightBuffer;

    struct UView {
        glm::vec3 Position;
    } View;
    Reference<Buffer> ViewBuffer;
} static sComponents;

static void TestAgnostic(const DesignerCamera &camera);

static void TestGL(const DesignerCamera &camera);


void Renderer::Test(const DesignerCamera &camera) {
    if (static bool once = true; once) {
        once = false;
        sComponents.CameraUniformBuffer = Buffer::Create(BufferType::Uniform, nullptr, sizeof(Components::CameraData));
        sComponents.CameraUniformBuffer2 = Buffer::Create(BufferType::Uniform, nullptr, sizeof(Components::CameraData2));

        sComponents.MaterialBuffer = Buffer::Create(BufferType::Uniform, nullptr, sizeof(Components::UMaterial));
        sComponents.LightBuffer = Buffer::Create(BufferType::Uniform, nullptr, sizeof(Components::ULight));
        sComponents.ViewBuffer = Buffer::Create(BufferType::Uniform, nullptr, sizeof(Components::UView));
    }
    sCommandBuffer = mCommandBuffer.get();

#if TEST_ABSTRACT_RENDER_CALLS
    TestAgnostic(camera);
#else
    TestGL(camera);
#endif
}

void TestAgnostic(const DesignerCamera &camera) {
    // Prepare Translation
    auto projection = camera.GetProjectionMatrix();
    auto view = camera.GetViewMatrix();
    auto model = glm::mat4(1.0f);

    // Load Shaders
    static auto lightShader = Shader::Create("Assets/Shaders/Light.glsl");
    static auto modelShader = Shader::Create("Assets/Shaders/Model.glsl");
    
    // ToDo: Create Light Component
    static auto timeValue = 0.1f;
    timeValue += 0.0001f;
    static auto lightPos = glm::vec3(-3.0f, 3.0f, 5.0f);
    //float radius = 1.5f;
    //float angle = timeValue * 2.0f;
    //lightPos.x = 1.0f + radius * std::cos(angle);
    //lightPos.z = 3.0f + radius * std::sin(angle);

    // Specify Pipeline and Shader
    static PipelineProperties properties;
    properties.DepthTest = true;
    properties.Wireframe = false;
    properties.Layout = {
        { ShaderDataType::Float3, "aPosition" }
    };
    static auto pipeline = PipelineState::Create(properties);

    // Specify Buffers and Textures
    static auto vertexBuffer = Buffer::Create(BufferType::Vertex, &sComponents.CubeVertices, sizeof(sComponents.CubeVertices));
    static auto indexBuffer = Buffer::Create(BufferType::Index, &sComponents.CubeIndices, sizeof(sComponents.CubeIndices));
    static auto propertiesUniform = Buffer::Create(BufferType::Uniform, &sComponents.Properties, sizeof(sComponents.Properties));
    static auto translationUnfiorm = Buffer::Create(BufferType::Uniform, &sComponents.Translation, sizeof(sComponents.Translation));

    // Update Vertices and Indices
    vertexBuffer->UpdateData(&sComponents.CubeVertices, sizeof(sComponents.CubeVertices));
    indexBuffer->UpdateData(&sComponents.CubeIndices, sizeof(sComponents.CubeIndices));

    // Update Properties
    float value = (sin(timeValue) / 2.0f) + 0.5f;
    sComponents.Properties.Color = glm::vec4(1.0f, 1.0f - value, value, 1.0f);
    propertiesUniform->UpdateData(&sComponents.Properties, sizeof(sComponents.Properties));

    // Update Translation
    auto lightModel = glm::translate(glm::mat4(1.0f), lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f, 0.2f, 0.2f));

    auto lightTransform = projection * view * lightModel;
    sComponents.Translation.Transform = lightTransform;
    translationUnfiorm->UpdateData(&sComponents.Translation, sizeof(sComponents.Translation));

    // Draw
    vertexBuffer->Bind();
    pipeline->Bind();
    indexBuffer->Bind();

    lightShader->Bind();
    propertiesUniform->Bind(0);
    translationUnfiorm->Bind(1);
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    sCommandBuffer->DrawIndexed(sComponents.CubeComponents, PrimitiveType::Triangle, true);

    // Load Models
    static Model cone("Assets/Models/Cone/Cone.obj");
    static Model cube("Assets/Models/Cube/Cube.obj");
    static Model cylinder("Assets/Models/Cylinder/Cylinder.obj");
    static Model level("Assets/Models/Level/Level.obj");
    static Model monkey("Assets/Models/Monkey/Monkey.obj");
    static Model plane("Assets/Models/Plane/Plane.obj");
    static Model sphere("Assets/Models/Sphere/SphereUV.obj");
    static Model torus("Assets/Models/Torus/Torus.obj");

    // Translate Cube Model
    auto cubeModel = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    cubeModel = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    // Update Camera, Light and View
    modelShader->Bind();
    sComponents.Camera2.Projection = projection;
    sComponents.Camera2.View = view;
    sComponents.Camera2.Model = cubeModel;

    sComponents.Material.Shininess = 16.0f;
    sComponents.Light.LightColor = { 1.0f, 1.0f, 1.0f };  //sComponents.Properties.Color; // 
    sComponents.Light.LightPosition = lightPos;
    sComponents.Light.Ambient = { 0.1f, 0.1f, 0.1f };
    sComponents.Light.Diffuse = { 0.5f, 0.5f, 0.5f };
    sComponents.Light.Specular = { 1.0f, 1.0f, 1.0f };
    sComponents.View.Position = camera.GetPosition();

    int location = modelShader->FindUniformLocation("Light");

    sComponents.CameraUniformBuffer2->Bind(0);
    //sComponents.MaterialBuffer->Bind(4);
    sComponents.LightBuffer->Bind(5);
    sComponents.ViewBuffer->Bind(6);
    sComponents.CameraUniformBuffer2->UpdateData(&sComponents.Camera2, sizeof(Components::CameraData2));
    //sComponents.MaterialBuffer->UpdateData(&sComponents.Material, sizeof(Components::UMaterial));
    sComponents.LightBuffer->UpdateData(&sComponents.Light, sizeof(Components::ULight));
    sComponents.ViewBuffer->UpdateData(&sComponents.View, sizeof(Components::UView));

    cube.Draw(sCommandBuffer);
}

void TestGL(const DesignerCamera &camera) {
    // Shader Source Code
    #pragma region Shader Source Code
    static auto vertexShaderSource = R"(
        #version 450

        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec4 aColor;
        layout (location = 2) in vec2 aTexCoord;

        layout (location = 0) out vec4 vColor;
        layout (location = 1) out vec2 vTexCoord;

        uniform mat4 uTransform;

        void main() {
            vColor = aColor;
            vTexCoord = aTexCoord;

            gl_Position = uTransform * vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
        }
    )";

    static auto fragmentShaderSource = R"(
        #version 450

        layout (location = 0) in vec4 vColor;
        layout (location = 1) in vec2 vTexCoord;
        
        layout (location = 0) out vec4 oFragColor;

        uniform vec4 uColor;
        uniform sampler2D uTexture;

        void main() {
            oFragColor = texture(uTexture, vTexCoord) * uColor;// * vColor;
        } 
    )";
    #pragma endregion

    // Build and Compile Shaders
    auto checkProgramState = [](unsigned int id) -> bool {
        vector<char> message;
        message.resize(1024);
        int result {};
        glGetProgramiv(id, GL_LINK_STATUS, &result);
        if (!result) {
            glGetProgramInfoLog(id, 1024, nullptr, message.data());
            Log("Error: Shader link failed: {0}", message.data());
            return false;
        };
        return true;
    };
    auto checkShaderState = [](unsigned int id) -> bool {
        vector<char> message(1024);
        int result {};
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (!result) {
            glGetShaderInfoLog(id, 1024, nullptr, message.data());
            Log("Error: Shader compilation failed: {0}", message.data());
            return false;
        };
        return true;
    };
    static unsigned int vertexShader {};
    if (!vertexShader) {
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        if (!checkShaderState(vertexShader)) return;
    }
    static unsigned int fragmentShader {};
    if (!fragmentShader) {
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        if (!checkShaderState(fragmentShader)) return;
    }
    static unsigned int shaderProgram {};
    if (!shaderProgram) {
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if (!checkProgramState(shaderProgram)) return;
    };

    // Create and Bind Vertex Array Object
    // Note: Must be done before binding VBO, 'cause VAO captures and stores the vertex attribute configuration.
    static unsigned int vertexArrayObject {};
    if (!vertexArrayObject) {
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);
    }

    // Create and Bind Vertex Buffer Object
    static unsigned int vertexBufferObject {};
    if (!vertexBufferObject) {
        glGenBuffers(1, &vertexBufferObject);

        // Copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(sComponents.TriangleVertices), (void *)sComponents.TriangleVertices, GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(sComponents.RectangleVertices), (void *)sComponents.RectangleVertices, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sComponents.CubeVertices), (void *)sComponents.CubeVertices, GL_STATIC_DRAW);

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Color
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture Coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(7 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    // Create and Bind Element Buffer Object
    static unsigned int elementBufferObject {};
    if (!elementBufferObject) {
        glGenBuffers(1, &elementBufferObject);

        // Copy our indices array in a element buffer for OpenGL to use
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sComponents.TriangleIndices), (void *)sComponents.TriangleIndices, GL_STATIC_DRAW);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sComponents.RectangleIndices), (void *)sComponents.RectangleIndices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sComponents.CubeIndices), (void *)sComponents.CubeIndices, GL_STATIC_DRAW);
    }

    // Create and Bind Texture
    static unsigned int texture {};
    if (!texture) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // load and generate the texture
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load("Assets/Textures/Wallpaper2.png", &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            LogError("Failed to load texture!");
        }
        stbi_image_free(data);
    }

    // Get Uniforms
    static int vertexColorLocation = glGetUniformLocation(shaderProgram, "uColor");
    static int vertexTextureLocation = glGetUniformLocation(shaderProgram, "uTexture");
    static unsigned int transformLoc = glGetUniformLocation(shaderProgram, "uTransform");

    // Create Camera
    static FlyCamera flyCamera = FlyCamera(glm::vec3(0.0f, 0.0f, 3.0f));

    // Random Data
    static float cameraSpeed = 0.0005f;
    static float speed = 0.0f;
    speed += 0.0005f;
    if (speed > 360.0f) speed = 0.0f;
    static auto timeValue = 0.1f;
    timeValue += 0.0001f;
    float value = (sin(timeValue) / 2.0f) + 0.5f;

    // Update Camera
    if (Input::GetKeyState(KeyCode::LAlt)) {
        auto [x, y] = Input::GetMousePositionDelta();
        auto wheelDelta = Input::GetMouseWheelDelta();
        flyCamera.ProcessMouseMovement(x * 2.5f, y * 2.5f);
        flyCamera.ProcessMouseScroll(wheelDelta * 2.5f);
    }
    if (Input::GetKeyState(KeyCode::KeyW)) flyCamera.ProcessKeyboard(Camera_Movement::FORWARD, cameraSpeed);
    if (Input::GetKeyState(KeyCode::KeyS)) flyCamera.ProcessKeyboard(Camera_Movement::BACKWARD, cameraSpeed);
    if (Input::GetKeyState(KeyCode::KeyA)) flyCamera.ProcessKeyboard(Camera_Movement::LEFT, cameraSpeed);
    if (Input::GetKeyState(KeyCode::KeyD)) flyCamera.ProcessKeyboard(Camera_Movement::RIGHT, cameraSpeed);

    // Model, View, Projection
    //auto orthographic = glm::ortho(0.0f, 1280.0f, 0.0f, 1024.0f, 0.1f, 100.0f);
    auto model = glm::mat4(1.0f);
    auto view = glm::mat4(1.0f);
    auto projection = glm::perspective(glm::radians(flyCamera.Zoom), 1280.0f / 1024.0f, 0.1f, 100.0f);
    view = flyCamera.GetViewMatrix();
    //view = glm::translate(view, glm::vec3(-1.0f, 1.0f, -5.0f));
    //view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    //view = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetForwardDirection(), camera.GetUpDirection());

    // Scale, Rotation and Transformation
    auto translation = projection * view * model;
    //glm::mat4 translation = glm::mat4(1.0f);
    //translation = glm::translate(translation, glm::vec3(0.5f, -0.5f, 0.0f));
    //translation = glm::rotate(translation, glm::radians(speed), glm::vec3(0.0f, 0.0f, 1.0f));
    //translation = glm::scale(translation, glm::vec3(0.52f, 0.52f, 0.52f));
    //view = glm::scale(model, glm::vec3(1.0f, 1.0f, -1.0f)); // Flip Z-Axis (OpenGL is a right-handed system)

    // Update Uniforms;
    glUseProgram(shaderProgram);
    glUniform4f(vertexColorLocation, 0.1f, 1.0f - value, value, 1.0f);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(translation));

    // Draw the triangle
    //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vertexArrayObject);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe: On
    //glDrawElements(GL_TRIANGLES, sComponents.TriangleComponents, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, sComponents.RectangleComponents, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, sComponents.CubeComponents, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Wireframe: Off
    glBindVertexArray(0);

    // Optional: De-allocate all resources once they've outlived their purpose...
    //glDeleteVertexArrays(1, &vertexArrayObject);
    //glDeleteBuffers(1, &vertexBufferObject);
    //glDeleteProgram(shaderProgram);
}

#pragma endregion

}