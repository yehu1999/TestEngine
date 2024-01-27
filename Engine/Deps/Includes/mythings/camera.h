#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum  Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//�������Ĭ��ֵ
const glm::vec3 FRONT(0.0f, 0.0f, -1.0f);//���θı��ӽǷ�����Զ����������򣬵���ʱ�޷��Զ��壬��Ϊyaw��pitch�ڼ���frontʱ����Ĭ�ϳ�ʼ����Ϊ�˷���
const float YAW = -90.0f;        //ƫ����
const float PITCH = 0.0f;        //������
const float SPEED = 2.5f;        //�ٶ�
const float SENSITIVITY = 0.05f; //������
const float ZOOM = 45.0f;        //����(fov)

class Camera
{
public:
	//�������
	glm::vec3 Position;  //�������λ��
	glm::vec3 Front;     //������߷���(-z��
	glm::vec3 Up;        //���������(y��
	glm::vec3 Right;     //������ҷ���(x��
	glm::vec3 WorldUp;   //������ֱ���Ϸ���(���ڸ���һ����֪�������������������)
	// euler Angles
	float Yaw;           //ƫ����
	float Pitch;         //������
	// camera options
	float MovementSpeed;   //�ٶ�
	float MouseSensitivity;//������
	float Zoom;            //����(fov)

	float Near, Far;       //���Ӿ�,Զ�Ӿ�

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float n = 0.01f, float f = 100.0f);

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	glm::mat4 GetViewMatrix();              //��ȡView����
	glm::mat4 GetPerspectiveMatrix(int SCR_WIDTH, int SCR_HEIGHT);       //��ȡPerspective����

	//Controller
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

	inline void report()
	{
		std::cout << "(" << Position.x << "," << Position.y << "," << Position.z << ")";
		std::cout << "(" << Front.x << "," << Front.y << "," << Front.z << ")" << std::endl;
	}

private:
	void updateCameraVectors()
	{
		//ͨ�������Ǻ�ƫ�����������Եõ��µ�������߷���������
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		//�����µ�������߷��������������������������
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors,��Ȼ��Ӱ���ƶ��ٶ�
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

//����ʵ��--------------------------------------------------------------------------------------

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float n, float f)
	: Front(FRONT), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	Near = n;
	Far = f;

	updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: Front(FRONT), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetPerspectiveMatrix(int SCR_WIDTH, int SCR_HEIGHT)
{
	return glm::perspective(glm::radians(Zoom), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, Near, Far);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;//��ÿ֡���ʱ��������ٶȽ�ϣ���֤��ͬ�豸�ƶ��ٶ�Ҳ��ͬ

	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	//���Ƹ����Ǽ���   
	if (constrainPitch)
	{
		if (Pitch > 89.9f)
			Pitch = 89.9f;
		if (Pitch < -89.9f)
			Pitch = -89.9f;
	}

	// update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	//ͨ���ı��ӳ������������ŵ�Ч��
	//��Ұ(Field of View)��fov���������ǿ��Կ��������ж��ķ�Χ��
	//����Ұ��Сʱ������ͶӰ�����Ŀռ�ͻ��С�������Ŵ�(Zoom In)�˵ĸо�
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= (float)yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;

}

#endif