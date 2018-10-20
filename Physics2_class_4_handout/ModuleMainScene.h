#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;
class b2RevoluteJoint;

class ModuleMainScene :
	public Module
{
public:
	ModuleMainScene(Application* app, bool start_enabled = true);
	~ModuleMainScene();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	p2List<PhysBody*> balls;
	p2List<PhysBody*> board_body;
	PhysBody* right_wall = nullptr;
	PhysBody* right_flipper = nullptr;
	PhysBody* left_flipper = nullptr;
	b2RevoluteJoint* right_flipper_joint = nullptr;
	b2RevoluteJoint* left_flipper_joint = nullptr;

	SDL_Texture* board_texture = nullptr;
	SDL_Texture* flipperR_texture = nullptr;
	SDL_Texture* flipperL_texture = nullptr;

	uint bonus_fx;
};

