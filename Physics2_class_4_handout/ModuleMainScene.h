#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

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
	PhysBody* board_body = nullptr;

	SDL_Texture* board_texture = nullptr;
	uint bonus_fx;
};

