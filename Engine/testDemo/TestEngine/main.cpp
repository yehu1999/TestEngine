//HELLO TESTENGINE
#include<mythings/engine/engine.h>
#include<..\imgui\ui.h>

#include<iostream>
#include<string>
using std::cout;
using std::cin;
using std::endl;
#include<random>

//引擎
Engine engine;
State EngineState;

//函数
void controller(GLFWwindow* window);
void processInput(GLFWwindow* window);
void FeedBack();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

//主函数
int main()
{
	//基础设置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);   //边框和标题栏 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	//窗口设置
	GLFWwindow* window = glfwCreateWindow(EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT, "testEngine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);//设置窗口交换间隔(限制帧率)

	//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); //透明窗口
	//glfwSetWindowOpacity(window, 0.9f);                      //设置透明度

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);               //设置光标模式
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);         //注册窗口大小调整回调函数
	glfwSetKeyCallback(window, key_callback);                                  //注册键盘回调函数

	//glad加载
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//初始化ImGui
	ImGui_Init(window);

	//开启y轴反转(已经在model.h记载后期处理过了)
	//stbi_set_flip_vertically_on_load(true);

	//设置openGL状态
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);   //深度测试函数设置为小于等于(用于绘制天空盒，因为天空盒的深度被设置为最大值1.0)	//glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  //在立方体贴图的面之间进行正确过滤的选项
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);

	//将视口配置为原始帧缓冲的屏幕尺寸
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	framebuffer_size_callback(window, scrWidth, scrHeight);
	//glViewport(0, 0, scrWidth, scrHeight);

	//准备帧缓冲
	unsigned int fbo, Colorbuffer, DepthStencilBuffer;
	FrameBuffer(fbo, Colorbuffer, DepthStencilBuffer, scrWidth, scrHeight);

	//Engine初始化
	engine.Init();

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//per-frame time logic
		GLfloat currentFrame = static_cast<float>(glfwGetTime());
		EngineState.deltaTime = currentFrame - EngineState.lastFrame;
		EngineState.lastFrame = currentFrame;

		//事件检测
		glfwPollEvents();

		//输入检测
		processInput(window);

		//场景渲染
		engine.Update();

		engine.Render(window, fbo);

		//UI渲染
		static bool BockingSpace_open = true;
		ImGui_Render(window, Colorbuffer, BockingSpace_open, engine);

		//双缓冲
		glfwSwapBuffers(window);

		//运行反馈
		FeedBack();
	}
	std::cout << "MISSION COMPLETE!" << std::endl;

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	engine.Clear();

	glfwDestroyWindow(window);

	//终止，清除所有先前分配的 GLFW 资源。
	glfwTerminate();

	return 0;
}

//每帧输入
void processInput(GLFWwindow* window)
{
	//controller
	controller(window);
}
void controller(GLFWwindow* window)
{
	//ESC
	if (EngineState.keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, true);
	//摄像机：WASD
	if (EngineState.keys[GLFW_KEY_W])
		EngineState.camera.ProcessKeyboard(FORWARD, EngineState.deltaTime);
	if (EngineState.keys[GLFW_KEY_S])
		EngineState.camera.ProcessKeyboard(BACKWARD, EngineState.deltaTime);
	if (EngineState.keys[GLFW_KEY_A])
		EngineState.camera.ProcessKeyboard(LEFT, EngineState.deltaTime);
	if (EngineState.keys[GLFW_KEY_D])
		EngineState.camera.ProcessKeyboard(RIGHT, EngineState.deltaTime);
}

//每帧反馈
void FeedBack()
{
	//camera.report();

}

//回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// glfw: whenever the window size changed (by OS or user resize) this callback function executes

	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
			EngineState.keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			EngineState.keys[key] = false;
			EngineState.keysPressed[key] = false;
		}
	}
}

