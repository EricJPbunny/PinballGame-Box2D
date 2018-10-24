#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;
class b2RevoluteJoint;
class b2DistanceJoint;
class b2Contact;
struct Target;

enum FLIPPER {BOTTOMRIGHT, BOTTOMLEFT, RIGHT, LEFT, TOPRIGHT, TOPLEFT, FLIPPER_MAX};
enum SOUNDS {FLIPPER, BUMPER, BONUS, TARGET, LAUNCHER, GAMEOVER, START, SOUNDS_MAX};

class ModuleMainScene :
	public Module
{
public:
	ModuleMainScene(Application* app, bool start_enabled = true);
	~ModuleMainScene();

	bool Start();
	void LoadTextures();
	void CreateBoard();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Contact* contact);
	void UpdateScore();
	void UpdateInputs();
	void CheckBallLimits();
	void Draw();
	void UpdateGameOver();
	void SpawnBall();
	bool lights[8];

public:
	p2List<PhysBody*> balls;
	p2List<PhysBody*> board_body;
	PhysBody* right_wall = nullptr;
	PhysBody* flippers[FLIPPER_MAX];
	PhysBody* flipper_anchors[FLIPPER_MAX];
	PhysBody* bumpers[2];
	PhysBody* bonus[6];
	Target* targets[8];
	PhysBody* launcher_top = nullptr;
	PhysBody* launcher_base = nullptr;
	b2RevoluteJoint* flipper_joints[FLIPPER_MAX];
	b2DistanceJoint* launcher_joint = nullptr;

	SDL_Texture* board_texture = nullptr;
	SDL_Texture* flippers_texture[FLIPPER_MAX];
	SDL_Texture* ball_texture = nullptr;
	SDL_Texture* score_text = nullptr;
	SDL_Texture* balls_text = nullptr;
	SDL_Texture* score_texture[10];
	SDL_Texture* spring_texture;
	SDL_Texture* game_over_texture = nullptr;

	SDL_Rect* scoreBG;
	SDL_Rect* gameoverBG;

	uint sounds[SOUNDS::SOUNDS_MAX];

	int scoreX = 0;
	int offsetScoreX = 17;

	int score_print[10];
	int score, lastscore, highscore;
	int nBalls;

	bool game_over = false;
};

