#ifndef GAME_H
#define GAME_H

#include <algorithm>

#include<irrklang/irrKlang.h>
using namespace irrklang;

#include"../resource_manager.h"
#include"../render.h"
#include"textRenderer.h"
#include"gameLevel.h"
#include"gameObject.h"
#include"ballObject.h"
#include"powerUpObject.h"
#include"particle.h"
#include"postProcessor.h"
#include<string>
#include<tuple>


//游戏当前状态
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

//四方向
enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};

//碰撞元组数据<是否发生碰撞，碰撞方向，偏移差向量>  (差向量：球心到砖块碰撞面最近点的向量)
typedef std::tuple<bool, Direction, glm::vec2> Collision;

//玩家挡板初始大小
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
//玩家挡板初始速度
const float PLAYER_VELOCITY(500.0f);
//小球初始速度
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
//小球半径
const float BALL_RADIUS = 12.5f;
//震动效果时间
GLfloat ShakeTime = 0.0f;

//非成员函数
bool CheckCollision_Box(GameObject& one, GameObject& two); //BOXtoBOX碰撞检测
bool CheckCollision_(BallObject& one, GameObject& two);    //BALLtoBOX碰撞检测
Collision CheckCollision(BallObject& one, GameObject& two);//BALLtoBOX碰撞检测（返回碰撞信息）
Direction VectorDirection(glm::vec2 closest);              //方向指向确认 （确认一个二维向量最偏向的那个正方向，即往哪？往上/下/左/右）
float clamp(float value, float min, float max);            //限制函数
bool ShouldSpawn(unsigned int chance);                     //道具随机复活函数
void ActivatePowerUp(PowerUp& powerUp);                    //激活道具
bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type); //是否还有其他同类被激活的道具

//游戏类包含游戏相关的状态和功能
//将所有游戏相关的数据合并到一个类中，便于组件的访问和管理
class Game
{
public:
	GameState               State;              //游戏状态
	bool                    Keys[1024];         //键盘输入
	bool                    KeysProcessed[1024];//键盘按下 （当相对应的按键被处理时，我们将其设置为true ，以确保按键只在之前没有被处理过（直到松开）时将其处理。）
	unsigned int            Width, Height;      //场景大小
	std::vector<GameLevel>  Levels;             //关卡容器
	std::vector<PowerUp>  PowerUps;             //道具容器
	unsigned int            Level;              //当前关卡
	unsigned int            Lives;              //生命值

	//构造函数/析构函数
	Game(unsigned int width, unsigned int height);
	~Game();

	//初始化游戏状态 (加载所有着色器/纹理/关卡)
	void Init();

	//游戏循环相关函数
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();

	//碰撞函数
	void DoCollisions();

	//重置函数
	void ResetLevel();
	void ResetPlayer();

	//道具函数
	void SpawnPowerUps(GameObject& block);
	void UpdatePowerUps(float dt);
};


//具体实现--------------------------------------------------------------------------------------

//游戏相关对象
SpriteRenderer* Renderer;      //渲染精灵
GameObject* Player;            //玩家挡板
BallObject* Ball;              //小球
ParticleGenerator* Particles;  //粒子制造器
PostProcessor* Effects;        //后处理特效生成器
ISoundEngine* SoundEngine = createIrrKlangDevice();  //音频播放器
TextRenderer* Text;            //文本渲染器

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Level(0), Lives(3) {}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
	delete Text;
	SoundEngine->drop();
}

void Game::Init()
{
	// 加载着色器 
	ResourceManager::LoadShader("../../Deps/ShaderFiles/sprite.vs", "../../Deps/ShaderFiles/sprite.frag", nullptr, "sprite");
	ResourceManager::LoadShader("../../Deps/ShaderFiles/particle.vs", "../../Deps/ShaderFiles/particle.frag", nullptr, "particle");
	ResourceManager::LoadShader("../../Deps/ShaderFiles/postprocess.vs", "../../Deps/ShaderFiles/postprocess.frag", nullptr, "postprocessing");

	// 配置着色器
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").use().setInt("image", 0);
	ResourceManager::GetShader("sprite").setMat4("projection", projection);
	ResourceManager::GetShader("particle").use().setInt("sprite", 0);
	ResourceManager::GetShader("particle").setMat4("projection", projection);

	// 加载纹理 （放在ResourceManager::GetTexture()前面）
	ResourceManager::LoadTexture("../../Resources/textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("../../Resources/textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("../../Resources/textures/block.png", false, "block");
	ResourceManager::LoadTexture("../../Resources/textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("../../Resources/textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("../../Resources/textures/particle.png", true, "particle");
	ResourceManager::LoadTexture("../../Resources/textures/powerup_speed.png", true, "powerup_speed");
	ResourceManager::LoadTexture("../../Resources/textures/powerup_sticky.png", true, "powerup_sticky");
	ResourceManager::LoadTexture("../../Resources/textures/powerup_increase.png", true, "powerup_increase");
	ResourceManager::LoadTexture("../../Resources/textures/powerup_confuse.png", true, "powerup_confuse");
	ResourceManager::LoadTexture("../../Resources/textures/powerup_chaos.png", true, "powerup_chaos");
	ResourceManager::LoadTexture("../../Resources/textures/powerup_passthrough.png", true, "powerup_passthrough");

	// 设置渲染控制
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 200);
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("../../Resources/fonts/OCRAEXT.TTF", 24);

	// 加载关卡
	GameLevel one; one.Load("../../Resources/levels/one.lvl", this->Width, this->Height / 2);
	GameLevel two; two.Load("../../Resources/levels/two.lvl", this->Width, this->Height / 2);
	GameLevel three; three.Load("../../Resources/levels/three.lvl", this->Width, this->Height / 2);
	GameLevel four; four.Load("../../Resources/levels/four.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;

	//设置游戏物体初始状态
	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

	//背景音乐（循环播放）
	SoundEngine->play2D("../../Resources/audio/breakout.mp3", true);
}

void Game::Update(float dt)
{
	//小球运动
	Ball->Move(dt, this->Width);

	//碰撞检测
	this->DoCollisions();

	//粒子效果
	Particles->Update(dt, *Ball, 1, glm::vec2(Ball->Radius / 2.0f));

	//减少Shake时间
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = GL_FALSE;
	}

	//道具效果
	this->UpdatePowerUps(dt);

	if (Levels[Level].IsCompleted())
		Level++;
	if (Level >= Levels.size())
		Level = 0;

	//检测失败条件
	if (Ball->Position.y >= this->Height) //小球触底减生命值
	{
		--this->Lives;

		if (this->Lives == 0)
		{
			this->ResetLevel();
			this->State = GAME_MENU;
		}
		this->ResetPlayer();
	}
	//检测胜利条件
	if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
	{
		this->ResetLevel();
		this->ResetPlayer();
		Effects->Chaos = true;
		this->State = GAME_WIN;
	}
}

void Game::ProcessInput(float dt)
{
	//菜单
	if (this->State == GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
		{
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = true;
		}
		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
		{
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = 3;
			this->KeysProcessed[GLFW_KEY_S] = true;
		}
	}
	//胜利
	if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_ENTER])
		{
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
			Effects->Chaos = false;
			this->State = GAME_MENU;
		}
	}
	//游戏中
	if (this->State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;

		//挡板移动
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)       //小球固定时也跟随挡板移动
					Ball->Position.x -= velocity;
			}


		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}
		}
		//解锁小球
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
	}

}

void Game::Render()
{
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
	{
		//DebugRenderer
		//Renderer->DrawSprite(ResourceManager::GetTexture("particle"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);

		//开始渲染后处理特效（帧缓冲）
		Effects->BeginRender();
		//---------------------------------------

		//绘制背景
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
		//绘制关卡
		this->Levels[this->Level].Draw(*Renderer);
		//绘制玩家
		Player->Draw(*Renderer);
		//绘制粒子
		Particles->Draw();
		//绘制小球
		Ball->Draw(*Renderer);
		//绘制道具 （绘制道具容器中所有未摧毁的道具）
		for (PowerUp& powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		//绘制文本
		std::stringstream ss; ss << this->Lives;
		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);

		//---------------------------------------
		//结束渲染后处理特效
		Effects->EndRender();
		//绘制后处理后的帧缓冲
		Effects->Render(glfwGetTime());
	}
	if (this->State == GAME_MENU)
	{
		Text->RenderText("Press ENTER to start", 250.0f, this->Height / 2.0f, 1.0f);
		Text->RenderText("Press W or S to select level", 245.0f, this->Height / 2.0f + 20.0f, 0.75f);
	}
	if (this->State == GAME_WIN)
	{
		Text->RenderText("You WON!!!", 320.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
	}
}

void Game::ResetLevel()
{
	if (this->Level == 0)
		this->Levels[0].Load("../../Resources/levels/one.lvl", this->Width, this->Height / 2);
	else if (this->Level == 1)
		this->Levels[1].Load("../../Resources/levels/two.lvl", this->Width, this->Height / 2);
	else if (this->Level == 2)
		this->Levels[2].Load("../../Resources/levels/three.lvl", this->Width, this->Height / 2);
	else if (this->Level == 3)
		this->Levels[3].Load("../../Resources/levels/four.lvl", this->Width, this->Height / 2);

	this->Lives = 3;
}

void Game::ResetPlayer()
{
	// reset player/ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
	// also disable all active powerups
	Effects->Chaos = Effects->Confuse = false;
	Ball->PassThrough = Ball->Sticky = false;
	Player->Color = glm::vec3(1.0f);
	Ball->Color = glm::vec3(1.0f);
}

void Game::SpawnPowerUps(GameObject& block)
{
	//probability: 1/N

	//buff
	if (ShouldSpawn(35))
		this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
	if (ShouldSpawn(35))
		this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
	if (ShouldSpawn(35))
		this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	if (ShouldSpawn(25))
		this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
	//debuff
	if (ShouldSpawn(55))
		this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
	if (ShouldSpawn(35))
		this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

void Game::UpdatePowerUps(float dt)
{
	for (PowerUp& powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt; //随时间减少道具持续时间

			if (powerUp.Duration <= 0.0f)
			{
				//停用过期道具效果（之后若未被重新激活启用则将被销毁）
				powerUp.Activated = false;

				//当没有其他被激活的同类道具时，就将其效果还原为默认值
				if (powerUp.Type == "sticky")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
					{
						Ball->Sticky = false;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
					{
						Ball->PassThrough = false;
						Ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
					{
						Effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
					{
						Effects->Chaos = false;
					}
				}
			}
		}
	}

	//如果道具已被摧毁，则从容器中删除该道具对象
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}

void Game::DoCollisions()
{
	//检测小球与砖块间碰撞（已被摧毁的砖块不检测）
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			//获取碰撞信息
			Collision collision = CheckCollision(*Ball, box);

			if (std::get<0>(collision)) //碰撞为true
			{
				//撞到摧毁可摧毁的砖块
				if (!box.IsSolid)
				{
					box.Destroyed = true;       //销毁该砖块
					this->SpawnPowerUps(box);   //随机生成道具
					SoundEngine->play2D("../../Resources/audio/bleep.mp3", false);//播放音效
				}

				else
				{
					//撞到不摧毁可摧毁的砖块，产生Shake效果
					ShakeTime = 0.05f;
					Effects->Shake = true;     //震动效果
					SoundEngine->play2D("../../Resources/audio/solid.wav", false); //播放音效
				}

				//碰撞处理（根据碰撞信息，改变小球回弹运动状态）
				Direction dir = std::get<1>(collision);              //碰撞方向
				glm::vec2 diff_vector = std::get<2>(collision);      //偏移差向量

				if (!(Ball->PassThrough && !box.IsSolid))     //当小球具有贯穿性并且撞到可摧毁砖块时不做碰撞处理
				{
					if (dir == LEFT || dir == RIGHT) //平撞
					{
						Ball->Velocity.x = -Ball->Velocity.x; //翻转小球水平速度分量
						//重定位（将小球从砖块内拽回来）
						float penetration = Ball->Radius - std::abs(diff_vector.x);
						if (dir == LEFT)
							Ball->Position.x += penetration; //往左撞则向右回正
						else
							Ball->Position.x -= penetration; //往右撞则向左回正
					}
					else //竖直撞
					{
						Ball->Velocity.y = -Ball->Velocity.y; //翻转小球竖直速度分量
						//重定位（将小球从砖块内拽回来）
						float penetration = Ball->Radius - std::abs(diff_vector.y);
						if (dir == UP)
							Ball->Position.y -= penetration; //往上撞则向下回正
						else
							Ball->Position.y += penetration; //往下撞则向上回正
					}
				}
			}
		}
	}

	//检测玩家挡板与小球碰撞 (挡板固定时不检测)
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		//根据小球与挡板的碰撞点改变小球速度
		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		float percentage = distance / (Player->Size.x / 2.0f);

		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); //使用储存的oldVelocity将改变后的小球速度统一，保证速度不变，只改变方向

		//Ball->Velocity.y = -Ball->Velocity.y;        //不使用竖直速度分量翻转，因为有粘板问题
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y); //粘板问题修复:当挡板移动过快，小球中心进入挡板内部，发生多次碰撞检测导致多次y轴反转。所以干脆碰撞就只往上弹得了。

		//如果小球具有黏性，则粘在挡板上
		Ball->Stuck = Ball->Sticky;

		//播放音效
		SoundEngine->play2D("../../Resources/audio/bleep.wav", false);
	}

	//检测玩家挡板与道具间碰撞
	for (PowerUp& powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = true;
			if (CheckCollision_Box(*Player, powerUp))
			{
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = true;  //被吃掉的道具被“摧毁”（不再绘制，不再参与碰撞）
				powerUp.Activated = true;  //激活其道具效果，且暂时不被道具容器删除
				SoundEngine->play2D("../../Resources/audio/powerup.wav", false);//播放音效
			}
		}
	}
}

//非成员函数
float clamp(float value, float min, float max)
{
	return std::max(min, std::min(max, value));
}

bool CheckCollision_Box(GameObject& one, GameObject& two) // AABB - AABB collision
{
	// collision x-axis?
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// collision y-axis?
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// collision only if on both axes
	return collisionX && collisionY;
}

bool CheckCollision_(BallObject& one, GameObject& two) // AABB - Circle collision
{
	// get center point circle first 
	glm::vec2 center(one.Position + one.Radius);
	// calculate AABB info (center, half-extents)
	glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(
		two.Position.x + aabb_half_extents.x,
		two.Position.y + aabb_half_extents.y
	);
	// get difference vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	// retrieve vector between center circle and closest point AABB and check if length <= radius
	difference = closest - center;
	return glm::length(difference) < one.Radius;
}

Collision CheckCollision(BallObject& one, GameObject& two) // AABB - Circle collision
{
	// get center point circle first 
	glm::vec2 center(one.Position + one.Radius);
	// calculate AABB info (center, half-extents)
	glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
	// get difference vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// now that we know the clamped values, add this to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	// now retrieve vector between center circle and closest point AABB and check if length < radius
	difference = closest - center;

	if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

Direction VectorDirection(glm::vec2 target)
{
	//计算向量的朝向(N,E,S or W)

	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f),	// down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

bool ShouldSpawn(unsigned int chance)
{
	//probability: 1/N
	unsigned int random = rand() % chance;
	return random == 0;
}

void ActivatePowerUp(PowerUp& powerUp)
{
	//被挡板“吃掉”的道具被激活，产生相应效果
	if (powerUp.Type == "speed")
	{
		Ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		Ball->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->PassThrough = true;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = true; // only activate if chaos wasn't already active
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = true;
	}
}

bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
	//判断是否还有其他同类被激活的道具

	for (const PowerUp& powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}
	return false;
}
#endif