#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObject.h"
#include "../texture.h"

//BallObject��GameObject�̳���ص�״̬����
//�����һЩ�ض��������Ķ��⹦�ܣ�
class BallObject : public GameObject
{
public:
    //С��״̬
    float   Radius;     //�뾶
    bool    Stuck;      //�Ƿ�̶�
    bool    Sticky;     //���
    bool    PassThrough;//�ᴩ��

    //���캯��
    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);
    
    //�˶�����
    glm::vec2 Move(float dt, unsigned int window_width);

    //����С��״̬
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};

//����ʵ��----------------------------------------------------------------------------------------------------

BallObject::BallObject()
    : GameObject(), Radius(12.5f), Stuck(true), Sticky(false), PassThrough(false) { }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true), Sticky(false), PassThrough(false) { }

glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
    //���С���ٹ̶��ڵ�����
    if (!this->Stuck)
    {
        //�˶�
        this->Position += this->Velocity * dt;
        
        //��������߽磬��ת�ٶȣ������̽������ر߽�����
        if (this->Position.x <= 0.0f)
        {
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = 0.0f;
        }
        else if (this->Position.x + this->Size.x >= window_width)
        {
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = window_width - this->Size.x;
        }
        if (this->Position.y <= 0.0f)
        {
            this->Velocity.y = -this->Velocity.y;
            this->Position.y = 0.0f;
        }
    }
    return this->Position;
}

//С������
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    this->Position = position;
    this->Velocity = velocity;
    this->Stuck = true;
    this->Sticky = false;
    this->PassThrough = false;
}




#endif