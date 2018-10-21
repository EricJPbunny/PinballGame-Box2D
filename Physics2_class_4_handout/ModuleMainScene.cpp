#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleMainScene.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"



ModuleMainScene::ModuleMainScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	for (int i = 0; i < FLIPPER_MAX; i++)
	{
		flippers[i] = nullptr;
		flipper_joints[i] = nullptr;
		flippers_texture[i] = nullptr;
	}
	
	for (int i = 0; i < 2; i++)
	{
		bumpers[i] = nullptr;
	}
}

ModuleMainScene::~ModuleMainScene()
{
}

// Load assets
bool ModuleMainScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Load Textures 
	board_texture = App->textures->Load("pinball/Pinball_GameBoard.png");
	flippers_texture[BOTTOMRIGHT] = App->textures->Load("pinball/FlipperR.png");
	flippers_texture[BOTTOMLEFT] = App->textures->Load("pinball/FlipperL.png");
	flippers_texture[LEFT] = App->textures->Load("pinball/FlipperL2.png");
	flippers_texture[RIGHT] = App->textures->Load("pinball/FlipperR2.png");
	flippers_texture[TOPLEFT] = App->textures->Load("pinball/FlipperL2.png");
	flippers_texture[TOPRIGHT] = App->textures->Load("pinball/FlipperR2.png");
	ball_texture = App->textures->Load("pinball/Pinball_Ball.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	
	//Add board bodies the the board list
	#include "BoardPoints.h"
	board_body.add(App->physics->CreateChain(0, 0, Pinball_GameBoard, length));
	board_body.add(App->physics->CreateChain(0, 0, Right_Low_Bouncer, 10));
	board_body.add(App->physics->CreateChain(0, 0, Left_Low_Bouncer, 10));
	board_body.add(App->physics->CreateChain(0, 0, Right_Wall, 28));
	board_body.add(App->physics->CreateChain(0, 0, Left_wall, 26));
	board_body.add(App->physics->CreateChain(0, 0, Top_Left_Turbo, 34));
	board_body.add(App->physics->CreateChain(0, 0, Mid_Left_Thingy, 30));

	//Creating flippers
	flippers[BOTTOMRIGHT] = App->physics->CreateFlipper(BOTTOMRIGHT, 184, 666, FlipperR, 20, 232, 676, -10.0f, 10.0f, -0.15f, 0.15f, board_body.getFirst()->data->body);
	flippers[BOTTOMLEFT] = App->physics->CreateFlipper(BOTTOMLEFT, 112, 666, FlipperL, 20, 119, 676, 10.0f, 10.0f, -0.15f, 0.15f, board_body.getFirst()->data->body);
	flippers[LEFT] = App->physics->CreateFlipper(LEFT, 65, 323, FlipperL2, 16, 69, 333, 10.0f, 10.0f, 0.0f, 0.40f, board_body.getFirst()->data->body);
	flippers[RIGHT] = App->physics->CreateFlipper(RIGHT, 303, 408, FlipperR2, 18, 338, 416, -10.0f, 10.0f, -0.40f, 0.0f, board_body.getFirst()->data->body);
	flippers[TOPRIGHT] = App->physics->CreateFlipper(TOPRIGHT, 292, 142, FlipperR2, 18, 324, 149, -10.0f, 10.0f, -0.21f, 0.15f, board_body.getFirst()->data->body);
	flippers[TOPLEFT] = App->physics->CreateFlipper(TOPLEFT, 239, 142, FlipperL2, 16, 244, 149, 10.0f, 10.0f, -0.15f, 0.21f, board_body.getFirst()->data->body);

	//Create launcher
	launcher_base = App->physics->CreateRectangle(354, 710, 16, 16);
	launcher_base->body->SetType(b2_staticBody);
	launcher_top = App->physics->CreateLauncher(356, 624, 16, 16, 30, 0.1f);

	//Create bumpers
	bumpers[0] = App->physics->CreateBumper(319, 256, 24);
	bumpers[1] = App->physics->CreateBumper(218, 345, 24);
	return ret;
}

// Load assets
bool ModuleMainScene::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleMainScene::Update()
{
	p2List_item<PhysBody*>* ball = balls.getFirst();

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		balls.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 8));
		balls.getLast()->data->listener = this;
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		App->renderer->camera.y -= 3;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		App->renderer->camera.y += 3;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		for (int i = 0; i < FLIPPER_MAX; i+=2)
		{
			flippers[i]->body->ApplyAngularImpulse(100, true);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		for (int i = 1; i < FLIPPER_MAX; i += 2)
		{
			flippers[i]->body->ApplyAngularImpulse(-100, true);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		launcher_top->body->ApplyForce({ 0, 10000 }, { PIXEL_TO_METERS(357), PIXEL_TO_METERS(636) }, true);
	}


	// Prepare for raycast ------------------------------------------------------

	b2Vec2 mouse_position;
	mouse_position.x = App->input->GetMouseX();
	mouse_position.y = App->input->GetMouseY();

	// All draw functions ------------------------------------------------------

	App->renderer->Blit(board_texture, 0, 0, NULL, 1.0f);
	

	for (p2List_item <PhysBody*>* ball = balls.getFirst(); ball; ball = ball->next)
	{
		int x, y;
		ball->data->GetPosition(x, y);
		App->renderer->Blit(ball_texture, x, y, NULL, 1.0F);
	}

	for (int i = 0; i < FLIPPER_MAX; i++)
	{
		App->renderer->Blit(flippers_texture[i], METERS_TO_PIXELS(flippers[i]->body->GetPosition().x), METERS_TO_PIXELS(flippers[i]->body->GetPosition().y), NULL, 1.0F, flippers[i]->GetRotation(), PIXEL_TO_METERS(10), PIXEL_TO_METERS(10));
	}

	return UPDATE_CONTINUE;
}

void ModuleMainScene::OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Contact* contact)
{
	b2WorldManifold worldManifold;
	switch (bodyB->type)
	{
	case BUMPER: App->audio->PlayFx(bonus_fx);
		contact->GetWorldManifold(&worldManifold);
		bodyA->body->ApplyForce(100*worldManifold.normal, worldManifold.points[0], true);
		break;
	case TARGET: break;
	default: break;
	}
}