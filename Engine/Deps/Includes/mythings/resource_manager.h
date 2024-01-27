#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "shader.h"
#include "texture.h"
#include "model.h"
#include "../stb_image.h"
#include<map>

/*
一个静态单一实例 ResourceManager 类，
它承载多个用于加载纹理和着色器的函数。
每个加载的纹理和/或着色器也会存储起来，可以通过字符串将来访问。
所有函数和资源都是静态的，没有定义公共构造函数。
*/
class ResourceManager
{
public:
	//资源容器     （map可以用[键值]来访问对于的value值）
	static std::map<std::string, Shader>    Shaders;
	static std::map<std::string, Texture2D> Textures;
	static std::map<std::string, Model>     Models;

	//着色器加载器 （通过读取GLSL文件，生成着色器Program）
	static Shader    LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
	//着色器检索器 （通过着色器名称访问着色器）
	static Shader    GetShader(std::string name);

	//纹理加载器   （读取纹理文件，生成纹理数据）
	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
	//纹理检索器   （通过纹理名称访问着色器）
	static Texture2D GetTexture(std::string name);

	//模型加载器
	static Model     LoadModel(const char* file, bool gamma, std::string name);
	//模型检索器
	static Model     GetModel(std::string name);

	//释放资源
	static void      Clear();  //所有已加载资源
	static void      RemoveShader(std::string name);
	static void      RemoveTexture(std::string name);
	static void      RemoveModel(std::string name);

private:
	//私有构造函数  （也就是说我们不想要任何实际的资源管理器对象。其成员和功能应公开可用（静态）。)
	ResourceManager() { }

	//着色器加载器的内部实现
	static Shader    loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
	//纹理加载器的内部实现
	static Texture2D loadTextureFromFile(const char* file, bool alpha);
};


//具体实现--------------------------------------------------------------------------------------

//实例化静态变量
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
	//释放所有着色器	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	//释放所有纹理
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
	//释放所有模型
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
		// 生成纹理
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
		// 处理错误（加载图像失败）
		std::cout << "加载图像失败" << std::endl;
	}

	return texture;
}


#endif