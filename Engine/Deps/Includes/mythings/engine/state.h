#ifndef STATE_H
#define STATE_H

#include<mythings/engine/engine.h>
#include<mythings/camera.h>
#include"../resource_manager.h"

//RenderState
enum renderStyle {
	FILL_STYLE, POINT_STYLE, LINE_STYLE
};
enum screenMode {
	FULLSCREEN, SIMPLESCREEN
};
enum renderMode {
	Phong, PBR
};

class State
{
public:
	//���ڴ�С
	GLuint SCR_WIDTH, SCR_HEIGHT;

	//�����
	Camera camera;

	//����
	GLboolean keys[1024];
	GLboolean keysPressed[1024];

	//ʱ��
	GLfloat deltaTime; // ��ǰ֡����һ֡��ʱ���
	GLfloat lastFrame; // ��һ֡��ʱ��

	//����ģʽ
	screenMode ScreenMode;

	//��Ⱦ���
	renderStyle  RenderStyle;

	//��Ⱦģʽ
	renderMode   RenderMode;

	//ȫ�ֻ�������
	glm::vec3 Ambient;
	float AmbientIntensity;

	//����۲���
	bool IfWatcherOpen;
	unsigned int WatchingTexture;

	State()
	{
		SCR_WIDTH = 1680; SCR_HEIGHT = 920;

		camera = Camera(glm::vec3(0.0f, 0.0f, 8.0f));//�����ʼλ��

		deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
		lastFrame = 0.0f; // ��һ֡��ʱ��

		ScreenMode = SIMPLESCREEN;

		RenderStyle = FILL_STYLE;

		RenderMode = Phong;

		Ambient = { 1.0f,1.0f,1.0f };
		AmbientIntensity = 0.3f;
	};
	~State() {};

	//����ȫ��״̬��ʼ��
	void Init()
	{
		Shader lightingShader = ResourceManager::GetShader("LightingShader");
		lightingShader.use();
		lightingShader.setInt("renderMode", 0);
		lightingShader.setVec3("Ambient", Ambient);
		lightingShader.setFloat("AmbientIntensity", AmbientIntensity);
	}

	//״̬ͬ��
	void Update()
	{
		Shader lightingShader = ResourceManager::GetShader("LightingShader");
		lightingShader.use();
		lightingShader.setInt("renderMode", RenderMode);
		lightingShader.setVec3("Ambient", Ambient);
		lightingShader.setFloat("AmbientIntensity", AmbientIntensity);
	}

	//����ģʽ�ж�
	void FillLinePoint()
	{
		if (RenderStyle == FILL_STYLE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else if (RenderStyle == LINE_STYLE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (RenderStyle == POINT_STYLE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
	}

	//�ָ�Ĭ��״̬
	void ResetState()
	{
		SCR_WIDTH = 1680; SCR_HEIGHT = 920;

		camera = Camera(glm::vec3(0.0f, 0.0f, 8.0f));//�����ʼλ��

		deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
		lastFrame = 0.0f; // ��һ֡��ʱ��

		ScreenMode = SIMPLESCREEN;

		RenderStyle = FILL_STYLE;

		RenderMode = Phong;

		Ambient = { 1.0f,1.0f,1.0f };
		AmbientIntensity = 0.3f;
	}
};

#endif