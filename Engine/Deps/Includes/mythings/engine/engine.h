#ifndef ENGINE_H
#define ENGINE_H
#include<mythings/shader.h>
#include<mythings/camera.h>
#include<mythings/model.h>
#include<mythings/render.h>
#include<mythings/texture.h>
#include<mythings/resource_manager.h>

#include"object.h"
#include"light.h"
#include"state.h"

#include<list>
#include<map>
#include<vector>

//引擎状态
extern State EngineState;

//函数前调
void GeometryBuffer(GLuint& gBuffer, unsigned int& gPosition, unsigned int& gNormal, unsigned int& gColorSpec, unsigned int& gShininess, unsigned int& gAlbedo, unsigned int& gMetRougAo);
void ILB_Generater(unsigned int& irradianceMapOut, unsigned int& prefilterMapOut, unsigned int& brdfLUTTextureOut, unsigned int& envCubemapOut);

//光信息缓冲
const int MAX_LIGHTSNUM = 50;

class Engine
{
public:
	//数据成员
	std::vector<Object*> Objects;
	std::vector<Light*> Lights;

	//G缓冲
	GLuint gBuffer;                               //G缓冲对象
	unsigned int gPosition, gNormal, gColorSpec, gShininess, gAlbedo, gMetRougAo;  //G缓冲纹理

	//IBL
	unsigned int irradianceMap, prefilterMap, brdfLUTTexture;//IBL纹理
	unsigned int skyCubeMap;                                 //天空盒纹理

	Engine() {}
	~Engine() {}

	//引擎初始化
	void ObjectLoad()
	{
		//初始物体
		basicObject* defaultGround = new basicObject();
		defaultGround->Class = BASIC_QUAD;
		defaultGround->materialType = PHONG_VALUE;
		defaultGround->Color = { 1.0,1.0,1.0 };
		defaultGround->Size = { 10, 10, 10 };
		defaultGround->Position = { 0.0f, -2.0f, 0.0f };
		defaultGround->RotateAngle = { 90.0f, 0.0f, 0.0f };

		basicObject* defaultCube1 = new basicObject();
		defaultCube1->Class = BASIC_CUBE;
		defaultCube1->materialType = PHONG_VALUE;
		defaultCube1->Color = { 0.5,0.5,0.5 };
		defaultCube1->Size = { 1, 1, 1 };
		defaultCube1->Position = { 0.0f, 1.0f, 0.0f };
		defaultCube1->RotateAngle = { 0.0f, 0.0f, 90.0f };

		basicObject* defaultCube2 = new basicObject();
		defaultCube2->Class = BASIC_CUBE;
		defaultCube2->materialType = PHONG_VALUE;
		defaultCube2->Color = { 0.5,0.5,0.5 };
		defaultCube2->Size = { 1, 1, 1 };
		defaultCube2->Position = { 0.0f, 3.0f, 0.0f };
		defaultCube2->RotateAngle = { 0.0f, 0.0f, 90.0f };

		basicObject* defaultSphere1 = new basicObject();
		defaultSphere1->Class = BASIC_SPHERE;
		defaultSphere1->materialType = PHONG_VALUE;
		defaultSphere1->Color = { 0.5,0.5,0.5 };
		defaultSphere1->Size = { 1, 1, 1 };
		defaultSphere1->Position = { 0.0f, -1.0f, 0.0f };
		defaultSphere1->RotateAngle = { 0.0f, 0.0f, 90.0f };
		defaultSphere1->roughness = 0.0f;

		modelObject* defaultModel1 = new modelObject();
		defaultModel1->Size = { 0.2f, 0.2f, 0.2f };
		defaultModel1->Position = { 1.0f, -2.0f, 3.0f };

		modelObject* defaultModel2 = new modelObject();
		defaultModel2->Size = { 0.2f, 0.2f, 0.2f };
		defaultModel2->Position = { -1.0f, -2.0f, 3.0f };

		Objects.push_back(defaultGround);
		Objects.push_back(defaultCube1);
		Objects.push_back(defaultCube2);
		Objects.push_back(defaultSphere1);
		Objects.push_back(defaultModel1);
		Objects.push_back(defaultModel2);


		//物体重命名
		int NumOfCube, NumOfQuad, NumOfSphere, NumOfModel;
		NumOfCube = NumOfQuad = NumOfSphere = NumOfModel = 0;
		for (auto obj : Objects)
		{
			switch (obj->Class)
			{
			case BASIC_CUBE:obj->Name = "Cube_" + std::to_string(NumOfCube++); break;
			case BASIC_QUAD:obj->Name = "Quad_" + std::to_string(NumOfQuad++); break;
			case BASIC_SPHERE:obj->Name = "Sphere_" + std::to_string(NumOfSphere++); break;
			case EXTEND_MODEL:obj->Name = "Model_" + std::to_string(NumOfModel++); break;
			}

		}




	}
	void LightLoad()
	{
		//初始光照
		PointLight* defaultLight1 = new PointLight;
		PointLight* defaultLight2 = new PointLight;
		PointLight* defaultLight3 = new PointLight;
		PointLight* defaultLight4 = new PointLight;
		PointLight* defaultLight5 = new PointLight;
		PointLight* defaultLight6 = new PointLight;

		defaultLight1->Position = { -5.0f,5.0f,5.0f };
		defaultLight2->Position = { 5.0f,5.0f,-5.0f };
		defaultLight3->Position = { 5.0f,5.0f,5.0f };
		defaultLight4->Position = { -5.0f,5.0f,-5.0f };
		defaultLight5->Position = { 0.0f,3.0f,5.0f };
		defaultLight6->Position = { 0.0f,0.0f,5.0f };

		defaultLight1->Color = { 0.5f,0.3f,0.3f };
		defaultLight2->Color = { 0.3f,0.5f,0.3f };
		defaultLight3->Color = { 0.3f,0.3f,0.5f };
		defaultLight4->Color = { 1.0f,1.0f,1.0f };
		defaultLight5->Color = { 1.0f,1.0f,1.0f };
		defaultLight6->Color = { 1.0f,1.0f,1.0f };

		Lights.push_back(defaultLight1);
		Lights.push_back(defaultLight2);
		Lights.push_back(defaultLight3);
		Lights.push_back(defaultLight4);
		Lights.push_back(defaultLight5);
		Lights.push_back(defaultLight6);

		//加载光信息数组
		for (const auto& light : Lights)
		{
			light->GetData();
		}
	}
	void ShaderLoad()
	{
		//着色器加载
		const char* World_vsPath = "../../Deps/GLSL/Engine/WorldShader.vs";
		const char* World_fsPath = "../../Deps/GLSL/Engine/WorldShader.fs";
		const char* Lighting_vsPath = "../../Deps/GLSL/Engine/LightingShader.vs";
		const char* Lighting_fsPath = "../../Deps/GLSL/Engine/LightingShader.fs";
		const char* Light_vsPath = "../../Deps/GLSL/Engine/LightShader.vs";
		const char* Light_fsPath = "../../Deps/GLSL/Engine/LightShader.fs";
		const char* Sky_vsPath = "../../Deps/GLSL/Engine/SkyShader.vs";
		const char* Sky_fsPath = "../../Deps/GLSL/Engine/SkyShader.fs";

		Shader worldShader = ResourceManager::LoadShader(World_vsPath, World_fsPath, NULL, "WorldShader");                        //世界着色器
		Shader lightingShader = ResourceManager::LoadShader(Lighting_vsPath, Lighting_fsPath, NULL, "LightingShader");            //光照着色器
		Shader lightShader = ResourceManager::LoadShader(Light_vsPath, Light_fsPath, NULL, "LightShader");                        //光源着色器
		Shader skyShader = ResourceManager::LoadShader(Sky_vsPath, Sky_fsPath, NULL, "SkyShader");                                //天空着色器

		//设置纹理单元

		lightingShader.use();
		//G缓冲
		lightingShader.setInt("gPosition", 0);
		lightingShader.setInt("gNormal", 1);
		lightingShader.setInt("gColorSpec", 2);
		lightingShader.setInt("gShininess", 3);
		lightingShader.setInt("gAlbedo", 4);
		lightingShader.setInt("gMetRougAo", 5);
		//IBL
		lightingShader.setInt("irradianceMap", 6);
		lightingShader.setInt("prefilterMap", 7);
		lightingShader.setInt("brdfLUT", 8);

		worldShader.use();
		//Phong-Map
		worldShader.setInt("material.diffuseMap", 0);
		worldShader.setInt("material.specularMap", 1);
		//PBR-Map
		worldShader.setInt("material.albedoMap", 2);
		worldShader.setInt("material.metallicMap", 3);
		worldShader.setInt("material.roughnessMap", 4);
		worldShader.setInt("material.aoMap", 5);
		worldShader.setInt("material.normalMap", 6);

		skyShader.use();
		skyShader.setInt("skyMap", 0);
	}
	void TextureLoad()
	{
		//加载默认材质资源
		ResourceManager::LoadTexture("../../../Asset/textures/pbr/default_Titanium/albedo.png", false, "defaultAlbedo");
		ResourceManager::LoadTexture("../../../Asset/textures/pbr/default_Titanium/normal.png", false, "defaultNormal");
		ResourceManager::LoadTexture("../../../Asset/textures/pbr/default_Titanium/metallic.png", false, "defaultMetallic");
		ResourceManager::LoadTexture("../../../Asset/textures/pbr/default_Titanium/roughness.png", false, "defaultRoughness");
		ResourceManager::LoadTexture("../../../Asset/textures/pbr/default_Titanium/ao.png", false, "defaultAo");

		ResourceManager::LoadTexture("../../../Asset/textures/container2.png", true, "container_diffuse");
		ResourceManager::LoadTexture("../../../Asset/textures/container2_specular.png", true, "container_specular");
		ResourceManager::LoadTexture("../../../Asset/textures/matrix.jpg", true, "container_emission");
	}
	void ModelLoad()
	{
		//加载默认模型资源
		ResourceManager::LoadModel("../../../Asset/objects/nanosuit/nanosuit.obj", false, "default");

	}
	void Init()
	{
		TextureLoad();
		ModelLoad();
		ObjectLoad();
		LightLoad();
		ShaderLoad();

		//G缓冲生成
		GeometryBuffer(gBuffer, gPosition, gNormal, gColorSpec, gShininess, gAlbedo, gMetRougAo);

		//IBL生成
		ILB_Generater(irradianceMap, prefilterMap, brdfLUTTexture, skyCubeMap);

		//全局状态初始化
		EngineState.Init();
	}

	//引擎数据更改（正在施工）
	void ObjectAdd(ObjectType type)
	{
		//物体的增删
		if (type == BASIC_CUBE)
		{
			basicObject* newCube = new basicObject(EngineState.camera);
			newCube->Name = "newCube";
			Objects.push_back(newCube);
		}
		else if (type == BASIC_QUAD)
		{
			basicObject* newQuad = new basicObject(EngineState.camera);
			newQuad->Class = BASIC_QUAD;
			newQuad->Name = "newQuad";
			Objects.push_back(newQuad);
		}
		else if (type == BASIC_SPHERE)
		{
			basicObject* newSphere = new basicObject(EngineState.camera);
			newSphere->Class = BASIC_SPHERE;
			newSphere->Name = "newSphere";
			Objects.push_back(newSphere);
		}
		else if (type == EXTEND_MODEL)
		{
			modelObject* newModel = new modelObject(EngineState.camera);

			Objects.push_back(newModel);
		}

		//物体数据改变
		//貌似在UI中实现了

		//重新生成G缓冲
		//GeometryBuffer(gBuffer, gPosition, gNormal, gColorSpec, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT);

	}
	void ObjectRenderMode(renderMode mode)
	{
		if (mode == PBR)
			for (auto aObject : Objects)
			{
				if (aObject->Class != EXTEND_MODEL)
					aObject->materialType = PBR_VALUE;
				else
					aObject->materialType = MODEL_PBR;
			}
		else if (mode == Phong)
			for (auto aObject : Objects)
			{
				if (aObject->Class != EXTEND_MODEL)
					aObject->materialType = PHONG_VALUE;
				else
					aObject->materialType = MODEL_PHONG;
			}
	}
	void LightAdd(LightType type)
	{
		//光源的增删
		if (type == POINT_LIGHT)
		{
			PointLight* newLight = new PointLight(EngineState.camera);
			Lights.push_back(newLight);
			newLight->GetData();
		}
		else if (type == DIR_LIGHT)
		{
			DirLight* newLight = new DirLight(EngineState.camera);
			Lights.push_back(newLight);
			newLight->GetData();
		}
		else if (type == SPOT_LIGHT)
		{
			SpotLight* newLight = new SpotLight(EngineState.camera);
			Lights.push_back(newLight);
			newLight->GetData();
		}
		//光源数据改变
	}
	void ShaderChange()
	{
		//将那些在Update/Draw中的，不必每帧上传的变量移到 ShaderChange，有条件地上传更改
	}
	void TextureChange()
	{
		//纹理增删
	}
	void ModelChange()
	{

	}

	//引擎动态更新
	void ObjectUpdate()
	{
		//引擎运行时物体动态成员更新


	}
	void LightUpdate()
	{
		//更新光信息

	}
	void ShaderUpdate()
	{
		//引擎运行时着色器动态参数上传
		glm::mat4 projection = EngineState.camera.GetPerspectiveMatrix(EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT);
		glm::mat4 view = EngineState.camera.GetViewMatrix();

		Shader worldShader = ResourceManager::GetShader("WorldShader");
		worldShader.use();
		worldShader.setMat4("projection", projection);
		worldShader.setMat4("view", view);

		Shader lightingShader = ResourceManager::GetShader("LightingShader");
		lightingShader.use();
		lightingShader.setVec3("camPos", EngineState.camera.Position);

		Shader lightShader = ResourceManager::GetShader("LightShader");
		lightShader.use();
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);

		Shader skyShader = ResourceManager::GetShader("SkyShader");
		skyShader.use();
		skyShader.setMat4("projection", projection);
		skyShader.setMat4("view", view);

		//上传光信息数组        （每帧都要上传整个光源数组，消耗极大，急需实例化或SSBO来优化）
		lightingShader.use();
		lightingShader.setInt("LightsNum", Lights.size());
		for (GLuint i = 0; i < Lights.size(); ++i)
		{
			//更新光源数据
			Lights.at(i)->GetData();

			std::string prefix = "lights[" + std::to_string(i) + "].";
			// 传递光源数据
			lightingShader.setInt(prefix + "type", Lights[i]->data.type);
			lightingShader.setVec3(prefix + "position", Lights[i]->data.position);
			lightingShader.setVec3(prefix + "direction", Lights[i]->data.direction);
			lightingShader.setVec3(prefix + "diffuse", Lights[i]->data.diffuse);
			lightingShader.setVec3(prefix + "specular", Lights[i]->data.specular);
			lightingShader.setFloat(prefix + "constant", Lights[i]->data.constant);
			lightingShader.setFloat(prefix + "linear", Lights[i]->data.linear);
			lightingShader.setFloat(prefix + "quadratic", Lights[i]->data.quadratic);
			lightingShader.setFloat(prefix + "cutOff", Lights[i]->data.cutOff);
			lightingShader.setFloat(prefix + "outerCutOff", Lights[i]->data.outerCutOff);
			lightingShader.setFloat(prefix + "Radius", Lights[i]->data.Radius);
		}


	}
	void TextureUpdate()
	{
		//引擎运行时新材质添加

	}
	void ModelUpdate()
	{
		//引擎运行时新模型添加

	}
	void Update()
	{
		//引擎动态更新
		TextureUpdate();
		ModelUpdate();
		ObjectUpdate();
		LightUpdate();
		ShaderUpdate();

		EngineState.Update();
	}

	//渲染
	void Render(GLFWwindow*& window, unsigned int& fbo)
	{
		//帧更新动态数据
		Update();

		//G缓冲阶段
		Shader worldShader = ResourceManager::GetShader("WorldShader");
		worldShader.use();
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT);

		//物体几何信息缓冲
		EngineState.FillLinePoint();
		for (auto aObject : Objects)
		{
			aObject->Draw();
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//帧缓冲开始
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT);

		//光照阶段
		Shader lightingShader = ResourceManager::GetShader("LightingShader");
		lightingShader.use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT);
		//绑定G缓冲数据
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gColorSpec);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gShininess);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, gMetRougAo);

		//绑定预计算的IBL数据
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); //辐照度环境贴图
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);  //预滤波环境贴图
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);      //BRDF环境贴图

		renderQuad();

		//光源绘制
		EngineState.FillLinePoint();
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);  //读取：G缓冲
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);      //写入：帧缓冲对象
		glBlitFramebuffer(0, 0, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT, 0, 0, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glDisable(GL_CULL_FACE);
		for (auto aLight : Lights)
		{
			aLight->Draw();
		}
		glEnable(GL_CULL_FACE);

		//天空盒绘制
		Shader skyShader = ResourceManager::GetShader("SkyShader");
		skyShader.use();
		glDisable(GL_CULL_FACE);   //关闭面剔除，不然天空盒会被剔除
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyCubeMap);      //hdr立方体贴图
		renderCube();
		glEnable(GL_CULL_FACE);

		//帧缓冲结束
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//清理
	void Clear()
	{
		for (auto obj : Objects)
		{
			delete obj;
		}
		Objects.clear();

		for (auto lit : Lights)
		{
			delete lit;
		}
		Lights.clear();
	}

};

//颜色深度帧缓冲
void FrameBuffer(unsigned int& fbo, unsigned int& ColorBuffer, unsigned int& DepthStencilBuffer, int SCR_WIDTH, int SCR_HEIGHT)
{
	//------------------------
	//帧缓冲
	//创建帧缓冲对象 FBO
	glGenFramebuffers(1, &fbo);
	//绑定帧缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//创建帧缓冲颜色附件
	glGenTextures(1, &ColorBuffer);
	//绑定颜色附件
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);
	//生成颜色纹理
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//设置纹理
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//环绕方式使用GL_CLAMP_TO_EDGE，来减少核效果在屏幕边缘部分因为缺失像素而导致的奇怪影响
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//将纹理附加到帧缓冲
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0);
	//创建帧缓冲深度模板附件
	glGenTextures(1, &DepthStencilBuffer);
	//绑定深度模板附件
	glBindTexture(GL_TEXTURE_2D, DepthStencilBuffer);
	//生成深度模板纹理
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	//将纹理附加到帧缓冲
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, DepthStencilBuffer, 0);

	//渲染缓冲
	//创建渲染缓冲对象 RBO (效率比帧缓冲的附件纹理更高)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	//绑定渲染缓冲对象
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//生成深度模板渲染缓冲
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	//附加到帧缓冲
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//检测当前绑定的帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	//将帧缓冲绑定到0(即解绑，恢复主窗口的默认帧缓冲)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//G缓冲
void GeometryBuffer(GLuint& gBuffer, unsigned int& gPosition, unsigned int& gNormal, unsigned int& gColorSpec, unsigned int& gShininess, unsigned int& gAlbedo, unsigned int& gMetRougAo)
{
	//G缓冲（几何帧缓冲）
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//位置颜色附件   (储存在3位数据的RGB中)
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	//法线颜色附件   (储存在3位数据的RGB中)
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	//颜色(3位浮点数) + 镜面值(1位浮点数)颜色附件   (合并储存在4位数据的RGBA中)
	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);
	//反光度颜色附件（1 位浮点数）
	glGenTextures(1, &gShininess);
	glBindTexture(GL_TEXTURE_2D, gShininess);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gShininess, 0);
	//Albedo 颜色附件（3 位浮点数）
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gAlbedo, 0);
	//MetRougAo 颜色附件（3 位浮点数）
	glGenTextures(1, &gMetRougAo);
	glBindTexture(GL_TEXTURE_2D, gMetRougAo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gMetRougAo, 0);

	//告诉 OpenGL 我们将要使用（帧缓冲的）多个颜色附件来进行渲染（多渲染目标）
	GLuint attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
	glDrawBuffers(6, attachments);

	//深度rbo附件
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, EngineState.SCR_WIDTH, EngineState.SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	//检查帧缓冲
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//IBL
void ILB_Generater(unsigned int& irradianceMapOut, unsigned int& prefilterMapOut, unsigned int& brdfLUTTextureOut, unsigned int& envCubemapOut)
{
	glDisable(GL_CULL_FACE);    //对天空盒的操作请关闭面剔除

	//等距柱状投影贴图
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf("../../../Asset/textures/hdr/newport_loft.hdr", &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}

	// DATAs
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	//着色器文件路径
	const char* C_vsPath = "../../Deps/GLSL/Engine/IBL/vERtoCubeMapShader.vs";
	const char* C_fsPath = "../../Deps/GLSL/Engine/IBL/fERtoCubeMapShader.fs";
	const char* I_fsPath = "../../Deps/GLSL/Engine/IBL/fIrradianceShader.fs";
	const char* PF_fsPath = "../../Deps/GLSL/Engine/IBL/fPrefilterShader.fs";
	const char* BRDF_vsPath = "../../Deps/GLSL/Engine/IBL/vBrdfShader.vs";
	const char* BRDF_fsPath = "../../Deps/GLSL/Engine/IBL/fBrdfShader.fs";

	//加载着色器 
	Shader equirectangularToCubemapShader = ResourceManager::LoadShader(C_vsPath, C_fsPath, nullptr, "equirectangularToCubemapShader");//等距柱状投影贴图转立方体贴图
	Shader irradianceShader = ResourceManager::LoadShader(C_vsPath, I_fsPath, nullptr, "irradianceShader");           //辐照度
	Shader prefilterShader = ResourceManager::LoadShader(C_vsPath, PF_fsPath, nullptr, "prefilterShader");           //预过滤
	Shader brdfShader = ResourceManager::LoadShader(BRDF_vsPath, BRDF_fsPath, nullptr, "brdfShader");           //预计算BRDF

	//帧缓冲-------------------------------------------------------------------------
	//capture帧缓冲（用于向六个方向“拍摄”）
	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	//颜色附件 （立方体贴图）
	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//生成立方体贴图（将hdr等距柱状投影贴图转换为立方体贴图）
	equirectangularToCubemapShader.use();
	equirectangularToCubemapShader.setInt("equirectangularMap", 0);
	equirectangularToCubemapShader.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//生成辐照度贴图帧缓冲 （用于储存辐照度漫反射卷积结果）
	unsigned int irradianceMap;
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	//生成辐照度贴图（在着色器内对立方体贴图进行卷积）
	irradianceShader.use();
	irradianceShader.setInt("environmentMap", 0);
	irradianceShader.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//预滤波环境贴图帧缓冲 （对立方体贴图进行多级别模糊，对应不同粗糙度级别，将被用于之后BRDF的镜面反射计算）
	unsigned int prefilterMap;
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//生成mipmap
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//生成各级预滤波环境贴图
	prefilterShader.use();
	prefilterShader.setInt("environmentMap", 0);
	prefilterShader.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//2D LUT纹理帧缓冲 （BRDF 积分贴图，用于储存BRDF镜面反射的卷积结果）
	unsigned int brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);
	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//生成LUT纹理 （在着色器内对立方体贴图进行卷积）
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	brdfShader.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//返回结果
	irradianceMapOut = irradianceMap;
	prefilterMapOut = prefilterMap;
	brdfLUTTextureOut = brdfLUTTexture;
	envCubemapOut = envCubemap;

	glEnable(GL_CULL_FACE);

}

//test
void PBR_ILB_Renderer(unsigned int irradianceMap, unsigned int prefilterMap, unsigned int brdfLUTTexture, unsigned int envCubemap, Camera& camera, GLfloat SCR_WIDTH, GLfloat SCR_HEIGHT)
{
	//加载纹理
	static unsigned int albedo[5] =
	{
		loadTexture("../../../Asset/textures/pbr/gold/albedo.png"),
		loadTexture("../../../Asset/textures/pbr/grass/albedo.png"),
		loadTexture("../../../Asset/textures/pbr/plastic/albedo.png"),
		loadTexture("../../../Asset/textures/pbr/rusted_iron/albedo.png"),
		loadTexture("../../../Asset/textures/pbr/wall/albedo.png")
	};
	static unsigned int normal[5] =
	{
		loadTexture("../../../Asset/textures/pbr/gold/normal.png"),
		loadTexture("../../../Asset/textures/pbr/grass/normal.png"),
		loadTexture("../../../Asset/textures/pbr/plastic/normal.png"),
		loadTexture("../../../Asset/textures/pbr/rusted_iron/normal.png"),
		loadTexture("../../../Asset/textures/pbr/wall/normal.png")
	};
	static unsigned int metallic[5] =
	{
		loadTexture("../../../Asset/textures/pbr/gold/metallic.png"),
		loadTexture("../../../Asset/textures/pbr/grass/metallic.png"),
		loadTexture("../../../Asset/textures/pbr/plastic/metallic.png"),
		loadTexture("../../../Asset/textures/pbr/rusted_iron/metallic.png"),
		loadTexture("../../../Asset/textures/pbr/wall/metallic.png")
	};
	static unsigned int roughness[5] =
	{
		loadTexture("../../../Asset/textures/pbr/gold/roughness.png"),
		loadTexture("../../../Asset/textures/pbr/grass/roughness.png"),
		loadTexture("../../../Asset/textures/pbr/plastic/roughness.png"),
		loadTexture("../../../Asset/textures/pbr/rusted_iron/roughness.png"),
		loadTexture("../../../Asset/textures/pbr/wall/roughness.png")
	};
	static unsigned int ao[5] =
	{
		loadTexture("../../../Asset/textures/pbr/gold/ao.png"),
		loadTexture("../../../Asset/textures/pbr/grass/ao.png"),
		loadTexture("../../../Asset/textures/pbr/plastic/ao.png"),
		loadTexture("../../../Asset/textures/pbr/rusted_iron/ao.png"),
		loadTexture("../../../Asset/textures/pbr/wall/ao.png")
	};

	//DATA
	static glm::vec3 lightPositions[] =
	{
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	static glm::vec3 lightColors[] =
	{
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	//getShader
	static Shader pbrShader = ResourceManager::GetShader("pbrShader");
	static Shader objectShader = ResourceManager::GetShader("objectShader");
	static Shader equirectangularToCubemapShader = ResourceManager::GetShader("equirectangularToCubemapShader");
	static Shader irradianceShader = ResourceManager::GetShader("irradianceShader");
	static Shader prefilterShader = ResourceManager::GetShader("prefilterShader");
	static Shader brdfShader = ResourceManager::GetShader("brdfShader");

	static Shader lightShader = ResourceManager::GetShader("lightShader");
	static Shader backgroundShader = ResourceManager::GetShader("backgroundShader");

	//getModel
	//**

	//设置着色器动态视消息
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	pbrShader.use();
	pbrShader.setMat4("projection", projection);
	pbrShader.setMat4("view", view);
	pbrShader.setVec3("camPos", camera.Position);

	//绑定预计算的IBL数据材质
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); //辐照度环境贴图
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);  //预滤波环境贴图
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);      //BRDF环境贴图

	//render
	//绘制纯色球体阵列
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;
	pbrShader.use();
	for (int row = 0; row < nrRows; ++row)
	{
		pbrShader.setFloat("metallic", (float)row / (float)nrRows);
		for (int col = 0; col < nrColumns; ++col)
		{
			// we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
			// on direct lighting.
			pbrShader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(col - (nrColumns / 2)) * spacing,
				(row - (nrRows / 2)) * spacing,
				0.0f
			));
			pbrShader.setMat4("model", model);
			pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
			renderSphere();
		}
	}
	//绘制材质球体
	objectShader.use();
	objectShader.setMat4("projection", projection);
	objectShader.setMat4("view", view);
	objectShader.setVec3("camPos", camera.Position);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.5, 0.0, 5.0));
	model = glm::scale(model, glm::vec3(0.5f));
	for (int i = 0; i < 5; i++)
	{
		model = glm::translate(model, glm::vec3(3.0, 0.0, 0.0));
		objectShader.setMat4("model", model);
		objectShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedo[i]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal[i]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallic[i]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughness[i]);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ao[i]);

		renderSphere();
	}

	//绘制模型
	//**

	//渲染光源
	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[i];
		pbrShader.use();
		pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

		objectShader.use();
		objectShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		objectShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

		model = glm::mat4(1.0f);
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.5f));
		pbrShader.setMat4("model", model);
		pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

		//绘制光源
		ResourceManager::GetShader("lightShader").use();
		ResourceManager::GetShader("lightShader").setMat4("model", model);
		ResourceManager::GetShader("lightShader").setMat4("projection", projection);
		ResourceManager::GetShader("lightShader").setMat4("view", view);
		ResourceManager::GetShader("lightShader").setVec3("lightColor", 1.0, 1.0, 1.0);
		renderSphere();
	}

	//绘制天空盒（最后绘制，防止场景被覆盖。用于展示和调试生成的立体纹理）
	glDisable(GL_CULL_FACE);   //关闭面剔除，不然天空盒会被剔除
	backgroundShader.use();
	backgroundShader.setMat4("projection", projection);
	backgroundShader.setMat4("view", view);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);    //pbr立体贴图
	//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); //pbr辐照度贴图
	//glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); // display prefilter map
	renderCube();
	glEnable(GL_CULL_FACE);

	//DEBUG:render BRDF map to screen
	//brdfShader.use();
	//renderQuad();

}

#endif