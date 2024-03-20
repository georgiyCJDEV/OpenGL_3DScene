// STD includes
#include <iostream>
#include <fstream>
#include <sstream>

// GLEW
#include "GL/glew.h"

// My includes
#include "GLDebug.h"
#include "Shader.h"

Shader::Shader(const std::string& filepath)
	:m_FilePath(filepath) 
{
	ShaderProgramSource source = parseShader(filepath); // �������� �������� ��� ��� ������� �� ����� 
	createShader(source.VertexSource, source.FragmentSource); // �������� �������
}

/* ��������� ��������� ���� ��� �������� �� ����� */
ShaderProgramSource Shader :: parseShader(const std::string& filepath)
{
	std::ifstream stream(m_FilePath); // ����� ��� ������ �� �����

	/* ��� ������� */
	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2]; // ����� ��� ������ �� ����� � ������
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n'; // ������ � ������ �� ���������������� ���� ������� �������
		}
	}

	return { ss[0].str(), ss[1].str() }; // ������
}

/* ����� ��� ���������� ������� */
unsigned int Shader:: compileShader(unsigned int type, // ��� �������
	const std::string& source // �������� ��� 
)
{
	GLCall(unsigned int id = glCreateShader(type)); // �������� ������� ������� ����
	const char* src = source.c_str(); // �������� ��� �������
	GLCall(glShaderSource(id, 1, &src, nullptr)); // �������� ��������� ����
	GLCall(glCompileShader(id)); // ���������� �������

	/* ��������� ������ */
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)_malloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
			" shader!" << std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id; // ������� ������� ���������� �������
}

/* �������� ���������-������� */
void Shader::createShader(const std::string& vertexShader, // �������� ��� ����������� �������
	const std::string& fragmentShader // �������� ��� ������������ �������
)
{
	GLCall(m_ProgramId = glCreateProgram()); // �������� ���������-�������
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader); // ���������� ����������� �������
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader); // ���������� ������������ �������

	GLCall(glAttachShader(m_ProgramId, vs)); // ������������ ����������� ������� � ���������-�������
	GLCall(glAttachShader(m_ProgramId, fs)); // ������������ ������������ ������� � ���������-�������

	GLCall(glLinkProgram(m_ProgramId));     // ���������� ���������
	GLCall(glValidateProgram(m_ProgramId)); // ������������� ������������� ���������
	
	GLCall(glDetachShader(m_ProgramId, vs));
	GLCall(glDetachShader(m_ProgramId, fs));

	GLCall(glDeleteShader(vs)); // �������� ����������� �������
	GLCall(glDeleteShader(fs)); // �������� ������������ �������
}

void Shader::bind()
{
	GLCall(glUseProgram(m_ProgramId));
}

void Shader::setUniform1i(const std::string& name, int value)
{
	GLCall(glProgramUniform1i(m_ProgramId, getUniformLocation(name), value));
}

void Shader::setUniform1f(const std::string& name, float value)
{
	GLCall(glProgramUniform1f(m_ProgramId, getUniformLocation(name), value));
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2)
{
	GLCall(glProgramUniform3f(m_ProgramId, getUniformLocation(name), v0, v1, v2));
}


void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glProgramUniform4f(m_ProgramId, getUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glProgramUniformMatrix4fv(m_ProgramId, getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::getUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}

	GLCall(int location = glGetUniformLocation(m_ProgramId, name.c_str()));
	if (location == -1)
	{
		std::cout << "Warning: uniform '" << name << "' doesn't exist! " << std::endl;
	}
	m_UniformLocationCache[name] = location;

	return location;
}
