#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../texture.h"
#include "../render.h"
#include "../shader.h"


//�������й�ͻ����Ϸ�����к���Ч����
//������Ϸ��Ⱦ��һ����������ı����ϣ�֮�����ͨ������the Confuse, Chaos or  Shake����ֵ�������ض�Ч����
//��Ҫ����Ⱦ��Ϸ֮ǰ���� BeginRender����������Ⱦ��Ϸ֮����� EndRender���������������������
class PostProcessor
{
public:
	//�������
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width, Height;

	//��Чѡ��
	bool Confuse, Chaos, Shake;

	//���캯��
	PostProcessor(Shader shader, unsigned int width, unsigned int height);

	//׼��֡���崦��
	void BeginRender();

	//����֡���壬��������
	void EndRender();

	//����֡��������
	void Render(float time);

private:

	//֡�������
	unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO �����֡�������
	unsigned int RBO;
	unsigned int VAO;

	//��ʼ����Ⱦ����
	void initRenderData();
};

//����ʵ��------------------------------------------------------------------------------------------

PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height)
	: PostProcessingShader(shader), Texture(), Width(width), Height(height), Confuse(false), Chaos(false), Shake(false)
{
	//����֡�������
	glGenFramebuffers(1, &this->MSFBO);  //���ز���֡�������
	glGenFramebuffers(1, &this->FBO);    //֡�������
	glGenRenderbuffers(1, &this->RBO);   //��Ⱦ�������

	//����һ��
	/*
	MSFBO�����ز���֡���������һ�����ڶ��ز�����֡�������
	�����ڴ洢���ز��������Ⱦ�����ͨ������ʵ�ֿ����Ч����
	����δ����У�����Ⱦ�������RBO�����ӵ�MSFBO����ɫ�����ϡ�

	FBO��֡���������һ����ͨ��֡�������
	�����ڴ洢��Ⱦ��������Ը�������������Ⱦ���������Ϊ��ɫ������
	����δ����У����������texture�����ӵ�FBO����ɫ�����ϡ�

	RBO����Ⱦ���������һ�����ڴ洢��Ⱦ����Ļ������
	���������ڴ洢��ȡ�ģ�����Ⱦ���ݡ�
	����δ����У�ʹ����Ⱦ�������RBO����Ϊ���ز���֡�������MSFBO������ɫ������
	*/

	//�󶨶��ز���֡�������
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	//����Ⱦ�������
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	//Ϊ��Ⱦ����������洢
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);
	//����Ⱦ�������RBO�����ӵ����ز���֡�������MSFBO������ɫ������
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); 
	//�����ز���֡�������������
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

	//��֡�������
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	//����һ��������󣬲����������
	this->Texture.Generate(width, height, NULL);
	//��������󸽼ӵ�֡����������ɫ����
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0);
	//���֡��������������
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	//���
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//��ʼ����Ⱦ���ݣ�����uniform
	this->initRenderData();
	this->PostProcessingShader.setInt("scene", 0, true);
	//ƫ����
	float offset = 1.0f / 300.0f;
	glm::vec2 offsets[9] =
	{
	{ -offset,  offset  },  // top-left
	{  0.0f,    offset  },  // top-center
	{  offset,  offset  },  // top-right
	{ -offset,  0.0f    },  // center-left
	{  0.0f,    0.0f    },  // center-center
	{  offset,  0.0f    },  // center - right
	{ -offset, -offset  },  // bottom-left
	{  0.0f,   -offset  },  // bottom-center
	{  offset, -offset  }   // bottom-right    
	};
	PostProcessingShader.setVec2fv("offsets", 9, offsets[0]);
	//glUniform2fv(glGetUniformLocation(this->PostProcessingShader.ID, "offsets"), 9, (float*)offsets);

    //��Ե����ں�
	int edge_kernel[9] =
	{
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);
	//ģ������ں�
	float blur_kernel[9] =
	{
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
	};
	glUniform1fv(glGetUniformLocation(this->PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::BeginRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
void PostProcessor::EndRender()
{
	//�����ز�����ɫ��������Ϊ�м� FBO �Դ洢������
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
	glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //����ȡ��д��֡����󶨵�Ĭ��֡����
}

void PostProcessor::Render(float time)
{
	//����uniformѡ��
	this->PostProcessingShader.use();
	this->PostProcessingShader.setFloat("time", time);
	this->PostProcessingShader.setInt("confuse", this->Confuse);
	this->PostProcessingShader.setInt("chaos", this->Chaos);
	this->PostProcessingShader.setInt("shake", this->Shake);

	//������Ļ
	glActiveTexture(GL_TEXTURE0);
	this->Texture.Bind();
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
	//����VAO/VBO
	unsigned int VBO;
	float vertices[] =
	{
		// pos        // tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}




#endif