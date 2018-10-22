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
		flipper_anchors[i] = nullptr;
	}
	
	for (int i = 0; i < 9; i++)
	{
		score_texture[i] = nullptr;
		score_print[i] = 0;
	}

	for (int i = 0; i < 2; i++)
	{
		bumpers[i] = nullptr;
	}

	for (int i = 0; i < 8; i++)
	{
		targets[i] = nullptr;
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
	score = 0;
	nBalls = 3;
	//Load Textures 
	board_texture = App->textures->Load("pinball/Pinball_GameBoard.png");
	flippers_texture[BOTTOMRIGHT] = App->textures->Load("pinball/FlipperR.png");
	flippers_texture[BOTTOMLEFT] = App->textures->Load("pinball/FlipperL.png");
	flippers_texture[LEFT] = App->textures->Load("pinball/FlipperL2.png");
	flippers_texture[RIGHT] = App->textures->Load("pinball/FlipperR2.png");
	flippers_texture[TOPLEFT] = App->textures->Load("pinball/FlipperL2.png");
	flippers_texture[TOPRIGHT] = App->textures->Load("pinball/FlipperR2.png");
	ball_texture = App->textures->Load("pinball/Pinball_Ball.png");
	score_texture[0] = App->textures->Load("pinball/0.png");
	score_texture[1] = App->textures->Load("pinball/1.png");
	score_texture[2] = App->textures->Load("pinball/2.png");
	score_texture[3] = App->textures->Load("pinball/3.png");
	score_texture[4] = App->textures->Load("pinball/4.png");
	score_texture[5] = App->textures->Load("pinball/5.png");
	score_texture[6] = App->textures->Load("pinball/6.png");
	score_texture[7] = App->textures->Load("pinball/7.png");
	score_texture[8] = App->textures->Load("pinball/8.png");
	score_texture[9] = App->textures->Load("pinball/9.png");
	score_text = App->textures->Load("pinball/score.png");
	balls_text = App->textures->Load("pinball/balls.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	target_fx = App->audio->LoadFx("pinball/target.wav");
	
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
	flipper_anchors[BOTTOMRIGHT] = App->physics->CreateCircle(231, 673, 6);
	flippers[BOTTOMRIGHT] = App->physics->CreateFlipper(BOTTOMRIGHT, 184, 666, FlipperR, 20, 42, 11, -20.0f, 20.0f, -0.15f, 0.15f, flipper_anchors[BOTTOMRIGHT]->body);
	flipper_anchors[BOTTOMLEFT] =  App->physics->CreateCircle(120, 673, 6);
	flippers[BOTTOMLEFT] = App->physics->CreateFlipper(BOTTOMLEFT, 112, 666, FlipperL, 20, 11, 11, 20.0f, 20.0f, -0.15f, 0.15f, flipper_anchors[BOTTOMLEFT]->body);
	flipper_anchors[LEFT] = App->physics->CreateCircle(69, 332, 6);
	flippers[LEFT] = App->physics->CreateFlipper(LEFT, 62, 325, FlipperL2, 16, 9, 8, 20.0f, 20.0f, 0.0f, 0.40f, flipper_anchors[LEFT]->body);
	flipper_anchors[RIGHT] = App->physics->CreateCircle(339, 416, 6);
	flippers[RIGHT] = App->physics->CreateFlipper(RIGHT, 303, 408, FlipperR2, 18, 30, 8, -20.0f, 20.0f, -0.40f, 0.0f, flipper_anchors[RIGHT]->body);
	flipper_anchors[TOPRIGHT] = App->physics->CreateCircle(325, 148, 6);
	flippers[TOPRIGHT] = App->physics->CreateFlipper(TOPRIGHT, 292, 142, FlipperR2, 18, 30, 8, -20.0f, 20.0f, -0.21f, 0.15f, flipper_anchors[TOPRIGHT]->body);
	flipper_anchors[TOPLEFT] = App->physics->CreateCircle(244, 148, 6);
	flippers[TOPLEFT] = App->physics->CreateFlipper(TOPLEFT, 239, 142, FlipperL2, 16, 9, 8, 20.0f, 20.0f, -0.15f, 0.21f, flipper_anchors[TOPLEFT]->body);

	//Create launcher
	launcher_base = App->physics->CreateRectangle(354, 710, 16, 16);
	launcher_base->body->SetType(b2_staticBody);
	launcher_top = App->physics->CreateLauncher(356, 624, 16, 16, 30, 0.1f);

	//Create bumpers
	bumpers[0] = App->physics->CreateBumper(319, 256, 24);
	bumpers[1] = App->physics->CreateBumper(218, 345, 24);

	//Create bonus
	bonus[0] = App->physics->CreateBonus(97, 443, 30, 30);
	bonus[1] = App->physics->CreateBonus(183, 425, 30, 30);
	bonus[2] = App->physics->CreateBonus(272, 447, 30, 30);
	bonus[3] = App->physics->CreateBonus(136, 558, 30, 30);
	bonus[4] = App->physics->CreateBonus(219, 556, 30, 30);
	bonus[5] = App->physics->CreateBonus(179, 634, 30, 30);

	//Create targets
	targets[0] = App->physics->CreateTarget(67, 380, 5, 20, 0.75f);
	targets[1] = App->physics->CreateTarget(52, 269, 5, 20, 0.0f);
	targets[2] = App->physics->CreateTarget(51, 234, 5, 20, 0.0f);
	targets[3] = App->physics->CreateTarget(362, 343, 5, 20, 0.0f);
	targets[4] = App->physics->CreateTarget(363, 201, 5, 20, 0.0f);
	targets[5] = App->physics->CreateTarget(62, 160, 5, 20, 0.40f);
	targets[6] = App->physics->CreateTarget(231, 34, 5, 20, 0.75f);
	targets[7] = App->physics->CreateTarget(340, 34, 5, 20, -0.75f);

	//Create deathzone
	death_zone = App->physics->CreateDeathZone(177, 728, 120, 20);

	//Create firstball
	SpawnBall();
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
			flippers[i]->body->ApplyAngularImpulse(50, true);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		for (int i = 1; i < FLIPPER_MAX; i += 2)
		{
			flippers[i]->body->ApplyAngularImpulse(-50, true);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		launcher_top->body->ApplyForce({ 0, 10000 }, { PIXEL_TO_METERS(357), PIXEL_TO_METERS(636) }, true);
	}

	//Update Score
	UpdateScore();

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

	//Draw SCORE
	for (int i = 0; i < 9; i++)
	{
		App->renderer->Blit(score_texture[score_print[i]], scoreX+=offsetScoreX, SCREEN_HEIGHT-35, NULL);
	}
	scoreX = -8;
	App->renderer->Blit(score_text, 10, SCREEN_HEIGHT - 55, NULL);

	//DRAW NBALLS
	App->renderer->Blit(score_texture[nBalls], SCREEN_WIDTH - 25, SCREEN_HEIGHT - 35, NULL);
	App->renderer->Blit(balls_text, SCREEN_WIDTH - 70, SCREEN_HEIGHT - 55, NULL);

	return UPDATE_CONTINUE;
}

void ModuleMainScene::OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Contact* contact)
{
	b2WorldManifold worldManifold;
	switch (bodyB->type)
	{
	case BUMPER: 
		contact->GetWorldManifold(&worldManifold);
		bodyA->body->ApplyForceToCenter(100*worldManifold.normal, true);
		score += 100;
		break;
	case TARGET: App->audio->PlayFx(target_fx);
		score += 100; break;
	case BONUS: App->audio->PlayFx(bonus_fx); 
		score += 100; break;
	case DEATHZONE: 
		nBalls-= 1;
		if (nBalls == 0)
		{
			game_over = true;
		}
		else {
			//p2List_item<PhysBody*> ball = bodyA;
			//balls.del(&ball);
			//SpawnBall();
		}
		break;
	default: break;
	}
}

void ModuleMainScene::UpdateScore()
{
	score_print[8] = score % 10;
	score_print[7] = score / 10 % 10;
	score_print[6] = score / 100 % 10;
	score_print[5] = score / 1000 % 10;
	score_print[4] = score / 10000 % 10;
	score_print[3] = score / 100000 % 10;
	score_print[2] = score / 1000000 % 10;
	score_print[1] = score / 10000000 % 10;
	score_print[0] = score / 100000000 % 10;
}

void ModuleMainScene::SpawnBall()
{
	balls.add(App->physics->CreateCircle(357, 599, 8));
	balls.getLast()->data->listener = this;
}


