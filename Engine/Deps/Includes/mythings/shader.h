#ifndef SHADER_H
#define SHADER_H
//这里，我们在头文件顶部使用了几个预处理指令(Preprocessor Directives)。
//这些预处理指令会告知你的编译器只在它没被包含过的情况下才包含和编译这个头文件，
//即使多个文件都包含了这个着色器头文件。它是用来防止链接冲突的。

#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//编写、编译、管理着色器是件麻烦事。
//在着色器主题的最后，我们会写一个类来让我们的生活轻松一点，
//它可以从硬盘读取着色器，然后编译并链接它们，并对它们进行错误检测，
//这就变得很好用了。这也会让你了解该如何封装目前所学的知识到一个抽象对象中。

class Shader
{
public:
	//着色器程序ID
	unsigned int ID;
	//默认构造空着色器
	Shader() { }
	//构造函数读取并构建着色器
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL);
	//编译空着色器
	void Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource = nullptr);
	//激活着色器程序函数
	Shader& use();
	//删除着色器
	void del();

	//uniform工具函数
	//version1
	void setBool(const std::string& name, bool value, bool useShader = false);
	void setInt(const std::string& name, int value, bool useShader = false);
	void setTex(const std::string& name, int value, bool useShader = false);
	void setFloat(const std::string& name, float value, bool useShader = false);

	void setVec2f(const std::string& name, float x, float y, bool useShader = false);
	void setVec2f(const std::string& name, const glm::vec2& value, bool useShader = false);
	void setVec2fv(const std::string& name, int NumOfVec2, glm::vec2& first_vec2, bool useShader = false);

	void setVec3f(const std::string& name, float v1, float v2, float v3, bool useShader = false);
	void setVec3f(const std::string& name, const glm::vec3& value, bool useShader = false);
	void setVec3fv(const std::string& name, int NumOfVec3, const glm::vec3& first_vec3, bool useShader = false);

	void setVec4f(const std::string& name, float v1, float v2, float v3, float v4, bool useShader = false);
	void setVec4f(const std::string& name, const glm::vec4& value, bool useShader = false);
	void setVec4fv(const std::string& name, int NumOfVec4, const glm::vec4& first_vec4, bool useShader = false);

	void setMat4fv(const std::string& name, GLsizei NumOfMat, glm::mat4& mat, GLboolean IfTranspose = GL_FALSE, bool useShader = false);
	void setMat4fv(const std::string& name, glm::mat4& mat, GLsizei NumOfMat, GLboolean IfTranspose = GL_FALSE, bool useShader = false);

	//version2
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec3fv(const std::string& name, int NumOfVec3, const glm::vec3& value) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w) const;
	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat, bool useShade = false);
	/*
	着色器类储存了着色器程序的ID。
	它的构造器需要顶点和片段着色器源代码的文件路径，这样我们就可以把源码的文本文件储存在硬盘上了。
	除此之外，为了让我们的生活更轻松一点，还加入了一些工具函数：
	use用来激活着色器程序，所有的set…函数能够查询一个unform的位置值并设置它的值。
	*/

private:
	void checkCompileErrors(unsigned int shader, std::string type);
};


//具体实现--------------------------------------------------------------------------------------------------------------------

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	//1.从文件路径中获取顶点/片段着色器源码文件
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	//保证ifstream正常抛出异常
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//清理ifstream对象
		vShaderFile.clear();
		fShaderFile.clear();
		//打开文件
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		//读取文件的缓冲内容到数据流
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//关闭文件处理器
		vShaderFile.close();
		fShaderFile.close();
		//转换数据到string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (geometryPath != NULL)
		{
			//清理ifstream对象
			gShaderFile.clear();
			//打开文件
			gShaderFile.open(geometryPath);
			//读取文件的缓冲内容到数据流
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			//关闭文件处理器
			gShaderFile.close();
			//转换数据到string
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	//转换为c风格字符串
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();

	//编译着色器
	unsigned int vertex, fragment, geometry = NULL;
	int success;
	char infoLog[512];

	//顶点着色器
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);//编译
	checkCompileErrors(vertex, "VERTEX");	//编译检查
	//片段着色器
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);//编译
	checkCompileErrors(fragment, "FRAGMENT");	//编译检查
	//几何着色器
	if (geometryPath != NULL)
	{
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);//编译
		checkCompileErrors(geometry, "GEOMETRY");	//编译检查
	}

	//创建着色器程序
	ID = glCreateProgram();
	//链接着色器
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryPath != NULL)
	{
		glAttachShader(ID, geometry);
	}
	glLinkProgram(ID);//链接
	checkCompileErrors(ID, "PROGRAM");	//链接检查

	//删除着色器(链接完就不需要了)
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}

void Shader::Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource)
{
	GLuint sVertex, sFragment, gShader;
	// Vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");
	// Fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");
	// If geometry shader source code is given, also compile geometry shader
	if (geometrySource != nullptr)
	{
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &geometrySource, NULL);
		glCompileShader(gShader);
		checkCompileErrors(gShader, "GEOMETRY");
	}
	// Shader Program
	this->ID = glCreateProgram();
	glAttachShader(this->ID, sVertex);
	glAttachShader(this->ID, sFragment);
	if (geometrySource != nullptr)
		glAttachShader(this->ID, gShader);
	glLinkProgram(this->ID);
	checkCompileErrors(this->ID, "PROGRAM");
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != nullptr)
		glDeleteShader(gShader);
}

Shader& Shader::use()
{
	glUseProgram(ID);
	return *this;
}
void Shader::del()
{
	glDeleteProgram(ID);
}

// Version1:
void Shader::setBool(const std::string& name, bool value, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform1i(location, (int)value);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setInt(const std::string& name, int value, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform1i(location, value);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setTex(const std::string& name, int value, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform1i(location, value);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setFloat(const std::string& name, float value, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform1f(location, value);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec2f(const std::string& name, float x, float y, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform2f(location, x, y);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec2f(const std::string& name, const glm::vec2& value, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform2fv(location, 1, &value[0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec2fv(const std::string& name, int NumOfVec2, glm::vec2& first_vec2, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform2fv(location, NumOfVec2, &first_vec2[0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec3f(const std::string& name, float v1, float v2, float v3, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform3f(location, v1, v2, v3);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec3f(const std::string& name, const glm::vec3& value, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform3fv(location, 1, &value[0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec3fv(const std::string& name, int NumOfVec3, const glm::vec3& first_vec3, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform3fv(location, NumOfVec3, &first_vec3[0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec4f(const std::string& name, float v1, float v2, float v3, float v4, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform4f(location, v1, v2, v3, v4);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec4f(const std::string& name, const glm::vec4& value, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform4fv(location, 1, &value[0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec4fv(const std::string& name, int NumOfVec4, const glm::vec4& first_vec4, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform4fv(location, NumOfVec4, &first_vec4[0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setMat4fv(const std::string& name, GLsizei NumOfMat, glm::mat4& mat, GLboolean IfTranspose, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniformMatrix4fv(location, NumOfMat, IfTranspose, glm::value_ptr(mat));
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setMat4fv(const std::string& name, glm::mat4& mat, GLsizei NumOfMat, GLboolean IfTranspose, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniformMatrix4fv(location, NumOfMat, IfTranspose, glm::value_ptr(mat));
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

// Version2:
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform2fv(location, 1, &value[0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform2f(location, x, y);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform3fv(location, 1, &value[0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform3f(location, x, y, z);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec3fv(const std::string& name, int NumOfVec3, const glm::vec3& value) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform3fv(location, NumOfVec3, &value[0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform4fv(location, 1, &value[0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniform4f(location, x, y, z, w);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat, bool useShader)
{
	if (useShader)
		this->use();

	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location != -1)
		glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
	else
		std::cerr << "Error: Uniform " << name << " not found in shader." << std::endl;
}

//编译检查函数
void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}
#endif