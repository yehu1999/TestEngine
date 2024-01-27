#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>

//Texture2D能够在OpenGL中存储和配置纹理。
//它还托管了很多实用函数，便于管理。
class Texture2D
{
public:
    //纹理对象的 ID，可引用此来对特定纹理进行操作
    unsigned int ID;

    //纹理图像宽高（单位：像素）
    unsigned int Width, Height; 

    //纹理格式
    unsigned int Internal_Format; //纹理对象格式
    unsigned int Image_Format; //图像格式

    //纹理设置
	unsigned int Wrap_S; //环绕模式 (on S axis)
	unsigned int Wrap_T; //环绕模式 (on T axis)
	unsigned int Filter_Min; //过滤模式 (if texture pixels < screen pixels)
	unsigned int Filter_Max; //过滤模式 (if texture pixels > screen pixels)

    //默认构造函数
    Texture2D();
    
	//生成纹理对象（从图像数据中）
    void Generate(unsigned int width, unsigned int height, unsigned char* data);
    
	//绑定当前纹理作为激活纹理
    void Bind() const;
};



//材质加载
GLuint loadTexture(char const* path, bool gammaCorrection = false);

//具体实现----------------------------------------------------------------------------------------
Texture2D::Texture2D()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR_MIPMAP_LINEAR), Filter_Max(GL_LINEAR)
{
    glGenTextures(1, &this->ID);
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;

    //创建纹理
	glGenTextures(1, &this->ID);
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    
	//设置纹理环绕过滤模式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);

	//生成MipMap
	glGenerateMipmap(GL_TEXTURE_2D);

    //解绑纹理
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}


//材质加载
GLuint loadTexture(char const* path, bool gammaCorrection)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if (nrComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrComponents == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

#endif