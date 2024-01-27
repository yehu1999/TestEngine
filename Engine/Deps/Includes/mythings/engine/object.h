#ifndef OBJECT_H
#define OBJECT_H
#include<mythings/shader.h>
#include<mythings/camera.h>
#include<mythings/model.h>
#include<mythings/render.h>
#include<mythings/texture.h>
#include<mythings/resource_manager.h>

#include"state.h"

extern State EngineState;

//物体种类
enum ObjectType
{
	BASIC_CUBE,
	BASIC_QUAD,
	BASIC_SPHERE,
	EXTEND_MODEL,
};

//BasicObject_MaterialType
enum MaterialType
{
	PHONG_VALUE = 0,
	PHONG_MAP = 1,
	PBR_VALUE = 2,
	PBR_MAP = 3,

	MODEL_PHONG = 4,
	MODEL_PBR = 5,
};

//物体父类
class Object
{
public:
	// object state
	ObjectType Class;                         //物体类别
	MaterialType materialType;                //材质类型
	std::string Name;                         //名称
	glm::vec3   Position, Size, RotateAngle;  //位置， 长宽高，三轴旋转角度
	glm::vec3   Velocity;                     //速度

	//构造函数
	Object()
	{
		Name = "newObject";
		Position = { 0.0f,0.0f,0.0f };
		Size = { 1.0f,1.0f,1.0f };
		RotateAngle = { 0.0f,0.0f,0.0f };
		Velocity = { 0.0f,0.0f,0.0f };
	}
	Object(Camera& camera)
	{
		Name = "newObject";
		Position = camera.Position + camera.Front + camera.Front + camera.Front + camera.Front;

		Size = { 1.0f,1.0f,1.0f };
		RotateAngle = { 0.0f,0.0f,0.0f };
		Velocity = { 0.0f,0.0f,0.0f };
	}

	//析构函数
	~Object() {}

	//绘制
	virtual void Draw() {};

};

//基础物体子类
class basicObject :public Object
{
public:
	GLuint sphereSegments;

	//Value-Phong
	glm::vec3   Color;                        //颜色
	float Specular;                           //镜面值
	//Map-Phong
	Texture2D diffuseMap, specularMap, emissionMap;
	//Common-Phong
	float Shininess;

	//Value-PBR
	glm::vec3 albedo;
	float metallic, roughness, ao;
	//Map-PBR
	Texture2D albedoMap, normalMap, metallicMap, roughnessMap, aoMap;

	//构造函数
	basicObject() : Object()
	{
		Class = BASIC_CUBE;
		sphereSegments = 64;

		materialType = PHONG_VALUE;


		Color = { 1.0f,0.7529f,0.7961f };
		Specular = 0.5f;
		Shininess = 32;

		diffuseMap = ResourceManager::GetTexture("container_diffuse");
		specularMap = ResourceManager::GetTexture("container_specular");

		albedo = { 1.0f,0.7529f,0.7961f };
		metallic = 0.7f;
		roughness = 0.3f;
		ao = 0.3f;

		albedoMap = ResourceManager::GetTexture("defaultAlbedo");
		normalMap = ResourceManager::GetTexture("defaultNormal");
		metallicMap = ResourceManager::GetTexture("defaultMetallic");
		roughnessMap = ResourceManager::GetTexture("defaultRoughness");
		aoMap = ResourceManager::GetTexture("defaultAo");

	}
	basicObject(Camera& camera) : Object(camera)
	{
		Class = BASIC_CUBE;
		sphereSegments = 64;

		if (EngineState.RenderMode == Phong)
			materialType = PHONG_VALUE;
		else if (EngineState.RenderMode == PBR)
			materialType = PBR_VALUE;

		Color = { 1.0f,0.7529f,0.7961f };
		Specular = 0.5f;
		Shininess = 32;

		diffuseMap = ResourceManager::GetTexture("container_diffuse");
		specularMap = ResourceManager::GetTexture("container_specular");
		emissionMap = ResourceManager::GetTexture("container_emission");

		albedo = { 1.0f,0.7529f,0.7961f };
		metallic = 0.7f;
		roughness = 0.3f;
		ao = 0.3f;

		albedoMap = ResourceManager::GetTexture("defaultAlbedo");
		normalMap = ResourceManager::GetTexture("defaultNormal");
		metallicMap = ResourceManager::GetTexture("defaultMetallic");
		roughnessMap = ResourceManager::GetTexture("defaultRoughness");
		aoMap = ResourceManager::GetTexture("defaultAo");
	}

	//析构函数
	~basicObject() {}

	//绘制
	void Draw() override
	{
		Shader worldShader = ResourceManager::GetShader("WorldShader");

		worldShader.use();
		//上传模型矩阵
		glm::mat4 ModelMat = glm::mat4(1.0f);
		Position += Velocity;
		ModelMat = glm::translate(ModelMat, Position);
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMat = glm::scale(ModelMat, Size);
		worldShader.setMat4("model", ModelMat);

		//激活纹理单元并绑定纹理/上传材质参数
		worldShader.use();
		worldShader.setBool("isModel", false);
		if (materialType == PBR_MAP)
		{
			worldShader.setInt("material.type", 3);
			glActiveTexture(GL_TEXTURE2);
			albedoMap.Bind();
			glActiveTexture(GL_TEXTURE3);
			metallicMap.Bind();
			glActiveTexture(GL_TEXTURE4);
			roughnessMap.Bind();
			glActiveTexture(GL_TEXTURE5);
			aoMap.Bind();
			glActiveTexture(GL_TEXTURE6);
			normalMap.Bind();
		}
		else if (materialType == PBR_VALUE)
		{
			worldShader.setInt("material.type", 2);
			worldShader.setVec3("material.albedo", albedo);
			worldShader.setFloat("material.metallic", metallic);
			worldShader.setFloat("material.roughness", roughness);
			worldShader.setFloat("material.ao", ao);
		}
		else if (materialType == PHONG_MAP)
		{
			worldShader.setInt("material.type", 1);
			worldShader.setFloat("material.shininess", Shininess);
			glActiveTexture(GL_TEXTURE0);
			diffuseMap.Bind();
			glActiveTexture(GL_TEXTURE1);
			specularMap.Bind();
		}
		else if (materialType == PHONG_VALUE)
		{
			worldShader.setInt("material.type", 0);
			worldShader.setVec3("material.diffuse", Color);
			worldShader.setFloat("material.specular", Specular);
			worldShader.setFloat("material.shininess", Shininess);
		}

		//绘制
		worldShader.use();
		switch (Class)
		{
		case BASIC_CUBE:
		{
			renderCube();
			break;
		}
		case BASIC_QUAD:
		{
			glDisable(GL_CULL_FACE);
			renderWall();
			glEnable(GL_CULL_FACE);
			break;
		}
		case BASIC_SPHERE:
		{
			renderSphere(sphereSegments);
			break;
		}
		}
	}

};

//模型物体子类
class modelObject :public Object
{
public:
	Model model;

	//Common-Phong
	float Shininess;

	//PBR
	float metallic, roughness, ao;

	//构造函数
	modelObject() :Object()
	{
		Class = EXTEND_MODEL;
		materialType = MODEL_PHONG;
		Name = "newModel";
		Shininess = 32;
		metallic = 0.7f;
		roughness = 0.3f;
		ao = 0.3f;
		model = ResourceManager::GetModel("default");
	}
	modelObject(Camera& camera) :Object(camera)
	{
		Class = EXTEND_MODEL;

		if (EngineState.RenderMode == Phong)
			materialType = MODEL_PHONG;
		else if (EngineState.RenderMode == PBR)
			materialType = MODEL_PBR;

		Name = "newModel";
		Shininess = 32;
		metallic = 0.7f;
		roughness = 0.3f;
		ao = 0.3f;
		model = ResourceManager::GetModel("default");
	}

	//析构函数
	~modelObject() {}

	//绘制
	void Draw() override
	{
		Shader worldShader = ResourceManager::GetShader("WorldShader");
		worldShader.use();

		//上传模型矩阵
		glm::mat4 ModelMat = glm::mat4(1.0f);
		Position += Velocity;
		ModelMat = glm::translate(ModelMat, Position);
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMat = glm::scale(ModelMat, Size);

		worldShader.setMat4("model", ModelMat);
		worldShader.setBool("isModel", true);
		if (materialType == MODEL_PHONG)
		{
			worldShader.setInt("material.type", 4);
			worldShader.setFloat("material.shininess", Shininess);
		}
		else if (materialType == MODEL_PBR)
		{
			worldShader.setInt("material.type", 5);
			worldShader.setFloat("material.metallic", metallic);
			worldShader.setFloat("material.roughness", roughness);
			worldShader.setFloat("material.ao", ao);
		}


		//绘制模型
		model.Draw(worldShader);

	}

};


#endif