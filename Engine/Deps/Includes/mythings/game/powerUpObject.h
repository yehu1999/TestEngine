#ifndef POWER_UP_OBJECT_H
#define POWER_UP_OBJECT_H
#include <string>
#include "gameObject.h"

//道具大小
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
//掉落速度
const glm::vec2 VELOCITY(0.0f, 150.0f);


//PowerUp 继承了 GameObject 的状态参数和渲染函数，
//还保留了额外的信息来说明其活动持续时间以及它是否被激活。
class PowerUp : public GameObject
{
public:
    std::string Type;      //道具种类
    float       Duration;  //持续时间
    bool        Activated; //是否激活道具效果

    //构造函数
    PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
        : GameObject(position, POWERUP_SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated() { }
};

#endif