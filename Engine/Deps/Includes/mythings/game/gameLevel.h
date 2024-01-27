#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObject.h"
#include "../render.h"
#include "../resource_manager.h"


//��Ϸ�ؿ�������ש����Ϊ��Ϸ��һ���ֱ��棬���й���ؼ���/��Ⱦ������
class GameLevel
{
public:
	//ש������
	std::vector<GameObject> Bricks;

	//���캯��
	GameLevel() { }

	//�ؿ�������
	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);

	//�ؿ���Ⱦ��
	void Draw(SpriteRenderer& renderer);

	//���ؿ��Ƿ�ͨ�أ����п��ƻ�ש������ƻ���
	bool IsCompleted();

private:
	//��ʼ��ש�����
	void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

//����ʵ��-----------------------------------------------------------------------

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight)
{
	//�����һ������
	this->Bricks.clear();

	//���ļ��м��عؿ���Ϣ(���е�ש����Ϣ)
	unsigned int tileCode;
	GameLevel level;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<unsigned int>> tileData;
	if (fstream)
	{
		while (std::getline(fstream, line)) //��ȡÿ��
		{
			std::istringstream sstream(line);
			std::vector<unsigned int> row;
			while (sstream >> tileCode) //�Կո�Ϊ�ָ�����ȡÿ������
				row.push_back(tileCode);
			tileData.push_back(row);
		}
		if (tileData.size() > 0)
			this->init(tileData, levelWidth, levelHeight);
	}
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
	for (GameObject& tile : this->Bricks)
		if (!tile.Destroyed)
			tile.Draw(renderer);
}

bool GameLevel::IsCompleted()
{
	for (GameObject& tile : this->Bricks)
		if (!tile.IsSolid && !tile.Destroyed)
			return false;
	return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
	//����ÿ��ש�Ŀ��
	unsigned int height = tileData.size();  //ÿ��ש������
	unsigned int width = tileData[0].size();//ÿ��ש������
	float unit_width = levelWidth / static_cast<float>(width);
	float unit_height = levelHeight / height;
	
	//ש���ʼ��
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			if (tileData[y][x] == 1) //ʵ��ש�������ƻ�
			{
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = true;
				this->Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1)	//��ʵ���򣺿��ƻ�
			{
				glm::vec3 color = glm::vec3(1.0f); // ������ɫ����
				if (tileData[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);

				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
			}
		}
	}
}


#endif