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

	for (int i = 0; i < 6; i++)
	{
		bonus[i] = nullptr;
	}

	for (int i = 0; i < 8; i++)
	{
		targets[i] = nullptr;
		light_position[i] = nullptr;
	}

	for (int i = 0; i < SOUNDS_MAX; i++)
	{
		sounds[i] = 0;
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
	LoadTextures();

	//Add board bodies the the board list
	CreateBoard();

	//Create firstball
	SpawnBall();

	return ret;
}

void ModuleMainScene::LoadTextures()
{
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
	lighton_texture = App->textures->Load("pinball/LightOn.png");
	spring_texture = App->textures->Load("pinball/Spring.png");
	score_text = App->textures->Load("pinball/score.png");
	lastscore_text = App->textures->Load("pinball/lastscore.png");
	highscore_text = App->textures->Load("pinball/highscore.png");
	balls_text = App->textures->Load("pinball/balls.png");
	game_over_texture = App->textures->Load("pinball/gameover.png");

	//LoadSfx and music
	sounds[SOUNDS::BUMPER] = App->audio->LoadFx("pinball/bumper.wav");
	sounds[SOUNDS::LAUNCHER] = App->audio->LoadFx("pinball/launcher.wav");
	sounds[SOUNDS::FLIPPER] = App->audio->LoadFx("pinball/flipper.wav");
	sounds[SOUNDS::BONUS] = App->audio->LoadFx("pinball/bonus.wav");
	sounds[SOUNDS::TARGET] = App->audio->LoadFx("pinball/target.wav");
	sounds[SOUNDS::BONUSMAX] = App->audio->LoadFx("pinball/bonusmax.wav");
	App->audio->PlayMusic("pinball/maintheme.ogg");
}

void ModuleMainScene::CreateBoard()
{
	#include "BoardPoints.h"
	board_body.add(App->physics->CreateChain(0, 0, Pinball_GameBoard, length));
	board_body.add(App->physics->CreateChain(0, 0, Right_Low_Bouncer, 10));
	board_body.add(App->physics->CreateChain(0, 0, Left_Low_Bouncer, 10));
	board_body.add(App->physics->CreateChain(0, 0, Right_Wall, 26));
	board_body.add(App->physics->CreateChain(0, 0, Left_wall, 22));
	board_body.add(App->physics->CreateChain(0, 0, Top_Left_Turbo, 34));
	board_body.add(App->physics->CreateChain(0, 0, Mid_Left_Thingy, 30));
	left_low_bumper = App->physics->CreateChain(0, 0, LeftLowBumper, 8);
	left_low_bumper->body->GetFixtureList()->SetRestitution(0.75f);
	right_low_bumper= App->physics->CreateChain(0, 0, RightLowBumper, 8);
	right_low_bumper->body->GetFixtureList()->SetRestitution(0.75f);

	//Creating flippers
	flipper_anchors[BOTTOMRIGHT] = App->physics->CreateCircle(231, 673, 6);
	flippers[BOTTOMRIGHT] = App->physics->CreateFlipper(BOTTOMRIGHT, 184, 666, FlipperR, 20, 42, 11, -20.0f, 20.0f, -0.15f, 0.15f, flipper_anchors[BOTTOMRIGHT]->body);
	flipper_anchors[BOTTOMLEFT] = App->physics->CreateCircle(120, 673, 6);
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
	//create points to lights
	
	light_position[0] = new b2Vec2({ 72, 385 });
	light_position[1] = new b2Vec2({ 63, 254 });
	light_position[2] = new b2Vec2({ 62, 218 });
	light_position[3] = new b2Vec2({ 320, 330 });
	light_position[4] = new b2Vec2({ 320, 185 });
	light_position[5] = new b2Vec2({ 73, 154 });
	light_position[6] = new b2Vec2({ 234, 38 });
	light_position[7] = new b2Vec2({ 307, 38 });
	scoreBG = new SDL_Rect({0, 720, 378, 60});
	gameoverBG = new SDL_Rect({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
}

// Load assets
bool ModuleMainScene::CleanUp()
{
	LOG("Unloading Main scene");

	return true;
}

// Update: draw background
update_status ModuleMainScene::Update()
{
	b2Vec2 mouse_position;
	mouse_position.x = App->input->GetMouseX();
	mouse_position.y = App->input->GetMouseY();

	if (!game_over)
	{
		//Update Inputs made by the player
		UpdateInputs();
		//Check if the ball is off limits
		CheckBallLimits();
		//Update Score
		UpdateScore();

		if (fullbonus)
		{
			TurnLightsOff();
		}
	}
	else UpdateGameOver();

	// All draw functions ------------------------------------------------------

	Draw();
	

	return UPDATE_CONTINUE;
}

void ModuleMainScene::OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Contact* contact)
{
	b2WorldManifold worldManifold;
	switch (bodyB->type)
	{
	case TYPE::BUMPER: App->audio->PlayFx(sounds[SOUNDS::BUMPER]);
		contact->GetWorldManifold(&worldManifold);
		//bodyA->body->ApplyForceToCenter(100*worldManifold.normal, true);
		score += 100;
		break;
	case TYPE::TARGET: App->audio->PlayFx(sounds[SOUNDS::TARGET]);
		Target* bTarget; 
		score += 100;
		for (int i = 0; i < 8; i++)
		{
			if (targets[i]->TBody == bodyB)
			{
				targets[i]->on = true;
			}
			if (targets[i]->on)
			{
				bonuscounter++;
				if (bonuscounter == 8) {
					App->audio->PlayFx(sounds[SOUNDS::BONUSMAX]);
					score += score * 3;
					fullbonus = true;
					lightsofftimer = 200;
				}
			}
		}
		bonuscounter = 0;
		
		break;
	case TYPE::BONUS: App->audio->PlayFx(sounds[SOUNDS::BONUS]); 
		score += 100; break;
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

void ModuleMainScene::UpdateInputs()
{
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
		for (int i = 0; i < FLIPPER_MAX; i += 2)
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

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN){
		App->audio->PlayFx(sounds[SOUNDS::FLIPPER]);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		launcher_top->body->ApplyForce({ 0, 10000 }, { PIXEL_TO_METERS(357), PIXEL_TO_METERS(636) }, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		App->audio->PlayFx(sounds[SOUNDS::LAUNCHER]);
	}
}

void ModuleMainScene::CheckBallLimits()
{
	p2List_item<PhysBody*>* ball = balls.getFirst();

	if (ball->data->body->GetPosition().y > PIXEL_TO_METERS(720))
	{
		nBalls--;
		if (nBalls == 0)
		{
			game_over = true;
			SetGameOverScore(lastscore_print, lastscore);
			if (highscore < score) highscore = score;
			SetGameOverScore(highscore_print, highscore);
			ball->data->body->SetTransform({ PIXEL_TO_METERS(357), PIXEL_TO_METERS(599) }, 0);
			for (int i = 0; i < 8; i++)
			{
				targets[i]->on = false;
			}
		}
		else ball->data->body->SetTransform({ PIXEL_TO_METERS(357), PIXEL_TO_METERS(599) }, 0);

	}
}

void ModuleMainScene::Draw()
{
	//Draw board
	App->renderer->Blit(board_texture, 0, 0, NULL, 1.0f);

	//Draw every ball in the game
	for (p2List_item <PhysBody*>* ball = balls.getFirst(); ball; ball = ball->next)
	{
		int x, y;
		ball->data->GetPosition(x, y);
		App->renderer->Blit(ball_texture, x, y, NULL, 1.0F);
	}

	//Draw Flippers
	for (int i = 0; i < FLIPPER_MAX; i++)
	{
		App->renderer->Blit(flippers_texture[i], METERS_TO_PIXELS(flippers[i]->body->GetPosition().x), METERS_TO_PIXELS(flippers[i]->body->GetPosition().y), NULL, 1.0F, flippers[i]->GetRotation(), PIXEL_TO_METERS(10), PIXEL_TO_METERS(10));
	}

	//Draw Launcher
	App->renderer->Blit(spring_texture, METERS_TO_PIXELS(launcher_top->body->GetPosition().x) - 5, METERS_TO_PIXELS(launcher_top->body->GetPosition().y), NULL, 1.0f);

	//Score Layer
	SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(App->renderer->renderer, scoreBG);

	//Draw SCORE
	for (int i = 0; i < 9; i++)
	{
		App->renderer->Blit(score_texture[score_print[i]], scoreX += offsetScoreX, SCREEN_HEIGHT - 35, NULL);
	}
	scoreX = -8;
	App->renderer->Blit(score_text, 10, SCREEN_HEIGHT - 55, NULL);

	//Draw Nballs
	App->renderer->Blit(score_texture[nBalls], SCREEN_WIDTH - 25, SCREEN_HEIGHT - 35, NULL);
	App->renderer->Blit(balls_text, SCREEN_WIDTH - 70, SCREEN_HEIGHT - 55, NULL);

	if (game_over)
	{
		//Blit Game Over TEXT
		SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, 150);
		SDL_RenderFillRect(App->renderer->renderer, gameoverBG);
		App->renderer->Blit(game_over_texture, 40, SCREEN_HEIGHT / 2, NULL);
		App->renderer->Blit(lastscore_text, 40, SCREEN_HEIGHT / 1.5f, NULL);
		App->renderer->Blit(highscore_text, 40, SCREEN_HEIGHT / 1.4f, NULL);
		for (int i = 0; i < 9; i++)
		{
			App->renderer->Blit(score_texture[lastscore_print[i]], lastScoreX += offsetScoreX, SCREEN_HEIGHT / 1.5f, NULL);
			App->renderer->Blit(score_texture[highscore_print[i]], lastScoreX, SCREEN_HEIGHT / 1.4f, NULL);
		}
		lastScoreX = 150;
	}
	//Draw lights on
	for (int i = 0; i < 8; ++i)
	{
		if (targets[i]->on == true)
		{
			App->renderer->Blit(lighton_texture , light_position[i]->x, light_position[i]->y, NULL);
		}
	}
	
}

void ModuleMainScene::SpawnBall()
{
	balls.add(App->physics->CreateCircle(357, 599, 8));
	balls.getLast()->data->listener = this;
}

void ModuleMainScene::TurnLightsOff()
{
	lightsofftimer--;
	if (lightsofftimer == 0)
	{
		for (int i = 0; i < 8; i++)
		{
			targets[i]->on = false;
		}
		fullbonus = false;
	}
}

void ModuleMainScene::UpdateGameOver()
{
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		game_over = false;
		nBalls = 3;
		lastscore = score;
		score = 0;
	}
}

void ModuleMainScene::SetGameOverScore(int print[], int value)
{
	print[8] = value % 10;
	print[7] = value / 10 % 10;
	print[6] = value / 100 % 10;
	print[5] = value / 1000 % 10;
	print[4] = value / 10000 % 10;
	print[3] = value / 100000 % 10;
	print[2] = value / 1000000 % 10;
	print[1] = value / 10000000 % 10;
	print[0] = value / 100000000 % 10;
}


