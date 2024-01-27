#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../shader.h"
#include "../texture.h"
#include "gameObject.h"

//#include "../debug.h"


//������
struct Particle
{
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float     Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


//�����������䵱һ��������ͨ���������ɺ͸������Ӳ��ڸ���ʱ���������������Ⱦ�������ӡ�
class ParticleGenerator
{
public:
	//���캯��
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
	//��������
	void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	//��������
	void Draw();
private:
	//��������
	std::vector<Particle> particles;
	unsigned int amount;
	//��Ⱦ��
	Shader shader;
	Texture2D texture;
	unsigned int VAO;
	//��ʼ��������Ͷ������ԣ�
	void init();
	//����������Ӳ��ң�����ɣ���
	unsigned int firstUnusedParticle();
	//��������
	void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

//����ʵ��---------------------------------------------------------------------------------

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
	: shader(shader), texture(texture), amount(amount)
{
	this->init();
}

void ParticleGenerator::Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset)
{
	//���������ӣ�ÿ֡newParticles����
	for (unsigned int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], object, offset);
	}
	//�������Ӳ���������ֵ��λ�ã���ɫ�ȣ�
	for (unsigned int i = 0; i < this->amount; ++i)
	{
		Particle& p = this->particles[i];
		p.Life -= dt; //��ʱ���������ֵ
		if (p.Life > 0.0f)
		{	//���»����ӵĲ���
			p.Position -= p.Velocity * dt;       //��С���ƶ�
			p.Color.a -= dt * 4.5f;              //͸����˥��
			if (p.Color.a <= 0)
				p.Life = 0;                      //͸����Ϊ0ֱ��ȥ������
		}
	}
}

// render all particles
void ParticleGenerator::Draw()
{

	//�޸Ļ��ģʽ����"�ⷢ��"Ч��
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
	//�ָ�Ĭ�ϻ��ģʽ
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
	//��䶥�㻺��
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	//���ö�������
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	//����һ�����������Ӷ���
	for (unsigned int i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

unsigned int lastUsedParticle = 0;   //�ϴ�ʹ�õ����ӵ������������������У�ͨ����һ�����ɵ�����֮����Ǳ����������ɵ����ӣ��������ĸ������,���Կ��ٷ�����һ�������ӣ�
unsigned int ParticleGenerator::firstUnusedParticle()
{
	//���ϴ�ʹ�õ����ӿ�ʼ���������������
	for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	//��������ѭ��û�ҵ��������ӣ���ִ����������
	for (unsigned int i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	//��û�оͷ���0����������0��ζ�����Ӷ�����������õĲ�����
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
	//�������������ȵ�
	float random = ((rand() % 100) - 50) / 10.0f;
	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.Position = object.Position + random + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = object.Velocity * 0.1f;
}


#endif