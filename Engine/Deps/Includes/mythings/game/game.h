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


//��Ϸ��ǰ״̬
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

//�ķ���
enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};

//��ײԪ������<�Ƿ�����ײ����ײ����ƫ�Ʋ�����>  (�����������ĵ�ש����ײ������������)
typedef std::tuple<bool, Direction, glm::vec2> Collision;

//��ҵ����ʼ��С
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
//��ҵ����ʼ�ٶ�
const float PLAYER_VELOCITY(500.0f);
//С���ʼ�ٶ�
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
//С��뾶
const float BALL_RADIUS = 12.5f;
//��Ч��ʱ��
GLfloat ShakeTime = 0.0f;

//�ǳ�Ա����
bool CheckCollision_Box(GameObject& one, GameObject& two); //BOXtoBOX��ײ���
bool CheckCollision_(BallObject& one, GameObject& two);    //BALLtoBOX��ײ���
Collision CheckCollision(BallObject& one, GameObject& two);//BALLtoBOX��ײ��⣨������ײ��Ϣ��
Direction VectorDirection(glm::vec2 closest);              //����ָ��ȷ�� ��ȷ��һ����ά������ƫ����Ǹ������򣬼����ģ�����/��/��/�ң�
float clamp(float value, float min, float max);            //���ƺ���
bool ShouldSpawn(unsigned int chance);                     //������������
void ActivatePowerUp(PowerUp& powerUp);                    //�������
bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type); //�Ƿ�������ͬ�౻����ĵ���

//��Ϸ�������Ϸ��ص�״̬�͹���
//��������Ϸ��ص����ݺϲ���һ�����У���������ķ��ʺ͹���
class Game
{
public:
	GameState               State;              //��Ϸ״̬
	bool                    Keys[1024];         //��������
	bool                    KeysProcessed[1024];//���̰��� �������Ӧ�İ���������ʱ�����ǽ�������Ϊtrue ����ȷ������ֻ��֮ǰû�б��������ֱ���ɿ���ʱ���䴦����
	unsigned int            Width, Height;      //������С
	std::vector<GameLevel>  Levels;             //�ؿ�����
	std::vector<PowerUp>  PowerUps;             //��������
	unsigned int            Level;              //��ǰ�ؿ�
	unsigned int            Lives;              //����ֵ

	//���캯��/��������
	Game(unsigned int width, unsigned int height);
	~Game();

	//��ʼ����Ϸ״̬ (����������ɫ��/����/�ؿ�)
	void Init();

	//��Ϸѭ����غ���
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();

	//��ײ����
	void DoCollisions();

	//���ú���
	void ResetLevel();
	void ResetPlayer();

	//���ߺ���
	void SpawnPowerUps(GameObject& block);
	void UpdatePowerUps(float dt);
};


//����ʵ��--------------------------------------------------------------------------------------

//��Ϸ��ض���
SpriteRenderer* Renderer;      //��Ⱦ����
GameObject* Player;            //��ҵ���
BallObject* Ball;              //С��
ParticleGenerator* Particles;  //����������
PostProcessor* Effects;        //������Ч������
ISoundEngine* SoundEngine = createIrrKlangDevice();  //��Ƶ������
TextRenderer* Text;            //�ı���Ⱦ��

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
	// ������ɫ�� 
	ResourceManager::LoadShader("../../Deps/ShaderFiles/sprite.vs", "../../Deps/ShaderFiles/sprite.frag", nullptr, "sprite");
	ResourceManager::LoadShader("../../Deps/ShaderFiles/particle.vs", "../../Deps/ShaderFiles/particle.frag", nullptr, "particle");
	ResourceManager::LoadShader("../../Deps/ShaderFiles/postprocess.vs", "../../Deps/ShaderFiles/postprocess.frag", nullptr, "postprocessing");

	// ������ɫ��
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").use().setInt("image", 0);
	ResourceManager::GetShader("sprite").setMat4("projection", projection);
	ResourceManager::GetShader("particle").use().setInt("sprite", 0);
	ResourceManager::GetShader("particle").setMat4("projection", projection);

	// �������� ������ResourceManager::GetTexture()ǰ�棩
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

	// ������Ⱦ����
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 200);
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("../../Resources/fonts/OCRAEXT.TTF", 24);

	// ���عؿ�
	GameLevel one; one.Load("../../Resources/levels/one.lvl", this->Width, this->Height / 2);
	GameLevel two; two.Load("../../Resources/levels/two.lvl", this->Width, this->Height / 2);
	GameLevel three; three.Load("../../Resources/levels/three.lvl", this->Width, this->Height / 2);
	GameLevel four; four.Load("../../Resources/levels/four.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;

	//������Ϸ�����ʼ״̬
	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

	//�������֣�ѭ�����ţ�
	SoundEngine->play2D("../../Resources/audio/breakout.mp3", true);
}

void Game::Update(float dt)
{
	//С���˶�
	Ball->Move(dt, this->Width);

	//��ײ���
	this->DoCollisions();

	//����Ч��
	Particles->Update(dt, *Ball, 1, glm::vec2(Ball->Radius / 2.0f));

	//����Shakeʱ��
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = GL_FALSE;
	}

	//����Ч��
	this->UpdatePowerUps(dt);

	if (Levels[Level].IsCompleted())
		Level++;
	if (Level >= Levels.size())
		Level = 0;

	//���ʧ������
	if (Ball->Position.y >= this->Height) //С�򴥵׼�����ֵ
	{
		--this->Lives;

		if (this->Lives == 0)
		{
			this->ResetLevel();
			this->State = GAME_MENU;
		}
		this->ResetPlayer();
	}
	//���ʤ������
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
	//�˵�
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
	//ʤ��
	if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_ENTER])
		{
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
			Effects->Chaos = false;
			this->State = GAME_MENU;
		}
	}
	//��Ϸ��
	if (this->State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;

		//�����ƶ�
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)       //С��̶�ʱҲ���浲���ƶ�
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
		//����С��
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

		//��ʼ��Ⱦ������Ч��֡���壩
		Effects->BeginRender();
		//---------------------------------------

		//���Ʊ���
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
		//���ƹؿ�
		this->Levels[this->Level].Draw(*Renderer);
		//�������
		Player->Draw(*Renderer);
		//��������
		Particles->Draw();
		//����С��
		Ball->Draw(*Renderer);
		//���Ƶ��� �����Ƶ�������������δ�ݻٵĵ��ߣ�
		for (PowerUp& powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		//�����ı�
		std::stringstream ss; ss << this->Lives;
		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);

		//---------------------------------------
		//������Ⱦ������Ч
		Effects->EndRender();
		//���ƺ�����֡����
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
			powerUp.Duration -= dt; //��ʱ����ٵ��߳���ʱ��

			if (powerUp.Duration <= 0.0f)
			{
				//ͣ�ù��ڵ���Ч����֮����δ�����¼��������򽫱����٣�
				powerUp.Activated = false;

				//��û�������������ͬ�����ʱ���ͽ���Ч����ԭΪĬ��ֵ
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

	//��������ѱ��ݻ٣����������ɾ���õ��߶���
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}

void Game::DoCollisions()
{
	//���С����ש�����ײ���ѱ��ݻٵ�ש�鲻��⣩
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			//��ȡ��ײ��Ϣ
			Collision collision = CheckCollision(*Ball, box);

			if (std::get<0>(collision)) //��ײΪtrue
			{
				//ײ���ݻٿɴݻٵ�ש��
				if (!box.IsSolid)
				{
					box.Destroyed = true;       //���ٸ�ש��
					this->SpawnPowerUps(box);   //������ɵ���
					SoundEngine->play2D("../../Resources/audio/bleep.mp3", false);//������Ч
				}

				else
				{
					//ײ�����ݻٿɴݻٵ�ש�飬����ShakeЧ��
					ShakeTime = 0.05f;
					Effects->Shake = true;     //��Ч��
					SoundEngine->play2D("../../Resources/audio/solid.wav", false); //������Ч
				}

				//��ײ����������ײ��Ϣ���ı�С��ص��˶�״̬��
				Direction dir = std::get<1>(collision);              //��ײ����
				glm::vec2 diff_vector = std::get<2>(collision);      //ƫ�Ʋ�����

				if (!(Ball->PassThrough && !box.IsSolid))     //��С����йᴩ�Բ���ײ���ɴݻ�ש��ʱ������ײ����
				{
					if (dir == LEFT || dir == RIGHT) //ƽײ
					{
						Ball->Velocity.x = -Ball->Velocity.x; //��תС��ˮƽ�ٶȷ���
						//�ض�λ����С���ש����ק������
						float penetration = Ball->Radius - std::abs(diff_vector.x);
						if (dir == LEFT)
							Ball->Position.x += penetration; //����ײ�����һ���
						else
							Ball->Position.x -= penetration; //����ײ���������
					}
					else //��ֱײ
					{
						Ball->Velocity.y = -Ball->Velocity.y; //��תС����ֱ�ٶȷ���
						//�ض�λ����С���ש����ק������
						float penetration = Ball->Radius - std::abs(diff_vector.y);
						if (dir == UP)
							Ball->Position.y -= penetration; //����ײ�����»���
						else
							Ball->Position.y += penetration; //����ײ�����ϻ���
					}
				}
			}
		}
	}

	//�����ҵ�����С����ײ (����̶�ʱ�����)
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		//����С���뵲�����ײ��ı�С���ٶ�
		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		float percentage = distance / (Player->Size.x / 2.0f);

		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); //ʹ�ô����oldVelocity���ı���С���ٶ�ͳһ����֤�ٶȲ��䣬ֻ�ı䷽��

		//Ball->Velocity.y = -Ball->Velocity.y;        //��ʹ����ֱ�ٶȷ�����ת����Ϊ��ճ������
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y); //ճ�������޸�:�������ƶ����죬С�����Ľ��뵲���ڲ������������ײ��⵼�¶��y�ᷴת�����Ըɴ���ײ��ֻ���ϵ����ˡ�

		//���С�������ԣ���ճ�ڵ�����
		Ball->Stuck = Ball->Sticky;

		//������Ч
		SoundEngine->play2D("../../Resources/audio/bleep.wav", false);
	}

	//�����ҵ�������߼���ײ
	for (PowerUp& powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = true;
			if (CheckCollision_Box(*Player, powerUp))
			{
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = true;  //���Ե��ĵ��߱����ݻ١������ٻ��ƣ����ٲ�����ײ��
				powerUp.Activated = true;  //���������Ч��������ʱ������������ɾ��
				SoundEngine->play2D("../../Resources/audio/powerup.wav", false);//������Ч
			}
		}
	}
}

//�ǳ�Ա����
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
	//���������ĳ���(N,E,S or W)

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
	//�����塰�Ե����ĵ��߱����������ӦЧ��
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
	//�ж��Ƿ�������ͬ�౻����ĵ���

	for (const PowerUp& powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}
	return false;
}
#endif