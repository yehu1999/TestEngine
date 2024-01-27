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

//相机属性默认值
const glm::vec3 FRONT(0.0f, 0.0f, -1.0f);//初次改变视角方向会自动会归这个方向，但暂时无法自定义，因为yaw和pitch在计算front时就是默认初始方向为此方向
const float YAW = -90.0f;        //偏航角
const float PITCH = 0.0f;        //俯仰角
const float SPEED = 2.5f;        //速度
const float SENSITIVITY = 0.05f; //灵敏度
const float ZOOM = 45.0f;        //缩放(fov)

class Camera
{
public:
	//相机属性
	glm::vec3 Position;  //相机世界位置
	glm::vec3 Front;     //相机视线方向(-z轴
	glm::vec3 Up;        //相机顶方向(y轴
	glm::vec3 Right;     //相机正右方向(x轴
	glm::vec3 WorldUp;   //世界竖直向上方向(用于根据一个已知方向计算其他两个方向)
	// euler Angles
	float Yaw;           //偏航角
	float Pitch;         //俯仰角
	// camera options
	float MovementSpeed;   //速度
	float MouseSensitivity;//灵敏度
	float Zoom;            //缩放(fov)

	float Near, Far;       //近视距,远视距

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float n = 0.01f, float f = 100.0f);

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	glm::mat4 GetViewMatrix();              //获取View矩阵
	glm::mat4 GetPerspectiveMatrix(int SCR_WIDTH, int SCR_HEIGHT);       //获取Perspective矩阵

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
		//通过俯仰角和偏航角来计算以得到新的相机视线方向向量：
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		//根据新的相机视线方向向量更新其他两个相机向量
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors,不然会影响移动速度
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

//具体实现--------------------------------------------------------------------------------------

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
	float velocity = MovementSpeed * deltaTime;//将每帧间隔时间与相机速度结合，保证不同设备移动速度也相同

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

	//限制俯仰角极限   
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
	//通过改变视场角来产生缩放的效果
	//视野(Field of View)或fov定义了我们可以看到场景中多大的范围。
	//当视野变小时，场景投影出来的空间就会减小，产生放大(Zoom In)了的感觉
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= (float)yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;

}

#endif