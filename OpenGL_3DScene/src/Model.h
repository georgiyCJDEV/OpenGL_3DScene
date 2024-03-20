#pragma once

// STD includes
#include <string>
#include <vector>

// Assimp
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

// My includes
#include "Mesh.h"
#include "Shader.h"

class Model
{
private:
	const aiScene* scene;

	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

	int loadTextureFromFile(const char* path, std::string directory);

	void loadModel(std::string path);

	void processNode(aiNode* node);
	void processMesh(aiNode* node);

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

public:
	Model(const char* path);

	void draw(Shader* shader,
		bool& noTextureFlag,
		glm::mat4&, glm::mat4&, glm::mat4&,
		const float& vPosX, const float& vPosY, const float& vPosZ,		// Camera positions
		const glm::vec3* pointLightPoisitions,							// Point light positions
		const float& frPosX, const float& frPOsY, const float& frPosZ	// Camera front positions);
	);
	void draw(Shader& shader,
		glm::mat4&,glm::mat4&,glm::mat4&,
		const float& vPosX, const float& vPosY, const float& vPosZ,		// Camera positions
		const glm::vec3* pointLightPoisitions,							// Point light positions
		const float& frPosX, const float& frPOsY, const float& frPosZ	// Camera front positions);
	);
};