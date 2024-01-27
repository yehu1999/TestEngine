#ifndef SHADER_H
#define SHADER_H
//���������ͷ�ļ�����ʹ���˼���Ԥ����ָ��(Preprocessor Directives)��
//��ЩԤ����ָ����֪��ı�����ֻ����û��������������²Ű����ͱ������ͷ�ļ���
//��ʹ����ļ��������������ɫ��ͷ�ļ�������������ֹ���ӳ�ͻ�ġ�

#include <glad/glad.h>; // ����glad����ȡ���еı���OpenGLͷ�ļ�
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//��д�����롢������ɫ���Ǽ��鷳�¡�
//����ɫ�������������ǻ�дһ�����������ǵ���������һ�㣬
//�����Դ�Ӳ�̶�ȡ��ɫ����Ȼ����벢�������ǣ��������ǽ��д����⣬
//��ͱ�úܺ����ˡ���Ҳ�������˽����η�װĿǰ��ѧ��֪ʶ��һ����������С�

class Shader
{
public:
	//��ɫ������ID
	unsigned int ID;
	//Ĭ�Ϲ������ɫ��
	Shader() { }
	//���캯����ȡ��������ɫ��
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL);
	//�������ɫ��
	void Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource = nullptr);
	//������ɫ��������
	Shader& use();
	//ɾ����ɫ��
	void del();

	//uniform���ߺ���
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
	��ɫ���ഢ������ɫ�������ID��
	���Ĺ�������Ҫ�����Ƭ����ɫ��Դ������ļ�·�����������ǾͿ��԰�Դ����ı��ļ�������Ӳ�����ˡ�
	����֮�⣬Ϊ�������ǵ����������һ�㣬��������һЩ���ߺ�����
	use����������ɫ���������е�set�������ܹ���ѯһ��unform��λ��ֵ����������ֵ��
	*/

private:
	void checkCompileErrors(unsigned int shader, std::string type);
};


//����ʵ��--------------------------------------------------------------------------------------------------------------------

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	//1.���ļ�·���л�ȡ����/Ƭ����ɫ��Դ���ļ�
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	//��֤ifstream�����׳��쳣
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//����ifstream����
		vShaderFile.clear();
		fShaderFile.clear();
		//���ļ�
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		//��ȡ�ļ��Ļ������ݵ�������
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//�ر��ļ�������
		vShaderFile.close();
		fShaderFile.close();
		//ת�����ݵ�string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (geometryPath != NULL)
		{
			//����ifstream����
			gShaderFile.clear();
			//���ļ�
			gShaderFile.open(geometryPath);
			//��ȡ�ļ��Ļ������ݵ�������
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			//�ر��ļ�������
			gShaderFile.close();
			//ת�����ݵ�string
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	//ת��Ϊc����ַ���
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();

	//������ɫ��
	unsigned int vertex, fragment, geometry = NULL;
	int success;
	char infoLog[512];

	//������ɫ��
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);//����
	checkCompileErrors(vertex, "VERTEX");	//������
	//Ƭ����ɫ��
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);//����
	checkCompileErrors(fragment, "FRAGMENT");	//������
	//������ɫ��
	if (geometryPath != NULL)
	{
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);//����
		checkCompileErrors(geometry, "GEOMETRY");	//������
	}

	//������ɫ������
	ID = glCreateProgram();
	//������ɫ��
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryPath != NULL)
	{
		glAttachShader(ID, geometry);
	}
	glLinkProgram(ID);//����
	checkCompileErrors(ID, "PROGRAM");	//���Ӽ��

	//ɾ����ɫ��(������Ͳ���Ҫ��)
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

//�����麯��
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