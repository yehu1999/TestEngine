#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../shader.h"
#include "../texture.h"
#include "gameObject.h"

//#include "../debug.h"


//粒子类
struct Particle
{
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float     Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


//粒子生成器充当一个容器，通过反复生成和更新粒子并在给定时间后消灭它们来渲染大量粒子。
class ParticleGenerator
{
public:
	//构造函数
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
	//更新粒子
	void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	//绘制粒子
	void Draw();
private:
	//粒子容器
	std::vector<Particle> particles;
	unsigned int amount;
	//渲染器
	Shader shader;
	Texture2D texture;
	unsigned int VAO;
	//初始化（缓冲和顶点属性）
	void init();
	//被消灭的粒子查找（复活吧！）
	unsigned int firstUnusedParticle();
	//粒子再生
	void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

//具体实现---------------------------------------------------------------------------------

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
	: shader(shader), texture(texture), amount(amount)
{
	this->init();
}

void ParticleGenerator::Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset)
{
	//产生新粒子（每帧newParticles个）
	for (unsigned int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], object, offset);
	}
	//更新粒子参数（生命值，位置，颜色等）
	for (unsigned int i = 0; i < this->amount; ++i)
	{
		Particle& p = this->particles[i];
		p.Life -= dt; //按时间减少生命值
		if (p.Life > 0.0f)
		{	//更新活粒子的参数
			p.Position -= p.Velocity * dt;       //随小球移动
			p.Color.a -= dt * 4.5f;              //透明度衰减
			if (p.Color.a <= 0)
				p.Life = 0;                      //透明度为0直接去世（）
		}
	}
}

// render all particles
void ParticleGenerator::Draw()
{

	//修改混合模式产生"外发光"效果
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.use();
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			this->shader.setVec2("offset", particle.Position);
			this->shader.setVec4("color", particle.Color);
			this->texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	//恢复默认混合模式
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
	unsigned int VBO;
	float particle_quad[] =
	{
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	//填充顶点缓冲
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	//设置顶点属性
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	//产生一定数量的粒子对象
	for (unsigned int i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

unsigned int lastUsedParticle = 0;   //上次使用的粒子的索引（在粒子容器中，通常上一次生成的粒子之后便是本轮最先生成的粒子，其已死的概率最大,可以快速访问下一个死粒子）
unsigned int ParticleGenerator::firstUnusedParticle()
{
	//从上次使用的粒子开始向后搜索死亡粒子
	for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	//如果上面的循环没找到死亡粒子，则执行线性搜索
	for (unsigned int i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	//还没有就返回0（经常返回0意味着粒子对象的数量设置的不够）
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
	//重置粒子生命等等
	float random = ((rand() % 100) - 50) / 10.0f;
	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.Position = object.Position + random + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = object.Velocity * 0.1f;
}


#endif