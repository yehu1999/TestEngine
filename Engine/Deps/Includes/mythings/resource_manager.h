#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "shader.h"
#include "texture.h"
#include "model.h"
#include "../stb_image.h"
#include<map>

/*
һ����̬��һʵ�� ResourceManager �࣬
�����ض�����ڼ����������ɫ���ĺ�����
ÿ�����ص������/����ɫ��Ҳ��洢����������ͨ���ַ����������ʡ�
���к�������Դ���Ǿ�̬�ģ�û�ж��幫�����캯����
*/
class ResourceManager
{
public:
	//��Դ����     ��map������[��ֵ]�����ʶ��ڵ�valueֵ��
	static std::map<std::string, Shader>    Shaders;
	static std::map<std::string, Texture2D> Textures;
	static std::map<std::string, Model>     Models;

	//��ɫ�������� ��ͨ����ȡGLSL�ļ���������ɫ��Program��
	static Shader    LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
	//��ɫ�������� ��ͨ����ɫ�����Ʒ�����ɫ����
	static Shader    GetShader(std::string name);

	//���������   ����ȡ�����ļ��������������ݣ�
	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
	//���������   ��ͨ���������Ʒ�����ɫ����
	static Texture2D GetTexture(std::string name);

	//ģ�ͼ�����
	static Model     LoadModel(const char* file, bool gamma, std::string name);
	//ģ�ͼ�����
	static Model     GetModel(std::string name);

	//�ͷ���Դ
	static void      Clear();  //�����Ѽ�����Դ
	static void      RemoveShader(std::string name);
	static void      RemoveTexture(std::string name);
	static void      RemoveModel(std::string name);

private:
	//˽�й��캯��  ��Ҳ����˵���ǲ���Ҫ�κ�ʵ�ʵ���Դ�������������Ա�͹���Ӧ�������ã���̬����)
	ResourceManager() { }

	//��ɫ�����������ڲ�ʵ��
	static Shader    loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
	//������������ڲ�ʵ��
	static Texture2D loadTextureFromFile(const char* file, bool alpha);
};


//����ʵ��--------------------------------------------------------------------------------------

//ʵ������̬����
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;
std::map<std::string, Model>        ResourceManager::Models;

//Shader
Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
	std::map<std::string, Shader>::iterator it;
	it = Shaders.find(name);

	if (it == Shaders.end())
		Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	else
		std::cout << name << " Shader has been loaded." << std::endl;

	return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

//Texture
Texture2D ResourceManager::LoadTexture(const char* file, bool gammaCorrection, std::string name)
{

	std::map<std::string, Texture2D>::iterator it;
	it = Textures.find(name);

	if (it == Textures.end())
		Textures[name] = loadTextureFromFile(file, gammaCorrection);
	else
		std::cout << name << " Texture has been loaded." << std::endl;

	return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
	return Textures[name];
}

//Model
Model ResourceManager::LoadModel(const char* file, bool gamma, std::string name)
{
	std::map<std::string, Model>::iterator it;
	it = Models.find(name);

	if (it == Models.end())
		Models[name] = Model(file, gamma);
	else
		std::cout << name << " Model has been loaded." << std::endl;

	return Models[name];
}

Model ResourceManager::GetModel(std::string name)
{
	return Models[name];
}

//other
void ResourceManager::Clear()
{
	//�ͷ�������ɫ��	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	//�ͷ���������
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
	//�ͷ�����ģ��
	for (auto iter : Models)
		iter.second.Destroy();
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		// open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// if geometry shader path is present, also load a geometry shader
		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();
	// 2. now create shader object from source code
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool gammaCorrection)
{
	// create texture object
	Texture2D texture;

	// load image
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	if (data)
	{
		// ��������
		if (nrChannels == 1)
		{
			texture.Internal_Format = texture.Image_Format = GL_RED;
		}
		else if (nrChannels == 3)
		{
			texture.Internal_Format = gammaCorrection ? GL_SRGB : GL_RGB;
			texture.Image_Format = GL_RGB;
		}
		else if (nrChannels == 4)
		{
			texture.Internal_Format = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			texture.Image_Format = GL_RGBA;
		}

		// now generate texture
		texture.Generate(width, height, data);
		// and finally free image data
		stbi_image_free(data);
	}
	else
	{
		// ������󣨼���ͼ��ʧ�ܣ�
		std::cout << "����ͼ��ʧ��" << std::endl;
	}

	return texture;
}


#endif