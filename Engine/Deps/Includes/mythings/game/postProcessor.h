#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../texture.h"
#include "../render.h"
#include "../shader.h"


//后处理器托管突破游戏的所有后处理效果。
//它将游戏渲染在一个带纹理的四边形上，之后可以通过启用the Confuse, Chaos or  Shake布尔值来启用特定效果。
//需要在渲染游戏之前调用 BeginRender（），在渲染游戏之后调用 EndRender（），类才能正常工作。
class PostProcessor
{
public:
	//后处理参数
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width, Height;

	//特效选项
	bool Confuse, Chaos, Shake;

	//构造函数
	PostProcessor(Shader shader, unsigned int width, unsigned int height);

	//准备帧缓冲处理
	void BeginRender();

	//结束帧缓冲，储存纹理
	void EndRender();

	//绘制帧缓冲纹理
	void Render(float time);

private:

	//帧缓冲参数
	unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO 多采样帧缓冲对象
	unsigned int RBO;
	unsigned int VAO;

	//初始化渲染数据
	void initRenderData();
};

//具体实现------------------------------------------------------------------------------------------

PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height)
	: PostProcessingShader(shader), Texture(), Width(width), Height(height), Confuse(false), Chaos(false), Shake(false)
{
	//生成帧缓冲对象
	glGenFramebuffers(1, &this->MSFBO);  //多重采样帧缓冲对象
	glGenFramebuffers(1, &this->FBO);    //帧缓冲对象
	glGenRenderbuffers(1, &this->RBO);   //渲染缓冲对象

	//解释一下
	/*
	MSFBO（多重采样帧缓冲对象）是一个用于多重采样的帧缓冲对象。
	它用于存储多重采样后的渲染结果，通常用于实现抗锯齿效果。
	在这段代码中，将渲染缓冲对象（RBO）附加到MSFBO的颜色附件上。

	FBO（帧缓冲对象）是一个普通的帧缓冲对象。
	它用于存储渲染结果，可以附加纹理对象或渲染缓冲对象作为颜色附件。
	在这段代码中，将纹理对象（texture）附加到FBO的颜色附件上。

	RBO（渲染缓冲对象）是一个用于存储渲染结果的缓冲对象。
	它可以用于存储深度、模板等渲染数据。
	在这段代码中，使用渲染缓冲对象（RBO）作为多重采样帧缓冲对象（MSFBO）的颜色附件。
	*/

	//绑定多重采样帧缓冲对象
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	//绑定渲染缓冲对象
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	//为渲染缓冲对象分配存储
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);
	//将渲染缓冲对象（RBO）附加到多重采样帧缓冲对象（MSFBO）的颜色附件上
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); 
	//检测多重采样帧缓冲对象完整性
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

	//绑定帧缓冲对象
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	//生成一个纹理对象，并设置其参数
	this->Texture.Generate(width, height, NULL);
	//将纹理对象附加到帧缓冲对象的颜色附件
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0);
	//检查帧缓冲对象的完整性
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	//解绑
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//初始化渲染数据，设置uniform
	this->initRenderData();
	this->PostProcessingShader.setInt("scene", 0, true);
	//偏移量
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

    //边缘卷积内核
	int edge_kernel[9] =
	{
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);
	//模糊卷积内核
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
	//将多重采样颜色附件解析为中间 FBO 以存储到纹理
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
	glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //将读取和写入帧缓冲绑定到默认帧缓冲
}

void PostProcessor::Render(float time)
{
	//设置uniform选项
	this->PostProcessingShader.use();
	this->PostProcessingShader.setFloat("time", time);
	this->PostProcessingShader.setInt("confuse", this->Confuse);
	this->PostProcessingShader.setInt("chaos", this->Chaos);
	this->PostProcessingShader.setInt("shake", this->Shake);

	//绘制屏幕
	glActiveTexture(GL_TEXTURE0);
	this->Texture.Bind();
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
	//设置VAO/VBO
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