#ifndef LIGHT_H
#define LIGHT_H

//��Դ����
enum LightType
{
	POINT_LIGHT = 0,
	DIR_LIGHT = 1,
	SPOT_LIGHT = 2,
};
//��Դ����
struct LightData
{
	int type;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
	float Radius;
};

//��Դ����
class Light
{
public:
	LightType Type;
	std::string Name;
	glm::vec3 Color, Specular; //��ɫ ,�����⣬�����
	float Constant, Linear, Quadratic;  //����˥�����ӣ���������������������
	float Radius;                       //������뾶

	LightData data;                     //����Ϣ

	//���캯��
	Light()
	{
		Color = { 1.0f,1.0f,1.0f };
		Specular = { 1.0f,1.0f,1.0f };

		Constant = 1.0f;
		Linear = 0.09f;
		Quadratic = 0.032f;

		Radius = 100;
	}
	Light(Camera& camera)
	{
		Color = { 1.0f,1.0f,1.0f };
		Specular = { 1.0f,1.0f,1.0f };

		Constant = 1.0f;
		Linear = 0.09f;
		Quadratic = 0.032f;

		Radius = 100;
	};
	//��������
	~Light() {}

	virtual void Draw() {};          //��Դ��Ϣ�����Դ��ɫ��

	virtual LightData GetData() { return LightData(); };  //���ع�Դ��Ϣ
};

//���Դ����
class PointLight :public Light
{
public:
	glm::vec3   Position, Size, RotateAngle;  //λ�ã� ����ߣ�������ת�Ƕ�
	glm::vec3   Velocity;                     //�ٶ�

	//���캯��
	PointLight() :Light()
	{
		Type = POINT_LIGHT;
		Name = "newPointLight";
		Position = { 0.0f,0.0f,0.0f };
		Size = { 0.2f,0.2f,0.2f };
		RotateAngle = { 0.0f,0.0f,0.0f };
		Velocity = { 0.0f,0.0f,0.0f };
	}
	PointLight(Camera& camera) :Light()
	{
		Type = POINT_LIGHT;
		Name = "newPointLight";
		Position = camera.Position + camera.Front;
		Size = { 0.2f,0.2f,0.2f };
		RotateAngle = { 0.0f,0.0f,0.0f };
		Velocity = { 0.0f,0.0f,0.0f };
	}
	//��������
	~PointLight() {}

	//����
	void Draw() override
	{
		Shader lightShader = ResourceManager::GetShader("LightShader");
		lightShader.use();

		//�ϴ�ģ�;���
		glm::mat4 ModelMat = glm::mat4(1.0f);
		Position += Velocity;
		ModelMat = glm::translate(ModelMat, Position);
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMat = glm::scale(ModelMat, Size);
		lightShader.setMat4("model", ModelMat);

		//�ϴ���ɫ
		lightShader.setVec3("lightColor", Color);

		renderLight();
	}

	//������Ϣ
	LightData GetData()  override
	{
		data.type = 0;
		data.position = Position;
		data.direction = { 0,0,0 };
		data.diffuse = Color;
		data.specular = Specular;
		data.constant = Constant;
		data.linear = Linear;
		data.quadratic = Quadratic;
		data.cutOff = 100;
		data.outerCutOff = 100;
		data.Radius = 100;

		return data;
	}
};

//���������
class DirLight :public PointLight
{
public:
	glm::vec3 Direction;       //���䷽��

	//���캯��
	DirLight() :PointLight()
	{
		Type = DIR_LIGHT;
		Name = "newDirLight";
		Direction = { 0.0f,0.0f,-1.0f };
		Constant = 1.0f;
		Linear = 0.0000001f;
		Quadratic = 0.0000001f;
	}
	DirLight(Camera& camera) :PointLight(camera)
	{
		Type = DIR_LIGHT;
		Name = "newDirLight";
		Direction = camera.Front;
		Constant = 1.0f;
		Linear = 0.0000001f;
		Quadratic = 0.0000001f;
	}
	//��������
	~DirLight() {}

	//����
	void Draw() override
	{
		Shader lightShader = ResourceManager::GetShader("LightShader");
		lightShader.use();

		//�ϴ�ģ�;���
		glm::mat4 ModelMat = glm::mat4(1.0f);
		Position += Velocity;
		ModelMat = glm::translate(ModelMat, Position);
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMat = glm::scale(ModelMat, Size);
		lightShader.setMat4("model", ModelMat);

		//�ϴ���ɫ
		lightShader.setVec3("lightColor", Color);

		UpdateDir();

		renderLight();
	}

	//���¹�����
	void UpdateDir()
	{
		//���¹�����
		glm::mat4 RotateMat = glm::mat4(1.0f);
		RotateMat = glm::rotate(RotateMat, glm::radians(RotateAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		RotateMat = glm::rotate(RotateMat, glm::radians(RotateAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		RotateMat = glm::rotate(RotateMat, glm::radians(RotateAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		Direction = glm::vec3(RotateMat * glm::vec4(glm::vec3(0, 0, -1), 1.0f));
	}

	//������Ϣ
	LightData GetData()  override
	{
		data.type = 1;
		data.position = Position;
		data.direction = Direction;

		data.diffuse = Color;
		data.specular = Specular;
		data.constant = Constant;
		data.linear = Linear;
		data.quadratic = Quadratic;
		data.cutOff = 100;
		data.outerCutOff = 100;
		data.Radius = 100;

		return data;
	}
};

//��Ȧ������
class SpotLight :public DirLight
{
public:
	float CutOff, OuterCutOff;   //��Ȧ�ڱ�Ե����Ȧ���Ե������֮��Ϊ������������

	//���캯��
	SpotLight() :DirLight()
	{
		Type = SPOT_LIGHT;
		Name = "newSpotLight";
		CutOff = 12.5f;
		OuterCutOff = 15.0f;
	}
	SpotLight(Camera& camera) :DirLight(camera)
	{
		Type = SPOT_LIGHT;
		Name = "newSpotLight";
		CutOff = 12.5f;
		OuterCutOff = 15.0f;
	}
	//��������
	~SpotLight() {}

	//����
	void Draw()  override
	{
		Shader lightShader = ResourceManager::GetShader("LightShader");
		lightShader.use();

		//�ϴ�ģ�;���
		glm::mat4 ModelMat = glm::mat4(1.0f);
		Position += Velocity;
		ModelMat = glm::translate(ModelMat, Position);
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMat = glm::scale(ModelMat, Size);
		lightShader.setMat4("model", ModelMat);

		//�ϴ���ɫ
		lightShader.setVec3("lightColor", Color);

		renderLight();
	}

	//������Ϣ
	LightData GetData()  override
	{
		data.type = 2;
		data.position = Position;
		data.direction = Direction;
		data.diffuse = Color;
		data.specular = Specular;
		data.constant = Constant;
		data.linear = Linear;
		data.quadratic = Quadratic;
		data.cutOff = CutOff;
		data.outerCutOff = OuterCutOff;
		data.Radius = 100;

		return data;
	}

};

#endif