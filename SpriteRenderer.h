#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Shader.h"
#include "Texture.h"

class SpriteRenderer {
public:
    SpriteRenderer(Shader& shader, Shader& shaderRoom, Shader& shaderCrosshair, float sphereRadius, float SCR_WIDTH, float SCR_HEIGHT);
    ~SpriteRenderer();

    void DrawSprite(glm::vec3 position, glm::vec3 size, float rotate, glm::vec3 color);

private:
    Shader shader;
    Shader shaderRoom;
    Shader crosshairShader;
    float sphereRadius;
    float SCR_WIDTH;
    float SCR_HEIGHT;
    unsigned int quadVAO, floorVAO, crosshairVAO, scoreVAO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    size_t indicesCount;

    void initRenderData();
    void generateSphere(float radius, int latitudeBands, int longitudeBands, std::vector<float>& vertices, std::vector<unsigned int>& indices);
};

#endif