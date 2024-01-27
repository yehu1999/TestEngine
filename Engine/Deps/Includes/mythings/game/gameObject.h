#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../texture.h"
#include "../render.h"


//容器对象，用于保存与单个游戏对象实体相关的所有状态。
class GameObject
{
public:
    // object state
    glm::vec2   Position, Size, Velocity;  //位置， 宽高， 速度
    glm::vec3   Color;                     //颜色
    float       Rotation;                  //旋转角
    bool        IsSolid;                   //是否不可摧毁
    bool        Destroyed;                 //是否被摧毁
    
    //渲染精灵（纹理）
    Texture2D   Sprite;
    
    //构造函数
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    
    //绘制函数
    virtual void Draw(SpriteRenderer& renderer);
};

//具体实现-----------------------------------------------------------------------
GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

#endif