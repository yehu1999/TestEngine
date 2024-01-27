#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>

//Texture2D�ܹ���OpenGL�д洢����������
//�����й��˺ܶ�ʵ�ú��������ڹ���
class Texture2D
{
public:
    //�������� ID�������ô������ض�������в���
    unsigned int ID;

    //����ͼ���ߣ���λ�����أ�
    unsigned int Width, Height; 

    //�����ʽ
    unsigned int Internal_Format; //��������ʽ
    unsigned int Image_Format; //ͼ���ʽ

    //��������
	unsigned int Wrap_S; //����ģʽ (on S axis)
	unsigned int Wrap_T; //����ģʽ (on T axis)
	unsigned int Filter_Min; //����ģʽ (if texture pixels < screen pixels)
	unsigned int Filter_Max; //����ģʽ (if texture pixels > screen pixels)

    //Ĭ�Ϲ��캯��
    Texture2D();
    
	//����������󣨴�ͼ�������У�
    void Generate(unsigned int width, unsigned int height, unsigned char* data);
    
	//�󶨵�ǰ������Ϊ��������
    void Bind() const;
};



//���ʼ���
GLuint loadTexture(char const* path, bool gammaCorrection = false);

//����ʵ��----------------------------------------------------------------------------------------
Texture2D::Texture2D()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR_MIPMAP_LINEAR), Filter_Max(GL_LINEAR)
{
    glGenTextures(1, &this->ID);
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;

    //��������
	glGenTextures(1, &this->ID);
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    
	//���������ƹ���ģʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);

	//����MipMap
	glGenerateMipmap(GL_TEXTURE_2D);

    //�������
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}


//���ʼ���
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