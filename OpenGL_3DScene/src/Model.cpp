// STD includes
#include <fstream>
#include <iostream>
#include <sstream>

// GLEW
#include "GL/glew.h"

// SOIL2
#include "SOIL2/SOIL2.h"

// Assimp
#include "assimp/postprocess.h"

// My includes
#include "GLDebug.h"
#include "Model.h"

Model::Model(const char* path)
{
	loadModel(path);
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	scene = importer.ReadFile(path, aiProcess_Triangulate |
		aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->
		mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;

		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode);
}

void Model::processNode(aiNode* node)
{
	if (node->mNumChildren == 0)
	{
		processMesh(node);
	}
	else
	{
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			this->processNode(node->mChildren[i]);
		}
	}
}

void Model::processMesh(aiNode *node)
{
	std::vector <Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Vertex vertex;

		aiColor3D ambient, diffuse, specular;
		float opacity=1.0f;
		if (scene->HasMaterials())
		{
			aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];
			mtl->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			mtl->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			mtl->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			mtl->Get(AI_MATKEY_OPACITY, opacity);
		}
		// Vertices loading (positions, normals, texture coords)
		for (unsigned int j = 0; j < mesh->mNumVertices; j++)
		{
			vertex.position = glm::vec3(mesh->mVertices[j].x,
				mesh->mVertices[j].y,
				mesh->mVertices[j].z);


			if (mesh->mNormals)
			{
				vertex.normal = glm::vec3(mesh->mNormals[j].x,
					mesh->mNormals[j].y,
					mesh->mNormals[j].z);
			}
			else
			{
				vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
			}

			if (mesh->mTextureCoords[0])
			{
				vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][j].x,
					mesh->mTextureCoords[0][j].y);
			}
			else
			{
				vertex.texCoords = glm::vec2(1.0f, 1.0f);
			}

			vertex.colorAmbient = glm::vec3(ambient.r, ambient.g, ambient.b);
			vertex.colorDiffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
			vertex.colorSpecular = glm::vec3(specular.r, specular.g, specular.b);
			vertex.opacity = opacity;

			vertices.push_back(vertex);
		}

		// Indices (Model's faces) loading
		for (unsigned int k = 0; k < mesh->mNumFaces; k++)
		{
			indices.push_back(mesh->mFaces[k].mIndices[0]);
			indices.push_back(mesh->mFaces[k].mIndices[1]);
			indices.push_back(mesh->mFaces[k].mIndices[2]);
		}
		
		// Textures loading
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<Texture> diffuseMaps = loadMaterialTextures(aiMat,
				aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Texture> specularMaps = loadMaterialTextures(aiMat,
				aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}
		meshes.push_back(Mesh(vertices,indices,textures));
	}
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;

		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j].path == str)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one.

				break;
			}
		}

		// If texture hasn't been loaded already, load it
		if (!skip)
		{   
			Texture texture;
			texture.id = loadTextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);

			this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}

	return textures;
}

int Model::loadTextureFromFile(const char* path, std::string directory)
{
	// Texture path
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	// Generate texture id
	unsigned int textureID;
	GLCall(glGenTextures(1, &textureID));

	int width, height;

	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	// Bind texture
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));

	// Create texture
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));

	// Parameters
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	// Unbind texture
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	SOIL_free_image_data(image);

	return textureID;
}

void Model::draw(Shader* shadersArr,
	bool& noTextureFlag,
	glm::mat4& projection, glm::mat4& view, glm::mat4& model,
	Camera& camera,
	const glm::vec3* pointLightPoisitions)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		if (meshes[i].getTextures().size() == 0)
		{
			meshes[i].draw(shadersArr[1],
				noTextureFlag,
				projection, view, model,
				camera,
				pointLightPoisitions);
		}
		else 
		{
			meshes[i].draw(shadersArr[0],
				projection, view, model,
				camera,
				pointLightPoisitions);
		}
	}
}

/*If model only has textures*/
void Model::draw(Shader& shader,
	glm::mat4& projection, glm::mat4& view, glm::mat4& model,
	Camera& camera,		
	const glm::vec3* pointLightPoisitions)					
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
			meshes[i].draw(shader,
				projection, view, model,
				camera,
				pointLightPoisitions);
	}
}
