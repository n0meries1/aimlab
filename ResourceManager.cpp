#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;

// Load and store a shader program
Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, const std::string& name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

// Retrieve a stored shader
Shader ResourceManager::GetShader(const std::string& name)
{
    return Shaders[name];
}

// Load and store a texture
Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, const std::string& name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

// Retrieve a stored texture
Texture2D ResourceManager::GetTexture(const std::string& name)
{
    return Textures[name];
}

// Clear all loaded resources
void ResourceManager::Clear()
{
    for (auto& iter : Shaders)
        glDeleteProgram(iter.second.ID);
    for (auto& iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
    Shaders.clear();
    Textures.clear();
}

// Load shader from file
Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
    std::string vertexCode, fragmentCode, geometryCode;

    try
    {
        // Read vertex shader code
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        if (!vertexShaderFile || !fragmentShaderFile)
            throw std::runtime_error("Could not open shader files");

        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        vertexShaderFile.close();
        fragmentShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // If geometry shader path is present, read geometry shader code
        if (gShaderFile)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            if (geometryShaderFile)
            {
                std::stringstream gShaderStream;
                gShaderStream << geometryShaderFile.rdbuf();
                geometryShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR::SHADER: Failed to read shader files\n" << e.what() << std::endl;
        throw;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = gShaderFile ? geometryCode.c_str() : nullptr;

    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderCode ? gShaderCode : nullptr);
    return shader;
}

// Load texture from file
Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }

    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "ERROR::TEXTURE: Failed to load texture file " << file << std::endl;
        throw std::runtime_error("Failed to load texture");
    }

    texture.Generate(width, height, data);
    stbi_image_free(data);
    return texture;
}



