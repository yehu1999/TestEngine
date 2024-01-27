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
	//窗口大小
	GLuint SCR_WIDTH, SCR_HEIGHT;

	//摄像机
	Camera camera;

	//控制
	GLboolean keys[1024];
	GLboolean keysPressed[1024];

	//时间
	GLfloat deltaTime; // 当前帧与上一帧的时间差
	GLfloat lastFrame; // 上一帧的时间

	//窗口模式
	screenMode ScreenMode;

	//渲染风格
	renderStyle  RenderStyle;

	//渲染模式
	renderMode   RenderMode;

	//全局环境光照
	glm::vec3 Ambient;
	float AmbientIntensity;

	//纹理观察器
	bool IfWatcherOpen;
	unsigned int WatchingTexture;

	State()
	{
		SCR_WIDTH = 1680; SCR_HEIGHT = 920;

		camera = Camera(glm::vec3(0.0f, 0.0f, 8.0f));//相机初始位置

		deltaTime = 0.0f; // 当前帧与上一帧的时间差
		lastFrame = 0.0f; // 上一帧的时间

		ScreenMode = SIMPLESCREEN;

		RenderStyle = FILL_STYLE;

		RenderMode = Phong;

		Ambient = { 1.0f,1.0f,1.0f };
		AmbientIntensity = 0.3f;
	};
	~State() {};

	//引擎全局状态初始化
	void Init()
	{
		Shader lightingShader = ResourceManager::GetShader("LightingShader");
		lightingShader.use();
		lightingShader.setInt("renderMode", 0);
		lightingShader.setVec3("Ambient", Ambient);
		lightingShader.setFloat("AmbientIntensity", AmbientIntensity);
	}

	//状态同步
	void Update()
	{
		Shader lightingShader = ResourceManager::GetShader("LightingShader");
		lightingShader.use();
		lightingShader.setInt("renderMode", RenderMode);
		lightingShader.setVec3("Ambient", Ambient);
		lightingShader.setFloat("AmbientIntensity", AmbientIntensity);
	}

	//点线模式判断
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

	//恢复默认状态
	void ResetState()
	{
		SCR_WIDTH = 1680; SCR_HEIGHT = 920;

		camera = Camera(glm::vec3(0.0f, 0.0f, 8.0f));//相机初始位置

		deltaTime = 0.0f; // 当前帧与上一帧的时间差
		lastFrame = 0.0f; // 上一帧的时间

		ScreenMode = SIMPLESCREEN;

		RenderStyle = FILL_STYLE;

		RenderMode = Phong;

		Ambient = { 1.0f,1.0f,1.0f };
		AmbientIntensity = 0.3f;
	}
};

#endif