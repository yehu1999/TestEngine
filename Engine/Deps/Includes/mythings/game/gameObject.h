#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../texture.h"
#include "../render.h"


//�����������ڱ����뵥����Ϸ����ʵ����ص�����״̬��
class GameObject
{
public:
    // object state
    glm::vec2   Position, Size, Velocity;  //λ�ã� ��ߣ� �ٶ�
    glm::vec3   Color;                     //��ɫ
    float       Rotation;                  //��ת��
    bool        IsSolid;                   //�Ƿ񲻿ɴݻ�
    bool        Destroyed;                 //�Ƿ񱻴ݻ�
    
    //��Ⱦ���飨����
    Texture2D   Sprite;
    
    //���캯��
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    
    //���ƺ���
    virtual void Draw(SpriteRenderer& renderer);
};

//����ʵ��-----------------------------------------------------------------------
GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

#endif