#ifndef POWER_UP_OBJECT_H
#define POWER_UP_OBJECT_H
#include <string>
#include "gameObject.h"

//���ߴ�С
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
//�����ٶ�
const glm::vec2 VELOCITY(0.0f, 150.0f);


//PowerUp �̳��� GameObject ��״̬��������Ⱦ������
//�������˶������Ϣ��˵��������ʱ���Լ����Ƿ񱻼��
class PowerUp : public GameObject
{
public:
    std::string Type;      //��������
    float       Duration;  //����ʱ��
    bool        Activated; //�Ƿ񼤻����Ч��

    //���캯��
    PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
        : GameObject(position, POWERUP_SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated() { }
};

#endif