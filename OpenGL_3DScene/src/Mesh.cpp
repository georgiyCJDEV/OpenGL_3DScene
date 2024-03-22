// STD includes
#include <string>
#include <fstream>
#include <sstream>
#include <format>

// GLEW
#include <GL/glew.h>

// My includes
#include "Mesh.h"
#include "GLDebug.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::setupMesh()
{
	// Create buffers/arrays
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glGenBuffers(1, &vbo));
	GLCall(glGenBuffers(1, &ibo));

	// Load data into vertex buffers
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));

	// Set the vertex attribute pointers
	// Vertex Positions
	GLCall(glBindVertexArray(vao));
	
	// Vertex positions
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
	// Vertex normals
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)));
	// Vertex texture coords
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords)));
	// Vertex ambient color
	GLCall(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colorAmbient)));
	// Vertex diffuse color
	GLCall(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colorDiffuse)));
	// vertex specular color
	GLCall(glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colorSpecular)));
	// vertex opacity
	GLCall(glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, opacity)));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glEnableVertexAttribArray(3));
	GLCall(glEnableVertexAttribArray(4));
	GLCall(glEnableVertexAttribArray(5));
	GLCall(glEnableVertexAttribArray(6));

	// Unbind vertex buffer
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	// Unbind vertex array
	GLCall(glBindVertexArray(0));

	// Load data into index buffers
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Mesh::draw(Shader& shader,
	glm::mat4& projection, glm::mat4& view, glm::mat4& model,
	Camera& camera,
	const glm::vec3* pointLightPoisitions)
{
	shader.setUniformMat4f("u_Projection", projection);
	shader.setUniformMat4f("u_View", view);
	shader.setUniformMat4f("u_Model", model);

	// Texture setup
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		GLCall(glActiveTexture(GL_TEXTURE0 + i)); // Activate proper texture unit before binding

		// Retrieve texture number (the N in diffuse_textureN)
		std::string uName = textures[i].type;

		if (textures[i].type == "texture_diffuse")
		{
			uName = "u_Material.texDiffuse";
		}
		else if (textures[i].type == "texture_specular")
		{
			uName = "u_Material.texSpecular";
		}

		// Set the sampler to the correct texture unit
		shader.setUniform1i(uName, i);

		// Bind texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	
	// View position
	shader.setUniform3f("u_ViewPos", camera.getPosition().x,camera.getPosition().y,camera.getPosition().z); // Camera positions

	// Material setup
	shader.setUniform1f("u_Material.shininess", 16.0f);

	// Point Light
	for (int i = 0; i < 11; i++)
	{
		std::string pointUniformLoc = std::format("u_PointLight[{}]", i);

		shader.setUniform3f(pointUniformLoc + ".position", pointLightPoisitions[i].x, pointLightPoisitions[i].y, pointLightPoisitions[i].z);	// Point light positions

		shader.setUniform3f(pointUniformLoc + ".ambient", 0.5f, 0.5f, 0.5f);
		shader.setUniform3f(pointUniformLoc + ".diffuse", 0.8f, 0.8f, 0.8f);
		shader.setUniform3f(pointUniformLoc + ".specular", 1.0f, 1.0f, 1.0f);

		shader.setUniform1f(pointUniformLoc + ".constant", 1.0f);
		shader.setUniform1f(pointUniformLoc + ".linear", 0.09f);
		shader.setUniform1f(pointUniformLoc + ".quadratic", 0.057f);
	}

	// Bind vertex array and index buffer objects
	GLCall(glBindVertexArray(vao));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

	shader.bind();

	// Draw mesh
	GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));

	// Unbind vertex array and index buffer objects
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	
	// Unbind textures
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		GLCall(glActiveTexture(GL_TEXTURE0 + i));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}
}

void Mesh::draw(Shader& shader,
	bool& noTextureFlag,
	glm::mat4& projection, glm::mat4& view, glm::mat4& model,
	Camera&camera,
	const glm::vec3* pointLightPoisitions)
{
	if (!noTextureFlag)
	{
		return;
	}
	shader.setUniformMat4f("u_Projection", projection);
	shader.setUniformMat4f("u_View", view);
	shader.setUniformMat4f("u_Model", model);

	// View position
	shader.setUniform3f("u_ViewPos", camera.getPosition().x,camera.getPosition().y,camera.getPosition().z); // Camera positions

	// Material setup
	shader.setUniform1f("u_Material.shininess", 16.0f);

	// Point Light
	for (int i = 0; i < 11; i++)
	{
		std::string pointUniformLoc = std::format("u_PointLight[{}]", i);

		shader.setUniform3f(pointUniformLoc + ".position", pointLightPoisitions[i].x, pointLightPoisitions[i].y, pointLightPoisitions[i].z);	// Point light positions

		shader.setUniform3f(pointUniformLoc + ".ambient", 0.5f, 0.5f, 0.5f);
		shader.setUniform3f(pointUniformLoc + ".diffuse", 0.8f, 0.8f, 0.8f);
		shader.setUniform3f(pointUniformLoc + ".specular", 1.0f, 1.0f, 1.0f);

		shader.setUniform1f(pointUniformLoc + ".constant", 1.0f);
		shader.setUniform1f(pointUniformLoc + ".linear", 0.09f);
		shader.setUniform1f(pointUniformLoc + ".quadratic", 0.057f);
	}

	// Bind vertex array and index buffer objects
	GLCall(glBindVertexArray(vao));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

	// Bind shader if it's not bound
	shader.bind();

	// Draw mesh
	GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));

	// Unbind vertex array and index buffer objects
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}