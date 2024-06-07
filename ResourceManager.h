#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>
#include <sstream>

#include <glad/glad.h>

#include "texture.h"
#include "shader.h"
#include "SpriteRenderer.h"

class ResourceManager
{
public:
    // Resource storage
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture2D> Textures;

    // Loads and generates a shader program from file
    static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, const std::string& name);
    // Retrieves a stored shader
    static Shader GetShader(const std::string& name);

    // Loads and generates a texture from file
    static Texture2D LoadTexture(const char* file, bool alpha, const std::string& name);
    // Retrieves a stored texture
    static Texture2D GetTexture(const std::string& name);

    // Properly de-allocates all loaded resources
    static void Clear();

private:
    // Private constructor, to prevent instantiation. Static class only
    ResourceManager() { }

    // Loads a single shader from file
    static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
    // Loads a single texture from file
    static Texture2D loadTextureFromFile(const char* file, bool alpha);
};



#endif