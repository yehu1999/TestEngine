//HELLO TESTENGINE
#include<mythings/engine/engine.h>
#include<..\imgui\ui.h>

#include<iostream>
#include<string>
using std::cout;
using std::cin;
using std::endl;
#include<random>

//����
Engine engine;
State EngineState;

//����
void controller(GLFWwindow* window);
void processInput(GLFWwindow* window);
void FeedBack();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

//������
int main()
{
	//��������
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);   //�߿�ͱ����� 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	//��������
	GLFWwindow* window = glfwCreateWindow(EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT, "testEngine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);//���ô��ڽ������(����֡��)

	//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); //͸������
	//glfwSetWindowOpacity(window, 0.9f);                      //����͸����

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);               //���ù��ģʽ
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);         //ע�ᴰ�ڴ�С�����ص�����
	glfwSetKeyCallback(window, key_callback);                                  //ע����̻ص�����

	//glad����
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//��ʼ��ImGui
	ImGui_Init(window);

	//����y�ᷴת(�Ѿ���model.h���غ��ڴ������)
	//stbi_set_flip_vertically_on_load(true);

	//����openGL״̬
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);   //��Ȳ��Ժ�������ΪС�ڵ���(���ڻ�����պУ���Ϊ��պе���ȱ�����Ϊ���ֵ1.0)	//glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  //����������ͼ����֮�������ȷ���˵�ѡ��
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);

	//���ӿ�����Ϊԭʼ֡�������Ļ�ߴ�
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	framebuffer_size_callback(window, scrWidth, scrHeight);
	//glViewport(0, 0, scrWidth, scrHeight);

	//׼��֡����
	unsigned int fbo, Colorbuffer, DepthStencilBuffer;
	FrameBuffer(fbo, Colorbuffer, DepthStencilBuffer, scrWidth, scrHeight);

	//Engine��ʼ��
	engine.Init();

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		//per-frame time logic
		GLfloat currentFrame = static_cast<float>(glfwGetTime());
		EngineState.deltaTime = currentFrame - EngineState.lastFrame;
		EngineState.lastFrame = currentFrame;

		//�¼����
		glfwPollEvents();

		//������
		processInput(window);

		//������Ⱦ
		engine.Update();

		engine.Render(window, fbo);

		//UI��Ⱦ
		static bool BockingSpace_open = true;
		ImGui_Render(window, Colorbuffer, BockingSpace_open, engine);

		//˫����
		glfwSwapBuffers(window);

		//���з���
		FeedBack();
	}
	std::cout << "MISSION COMPLETE!" << std::endl;

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	engine.Clear();

	glfwDestroyWindow(window);

	//��ֹ�����������ǰ����� GLFW ��Դ��
	glfwTerminate();

	return 0;
}

//ÿ֡����
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
	//�������WASD
	if (EngineState.keys[GLFW_KEY_W])
		EngineState.camera.ProcessKeyboard(FORWARD, EngineState.deltaTime);
	if (EngineState.keys[GLFW_KEY_S])
		EngineState.camera.ProcessKeyboard(BACKWARD, EngineState.deltaTime);
	if (EngineState.keys[GLFW_KEY_A])
		EngineState.camera.ProcessKeyboard(LEFT, EngineState.deltaTime);
	if (EngineState.keys[GLFW_KEY_D])
		EngineState.camera.ProcessKeyboard(RIGHT, EngineState.deltaTime);
}

//ÿ֡����
void FeedBack()
{
	//camera.report();

}

//�ص�����
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

