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
	ShaderProgramSource source = parseShader(filepath); // Получаем исходный код для шейдера из файла 
	createShader(source.VertexSource, source.FragmentSource); // Создание шейдера
}

/* Получение исходного кода для шейдеров из файла */
ShaderProgramSource Shader :: parseShader(const std::string& filepath)
{
	std::ifstream stream(m_FilePath); // Поток для чтения из файла

	/* Тип шейдера */
	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2]; // Поток для записи из файла в строку
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
			ss[(int)type] << line << '\n'; // Запись в строку по соответствующему типу шейдера индексу
		}
	}

	return { ss[0].str(), ss[1].str() }; // Кортеж
}

/* Метод для компиляции шейдера */
unsigned int Shader:: compileShader(unsigned int type, // Тип шейдера
	const std::string& source // Исходный код 
)
{
	GLCall(unsigned int id = glCreateShader(type)); // Создание шейдера нужного типа
	const char* src = source.c_str(); // Исходный код шейдера
	GLCall(glShaderSource(id, 1, &src, nullptr)); // Передача исходного кода
	GLCall(glCompileShader(id)); // Компиляция шейдера

	/* Обработка ошибок */
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

	return id; // Возврат индекса созданного шейдера
}

/* Создание программы-шейдера */
void Shader::createShader(const std::string& vertexShader, // Исходный код вертексного шейдера
	const std::string& fragmentShader // Исходный код фрагмнетного шейдера
)
{
	GLCall(m_ProgramId = glCreateProgram()); // Создание программы-шейдера
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader); // Компиляция вертексного шейдера
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader); // Компиляция фрагментного шейдера

	GLCall(glAttachShader(m_ProgramId, vs)); // Прикрепление вертексного шейдера к программе-шейдеру
	GLCall(glAttachShader(m_ProgramId, fs)); // Прикрепление фрагментного шейдера к программе-шейдеру

	GLCall(glLinkProgram(m_ProgramId));     // Компановка программы
	GLCall(glValidateProgram(m_ProgramId)); // Подтверждение исполняемости программы
	
	GLCall(glDetachShader(m_ProgramId, vs));
	GLCall(glDetachShader(m_ProgramId, fs));

	GLCall(glDeleteShader(vs)); // Удаление вертексного шейдера
	GLCall(glDeleteShader(fs)); // Удаление фрагмнетного шейдера
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
