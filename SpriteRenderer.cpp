#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(Shader& shader, Shader& shaderRoom, Shader& shaderCrosshair, float sphereRadius, float SCR_WIDTH, float SCR_HEIGHT)
    : shader(shader), shaderRoom(shaderRoom), crosshairShader(shaderCrosshair), sphereRadius(sphereRadius), SCR_WIDTH(SCR_WIDTH), SCR_HEIGHT(SCR_HEIGHT) {
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &this->quadVAO);
    glDeleteVertexArrays(1, &this->floorVAO);
    glDeleteVertexArrays(1, &this->crosshairVAO);
}

void SpriteRenderer::DrawSprite(glm::vec3 position, glm::vec3 size, float rotate, glm::vec3 color) {
    // Render the sprite
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, size);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
    this->shader.SetMatrix4("projection", projection);
    this->shader.SetMatrix4("model", model);
    this->shader.SetVector3f("spriteColor", color);


    glBindVertexArray(this->quadVAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    this->shaderRoom.Use();
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.15f));
    model = glm::translate(model, glm::vec3(0.0f, -20.0f, 50.0f));
    this->shaderRoom.SetMatrix4("model", model);
    this->shaderRoom.SetMatrix4("projection", projection);
    this->shaderRoom.SetVector3f("spriteColor", glm::vec3(0.5f, 0.5f, 0.5f));
    glBindVertexArray(this->floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.0f));
    model = glm::translate(model, glm::vec3(glm::vec3(0.0f, 25.0f, -35.0f)));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    this->shaderRoom.SetMatrix4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    this->crosshairShader.Use();
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.15f, 0.25f, 0.25f));
    this->crosshairShader.SetMatrix4("model", model);
    glBindVertexArray(this->crosshairVAO);
    glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

void SpriteRenderer::initRenderData() {
    unsigned int VBO, EBO;
    generateSphere(sphereRadius, 30, 30, vertices, indices);
    indicesCount = indices.size();

    // Setup sprite (quad) VAO
    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(this->quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    float floorVertices[] = {
        -50.0f, 0.0f, -50.0f, 0.0f, 0.0f,
         50.0f, 0.0f, -50.0f, 1.0f, 0.0f,
         50.0f, 0.0f,  50.0f, 1.0f, 1.0f,
        -50.0f, 0.0f,  50.0f, 0.0f, 1.0f
    };
    unsigned int floorIndices[] = { 0, 1, 2, 2, 3, 0 };

    unsigned int floorVBO, floorEBO;
    glGenVertexArrays(1, &this->floorVAO);
    glGenBuffers(1, &floorVBO);
    glGenBuffers(1, &floorEBO);

    glBindVertexArray(this->floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), &floorIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    float crosshairVertices[] = {
        -0.05f,  0.0f, 0.0f,
         0.05f,  0.0f, 0.0f,
         0.0f, -0.05f, 0.0f,
         0.0f,  0.05f, 0.0f
    };

    unsigned int crosshairIndices[] = {
        0, 1,
        2, 3
    };

    unsigned int crosshairVBO, crosshairEBO;
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glGenBuffers(1, &crosshairEBO);
    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), &crosshairVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crosshairEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(crosshairIndices), &crosshairIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

}

void SpriteRenderer::generateSphere(float radius, int latitudeBands, int longitudeBands, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    vertices.clear();
    indices.clear();

    for (int latNumber = 0; latNumber <= latitudeBands; ++latNumber) {
        float theta = latNumber * glm::pi<float>() / latitudeBands;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int longNumber = 0; longNumber <= longitudeBands; ++longNumber) {
            float phi = longNumber * 2 * glm::pi<float>() / longitudeBands;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;
            float u = 1 - (float(longNumber) / longitudeBands);
            float v = 1 - (float(latNumber) / latitudeBands);

            vertices.push_back(radius * x);
            vertices.push_back(radius * y);
            vertices.push_back(radius * z);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (int latNumber = 0; latNumber < latitudeBands; ++latNumber) {
        for (int longNumber = 0; longNumber < longitudeBands; ++longNumber) {
            int first = (latNumber * (longitudeBands + 1)) + longNumber;
            int second = first + longitudeBands + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}