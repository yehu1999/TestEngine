#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObject.h"
#include "../texture.h"

//BallObject从GameObject继承相关的状态数据
//并添加一些特定于球对象的额外功能，
class BallObject : public GameObject
{
public:
    //小球状态
    float   Radius;     //半径
    bool    Stuck;      //是否固定
    bool    Sticky;     //黏性
    bool    PassThrough;//贯穿性

    //构造函数
    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);
    
    //运动函数
    glm::vec2 Move(float dt, unsigned int window_width);

    //重置小球状态
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};

//具体实现----------------------------------------------------------------------------------------------------

BallObject::BallObject()
    : GameObject(), Radius(12.5f), Stuck(true), Sticky(false), PassThrough(false) { }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true), Sticky(false), PassThrough(false) { }

glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
    //如果小球不再固定在挡板上
    if (!this->Stuck)
    {
        //运动
        this->Position += this->Velocity * dt;
        
        //如果超出边界，则反转速度，并立刻将其拉回边界以内
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

//小球重置
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    this->Position = position;
    this->Velocity = velocity;
    this->Stuck = true;
    this->Sticky = false;
    this->PassThrough = false;
}




#endif