#pragma once

// STD includes
#include <vector>
#include <iostream>

// Assimp
#include "assimp/scene.h"

// My includes
#include "Shader.h"
#include "Camera.h"

struct Vertex
{
	// Position
	glm::vec3 position;
	// Normal
	glm::vec3 normal;
	// TexCoords
	glm::vec2 texCoords;
	// Colors
	glm::vec3 colorAmbient;
	glm::vec3 colorDiffuse;
	glm::vec3 colorSpecular;

	float opacity;
};

struct Texture
{
	unsigned int id;
	std::string type;
	aiString path;
};

class Mesh
{
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//unsigned int vbo[4];
	unsigned int vbo, vao, ibo;

	void setupMesh();

public:
	// Constructor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	// Render
	void draw(Shader& shader,
		glm::mat4&, glm::mat4&, glm::mat4&,
		Camera& camera,
		const glm::vec3* pointLightPoisitions);
	void draw(Shader& shader,
		bool& noTextureFlag,
		glm::mat4&, glm::mat4&, glm::mat4&,
		Camera& camera,
		const glm::vec3* pointLightPoisitions);

	inline std::vector<Texture> getTextures() const { return textures; }
};

