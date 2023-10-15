﻿export module Ultra.Asset;

export import Ultra.Asset.Resource;

import Ultra.Core;
import Ultra.Math.Vector;

import <glm/glm.hpp>;
import <glm/gtc/matrix_transform.hpp>;
import <glm/gtc/type_ptr.hpp>;

export namespace Ultra {

enum class AssetType {
    Font,
    Image,
    Model,
    Script,
    Shader,
    Sound,
    Unknown,
};

string to_string(const AssetType &type) {
    switch (type) {
        case AssetType::Font:       return "Font";
        case AssetType::Image:      return "Image";
        case AssetType::Model:      return "Model";
        case AssetType::Script:     return "Script";
        case AssetType::Shader:     return "Shader";
        case AssetType::Sound:      return "Sound";
        case AssetType::Unknown:    return "Unknown";
        default:                    return "Invalid";
    }
}

struct Asset {
    string Path {};
    AssetType Type { AssetType::Unknown };
};


//enum class TextureType {
//    Diffuse,
//    Specular,
//    Normal,
//    Height,
//    Ambient,
//    Emissive,
//    Opacity,
//    Shininess,
//    Displacement,
//    Lightmap,
//    Reflection,
//    Roughness,
//    Metallic,
//    Occlusion,
//    Unknown,
//};


namespace Colors {
    // ToDo: Switch to Vector class
    //Vector3Df Black { 0.0f, 0.0f, 0.0f };
    //Vector3Df Blue  { 0.0f, 0.0f, 1.0f };
    //Vector3Df Coral { 1.0f, 0.5f, 0.31f };
    //Vector3Df Green { 0.0f, 1.0f, 0.0f };
    //Vector3Df Red   { 1.0f, 0.0f, 0.0f };
    //Vector3Df White { 1.0f, 1.0f, 1.0f };

    glm::vec3 Black { 0.0f, 0.0f, 0.0f };
    glm::vec3 Blue  { 0.0f, 0.0f, 1.0f };
    glm::vec3 Coral { 1.0f, 0.5f, 0.31f };
    glm::vec3 Green { 0.0f, 1.0f, 0.0f };
    glm::vec3 Red   { 1.0f, 0.0f, 0.0f };
    glm::vec3 White { 1.0f, 1.0f, 1.0f };
}

struct Light {
    glm::vec3 Color;
    glm::vec3 AmbientIntensity;
    glm::vec3 DiffuseIntensity;
    glm::vec3 SpecularIntensity;

    glm::vec3 Direction;
    glm::vec3 Position;
    float Intensity;
};

}