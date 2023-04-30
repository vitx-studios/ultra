﻿module;

#include <glm/glm.hpp>

export module Ultra.Engine.Renderer.Shader;

export import Ultra.Core;
export import Ultra.Logger;
import Ultra.Engine.Renderer.Data;

export namespace Ultra {

///
/// @brief Shader Data and Uniform Types
///
enum class ShaderDataType: uint32_t {
	Null    = 0x00u,

	Bool    = 0x11u,
    Bool2   = 0x12u,
    Bool3   = 0x13u,
    Bool4   = 0x14u,

	Float   = 0x21u,
	Float2  = 0x22u,
	Float3  = 0x23u,
	Float4  = 0x24u,

	Int     = 0x31u,
	Int2    = 0x32u,
	Int3    = 0x33u,
	Int4    = 0x34u,

    Mat2    = 0x42u,
	Mat3    = 0x43u,
	Mat4    = 0x44u,
};

uint32_t ShaderDataTypeSize(ShaderDataType type) {
	switch (type) {
		case ShaderDataType::Bool:	    return 1u;
        case ShaderDataType::Bool2:     return 1u * 2u;
        case ShaderDataType::Bool3:     return 1u * 3u;
        case ShaderDataType::Bool4:     return 1u * 4u;
		case ShaderDataType::Float:		return 4u;
		case ShaderDataType::Float2:	return 4u * 2u;
		case ShaderDataType::Float3:	return 4u * 3u;
		case ShaderDataType::Float4:    return 4u * 4u;
		case ShaderDataType::Int:	    return 4u;
		case ShaderDataType::Int2:	    return 4u * 2u;
		case ShaderDataType::Int3:	    return 4u * 3u;
		case ShaderDataType::Int4:	    return 4u * 4u;
        case ShaderDataType::Mat2:	    return 4u * 3u * 3u;
		case ShaderDataType::Mat3:	    return 4u * 3u * 3u;
		case ShaderDataType::Mat4:	    return 4u * 4u * 4u;
		default:						return 0u;
	}
}

// Shader Uniform Types (specific)
enum class ShaderUniformType {
    // Scalars
    Null    = 0x00,
    Bool    = 0x01,
    Int     = 0x02,
    UInt    = 0x03,
    Float   = 0x04,
    Double  = 0x05,

    // Vectors
    BVec    = 0x11,
    BVec2   = 0x12,
    BVec3   = 0x13,
    BVec4   = 0x14,
    IVec    = 0x21,
    IVec2   = 0x22,
    IVec3   = 0x23,
    IVec4   = 0x24,
    UVec    = 0x31,
    UVec2   = 0x32,
    UVec3   = 0x33,
    UVec4   = 0x34,
    Vec     = 0x41,
    Vec2    = 0x42,
    Vec3    = 0x43,
    Vec4    = 0x44,
    DVec    = 0x51,
    DVec2   = 0x52,
    DVec3   = 0x53,
    DVec4   = 0x54,

    // Matrices
    Mat2    = 0x27,
    Mat3    = 0x28,
    Mat4    = 0x29,
};


///
/// @brief Shader Languages
/// C-like languages which are used to write shader programs for various stages of the graphics pipeline.
///
enum class ShaderLanguage {
    GLSL,   // "OpenGL Shading Language"
    HLSL,   // "High-Level Shading Language"
};


///
/// @brief Shader Types
///
enum class ShaderType {
    Linked,         // Combination of vertex and fragment shaders
    Compute,        // Shader that performs general-purpose calculations on the GPU
    Fragment,       // Shader that processes pixel or fragment data
    Geometry,       // Shader that processes primitive geometry
    TessControl,    // Shader that controls the tessellation of geometric primitives
    TessEvaluation, // Shader that evaluates the tessellation of geometric primitives
    Vertex,         // Shader that processes vertices and vertex data
};

size_t ShaderTypeFromString(const string &type) {
    if (type == "compute")      return (size_t)ShaderType::Compute;
    if (type == "fragment")     return (size_t)ShaderType::Fragment;
    if (type == "geometry")     return (size_t)ShaderType::Geometry;
    if (type == "control")      return (size_t)ShaderType::TessControl;
    if (type == "evaluation")   return (size_t)ShaderType::TessEvaluation;
    if (type == "vertex")       return (size_t)ShaderType::Vertex;
    return (size_t)ShaderType::Linked;
}


/// 
/// @brief Agnostic Shader
///
/// @example: Separate
/// auto vertexShader = Shader::Create("source code", ShaderType::Vertex);
/// auto fragmentShader = Shader::Create("source code", ShaderType::Fragment);
/// 
/// @example: Linked
/// auto linkedShaders = Shader::Create("example.glsl");
/// 
class Shader {
protected:
    using ShaderList = unordered_map<size_t, string>;

    Shader(const string &source, const string &entryPoint, const ShaderType type): mSource(source), mEntryPoint(entryPoint), mType(type) {}

public:
    Shader() = default;
    virtual ~Shader() = default;

    static Scope<Shader> Create(const string &source, const ShaderType type = ShaderType::Linked);
    static Scope<Shader> Create(const string &source, const string &entryPoint, const ShaderType type = ShaderType::Linked);

    virtual void Compile(ShaderList shaders) = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    // Accessors
    virtual int32_t GetUniformLocation(const string &name) const = 0;

    // Mutators
    virtual void SetUniformBuffer(const string &name, const void *data, size_t size) = 0;
    virtual void SetUniform(const string &name, bool data) = 0;
    virtual void SetUniform(const string &name, float data) = 0;
    virtual void SetUniform(const string &name, glm::vec2 &data) = 0;
    virtual void SetUniform(const string &name, glm::vec3 &data) = 0;
    virtual void SetUniform(const string &name, glm::vec4 &data) = 0;
    virtual void SetUniform(const string &name, int data) = 0;
    virtual void SetUniform(const string &name, glm::ivec2 &data) = 0;
    virtual void SetUniform(const string &name, glm::ivec3 &data) = 0;
    virtual void SetUniform(const string &name, glm::ivec4 &data) = 0;
    virtual void SetUniform(const string &name, unsigned int data) = 0;
    virtual void SetUniform(const string &name, glm::uvec2 &data) = 0;
    virtual void SetUniform(const string &name, glm::uvec3 &data) = 0;
    virtual void SetUniform(const string &name, glm::uvec4 &data) = 0;
    virtual void SetUniform(const string &name, glm::mat2 &data) = 0;
    virtual void SetUniform(const string &name, glm::mat3 &data) = 0;
    virtual void SetUniform(const string &name, glm::mat4 &data) = 0;

protected:
    ShaderList Convert(string &source);

    RendererID mShaderID;
    string mShaderName;
    string mEntryPoint;
    string mSource;
    ShaderType mType;

    ShaderList mShaders;
};

}