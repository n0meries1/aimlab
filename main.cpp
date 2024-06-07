#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include <irrklang/irrKlang.h>
#include <ft2build.h>
#include <stb/stb_image.h>
#define STB_IMAGE_IMPLEMENTATION
#include FT_FREETYPE_H

#include "ResourceManager.h"
#include "Camera.h"
#include "Scoring.h"

void ProcessInput(GLFWwindow* window, float deltatime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
glm::vec3 ScreenToWorldRay(float mouseX, float mouseY);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void GenerateTarget();
unsigned int loadCubemap(std::vector<std::string> faces);

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;
float deltatime = 0.0f;
float lastFrame = 0.0f;


Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;

float sphereRadius = 0.25f;
glm::vec3 positionTarget = glm::vec3(0.0f, 0.0f, 0.0f);
int score = 0;

std::vector<std::string> faces
{
    "Resource/right.jpg",
    "Resource/left.jpg",
    "Resource/top.jpg",
    "Resource/bottom.jpg",
    "Resource/front.jpg",
    "Resource/back.jpg"
};

float skyboxVertices[] =
{
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f,
		 -1.0f, -1.0f, 1.0f,

		 -1.0f, 1.0f, -1.0f,
		 1.0f, 1.0f, -1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 -1.0f, 1.0f, 1.0f,
		 -1.0f, 1.0f, -1.0f,

		 -1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f
};

irrklang::ISoundEngine* Soundengine = irrklang::createIrrKlangDevice();


struct Character
{
	unsigned int TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;
};

std::map<GLchar, Character> Characters;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _APPLE_
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // _APPLE_
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
	SCR_WIDTH = static_cast <unsigned int> (mode->width);
	SCR_HEIGHT = static_cast <unsigned int> (mode->height);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Aimlabs", NULL, NULL);


	if (window == NULL)
	{
		std::cout << "Window Creation Failed" << std::endl;
		glfwTerminate();
		return -1;
	}

	/*glfwSetCursorPosCallback(window, mouse_callback);*/
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE:: Could not init FreeType Library" << std::endl;
		return -1;
	}

	std::string font_name = "font.ttf";
	if (font_name.empty())
	{
		std::cout << "ERRPR::FREETYPE: Failed to load font_name" << std::endl;
		return -1;
	}

	FT_Face face;
	if (FT_New_Face(ft, font_name.c_str(), 0, &face))
	{
		std::cout << "ERROR::FREETYPE:: Failed to load font" << std::endl;
		return -1;
	}
	else
	{
		FT_Set_Pixel_Sizes(face, 0, 48);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	

	glEnable(GL_DEPTH_TEST);

	unsigned int cubemapTexture = loadCubemap(faces);

	ResourceManager::LoadShader("Resource/vertexTarget.shader", "Resource/fragmentTarget.shader", nullptr, "sprite");
	Shader spriteShader = ResourceManager::GetShader("sprite");
	ResourceManager::LoadShader("Resource/vertexTarget.shader", "Resource/fragmentShaderRoom.shader", nullptr, "shaderRoom");
	Shader shaderRoom = ResourceManager::GetShader("shaderRoom");
	ResourceManager::LoadShader("Resource/vertexCrosshair.shader", "Resource/fragmentCrosshair.shader", nullptr, "crosshair");
	Shader crosshairShader = ResourceManager::GetShader("crosshair");
	ResourceManager::LoadShader("Resource/vertexScore.shader", "Resource/fragmentScore.shader", nullptr, "scoreShader");
	Shader scoreShader = ResourceManager::GetShader("scoreShader");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
	scoreShader.SetMatrix4("projection", projection);
	SpriteRenderer renderer(spriteShader, shaderRoom, crosshairShader, sphereRadius, static_cast<float> (SCR_WIDTH), static_cast <float> (SCR_HEIGHT));
	ResourceManager::LoadShader("Resource/vertexSkybox.shader", "Resource/fragmentSkybox.shader", nullptr, "skybox");
	Shader skybox = ResourceManager::GetShader("skybox");
	ResourceManager::LoadShader("Resource/vertexBox.shader", "Resource/fragmentBox.shader", nullptr, "boxShader");
	Shader boxShader = ResourceManager::GetShader("boxShader");

	Soundengine->play2D("Resource/background.mp3", true);

	float boxVertices[] = {
		// positions      // texture coords
		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  // top-left
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // bottom-left
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  // bottom-right
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f   // top-right
	};

	unsigned int boxIndices[] = {
		0, 1, 2,   // first triangle
		2, 3, 0    // second triangle
	};

	unsigned int boxVAO, boxVBO, boxEBO;
	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &boxVBO);
	glGenBuffers(1, &boxEBO);

	glBindVertexArray(boxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxIndices), boxIndices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	unsigned int scoreVAO, scoreVBO;
	glGenVertexArrays(1, &scoreVAO);
	glGenBuffers(1, &scoreVBO);
	glBindVertexArray(scoreVAO);
	glBindBuffer(GL_ARRAY_BUFFER, scoreVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	std::string text = "test";
	

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltatime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		ProcessInput(window, deltatime);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 view = camera.GetViewMatrix();
		ResourceManager::GetShader("sprite").Use().SetMatrix4("view", view);
		ResourceManager::GetShader("shaderRoom").Use().SetMatrix4("view", view);
		renderer.DrawSprite(positionTarget, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, glm::vec3(0.07f, 0.22, 1.00f));

		boxShader.Use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.5, 0.2, 0.2));
		model = glm::translate(model, glm::vec3(0.0, 4.5 , 0.0));
		boxShader.SetMatrix4("model", model);
		glBindVertexArray(boxVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		glDepthFunc(GL_LEQUAL);
		skybox.Use();
		view = glm::mat4(1.0f);
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast <float>(SCR_WIDTH) / static_cast <float> (SCR_HEIGHT), 0.1f, 100.0f);
		skybox.SetMatrix4("view", view);
		skybox.SetMatrix4("projection", projection);

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

	
		scoreShader.Use();
		scoreShader.SetVector3f("textColor", glm::vec3(1.0f, 1.0f, 1.0f));
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(scoreVAO);

		float x = 25.0f;
		float y = 25.0f;
		float scale = 1.0f;
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];

			float xpos = x + ch.Bearing.x * scale;
			float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;
			// Update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, scoreVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Advance cursors for next glyph
			x += (ch.Advance >> 6) * scale;
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}

void ProcessInput(GLFWwindow* window, float deltatime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltatime);

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	float aspectRatio = static_cast <float> (SCR_WIDTH) / static_cast <float> (SCR_HEIGHT);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);
	ResourceManager::GetShader("shaderRoom").Use().SetMatrix4("projection", projection);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float Xpos = static_cast <float>(xpos);
	float Ypos = static_cast <float>(ypos);

	if (firstMouse)
	{
		lastX = Xpos;
		lastY = Ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = Xpos;
	lastY = Ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		Soundengine->play2D("Resource/gunshot.mp3", false);
		float centerX = SCR_WIDTH / 2.0f;
		float centerY = SCR_HEIGHT / 2.0f;

		glm::vec3 rayDirection = ScreenToWorldRay(centerX, centerY);
		Ray ray = { camera.Position, rayDirection };
		Sphere sphere = { positionTarget, sphereRadius };

		
		bool result = RaySphereIntersection(ray, sphere);
		if (result)
		{
			GenerateTarget();
			Soundengine->play2D("Resource/pop.mp3", false);
		}
		
	}
}

glm::vec3 ScreenToWorldRay(float mouseX, float mouseY)
{
	float x = (2.0f * mouseX) / SCR_WIDTH - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / SCR_HEIGHT;
	float z = 1.0f;

	glm::vec3 rayNDC(x, y, -1.0f);
	glm::vec4 rayClip(rayNDC, 1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast <float> (SCR_WIDTH) / static_cast <float> (SCR_HEIGHT), 0.1f, 100.0f);
	glm::vec4 rayEye = glm::inverse(projection) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
	rayWorld = glm::normalize(rayWorld);
	return rayWorld;


	
}

void GenerateTarget()
{
	srand(time(0));
	glm::vec3 newpositiontarget;
	do
	{
	float x = (rand() % 150) - 75;
	float y = (rand() % 150) - 75;
	newpositiontarget = glm::vec3(x * 0.025, y * 0.025, 0);

	} while (glm::distance(newpositiontarget, positionTarget) < 0.025*50);
	positionTarget = newpositiontarget;
	score += 10;
	std::cout << "Score: " << score << std::endl;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed skybox generation" << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
	return textureID;
}

