#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObject.h"
#include "../render.h"
#include "../resource_manager.h"


//游戏关卡将所有砖块作为游戏的一部分保存，并托管相关加载/渲染函数。
class GameLevel
{
public:
	//砖块容器
	std::vector<GameObject> Bricks;

	//构造函数
	GameLevel() { }

	//关卡加载器
	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);

	//关卡渲染器
	void Draw(SpriteRenderer& renderer);

	//检测关卡是否通关（所有可破坏砖块均被破坏）
	bool IsCompleted();

private:
	//初始化砖块对象
	void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

//具体实现-----------------------------------------------------------------------

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight)
{
	//清空上一关数据
	this->Bricks.clear();

	//从文件中加载关卡信息(所有的砖块信息)
	unsigned int tileCode;
	GameLevel level;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<unsigned int>> tileData;
	if (fstream)
	{
		while (std::getline(fstream, line)) //读取每行
		{
			std::istringstream sstream(line);
			std::vector<unsigned int> row;
			while (sstream >> tileCode) //以空格为分隔符读取每个数据
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
	//计算每块砖的宽高
	unsigned int height = tileData.size();  //每列砖块数量
	unsigned int width = tileData[0].size();//每行砖块数量
	float unit_width = levelWidth / static_cast<float>(width);
	float unit_height = levelHeight / height;
	
	//砖块初始化
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			if (tileData[y][x] == 1) //实心砖：不可破坏
			{
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = true;
				this->Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1)	//不实心球：可破坏
			{
				glm::vec3 color = glm::vec3(1.0f); // 基础颜色：白
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