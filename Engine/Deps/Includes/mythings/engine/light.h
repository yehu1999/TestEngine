#ifndef LIGHT_H
#define LIGHT_H

//光源种类
enum LightType
{
	POINT_LIGHT = 0,
	DIR_LIGHT = 1,
	SPOT_LIGHT = 2,
};
//光源数据
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

//光源父类
class Light
{
public:
	LightType Type;
	std::string Name;
	glm::vec3 Color, Specular; //光色 ,环境光，镜面光
	float Constant, Linear, Quadratic;  //光线衰减因子：常量，线性量，二次量
	float Radius;                       //光体积半径

	LightData data;                     //光信息

	//构造函数
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
	//析构函数
	~Light() {}

	virtual void Draw() {};          //光源信息传入光源着色器

	virtual LightData GetData() { return LightData(); };  //返回光源信息
};

//点光源子类
class PointLight :public Light
{
public:
	glm::vec3   Position, Size, RotateAngle;  //位置， 长宽高，三轴旋转角度
	glm::vec3   Velocity;                     //速度

	//构造函数
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
	//析构函数
	~PointLight() {}

	//绘制
	void Draw() override
	{
		Shader lightShader = ResourceManager::GetShader("LightShader");
		lightShader.use();

		//上传模型矩阵
		glm::mat4 ModelMat = glm::mat4(1.0f);
		Position += Velocity;
		ModelMat = glm::translate(ModelMat, Position);
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMat = glm::scale(ModelMat, Size);
		lightShader.setMat4("model", ModelMat);

		//上传光色
		lightShader.setVec3("lightColor", Color);

		renderLight();
	}

	//返回信息
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

//有向光子类
class DirLight :public PointLight
{
public:
	glm::vec3 Direction;       //照射方向

	//构造函数
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
	//析构函数
	~DirLight() {}

	//绘制
	void Draw() override
	{
		Shader lightShader = ResourceManager::GetShader("LightShader");
		lightShader.use();

		//上传模型矩阵
		glm::mat4 ModelMat = glm::mat4(1.0f);
		Position += Velocity;
		ModelMat = glm::translate(ModelMat, Position);
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMat = glm::scale(ModelMat, Size);
		lightShader.setMat4("model", ModelMat);

		//上传光色
		lightShader.setVec3("lightColor", Color);

		UpdateDir();

		renderLight();
	}

	//更新光向量
	void UpdateDir()
	{
		//更新光向量
		glm::mat4 RotateMat = glm::mat4(1.0f);
		RotateMat = glm::rotate(RotateMat, glm::radians(RotateAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		RotateMat = glm::rotate(RotateMat, glm::radians(RotateAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		RotateMat = glm::rotate(RotateMat, glm::radians(RotateAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		Direction = glm::vec3(RotateMat * glm::vec4(glm::vec3(0, 0, -1), 1.0f));
	}

	//返回信息
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

//光圈子子类
class SpotLight :public DirLight
{
public:
	float CutOff, OuterCutOff;   //光圈内边缘，光圈外边缘（两者之间为明暗渐变区域）

	//构造函数
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
	//析构函数
	~SpotLight() {}

	//绘制
	void Draw()  override
	{
		Shader lightShader = ResourceManager::GetShader("LightShader");
		lightShader.use();

		//上传模型矩阵
		glm::mat4 ModelMat = glm::mat4(1.0f);
		Position += Velocity;
		ModelMat = glm::translate(ModelMat, Position);
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMat = glm::rotate(ModelMat, glm::radians(RotateAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMat = glm::scale(ModelMat, Size);
		lightShader.setMat4("model", ModelMat);

		//上传光色
		lightShader.setVec3("lightColor", Color);

		renderLight();
	}

	//返回信息
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