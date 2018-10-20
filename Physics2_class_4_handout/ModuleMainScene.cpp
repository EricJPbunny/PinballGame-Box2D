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

}

ModuleMainScene::~ModuleMainScene()
{}

// Load assets
bool ModuleMainScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Load Textures 
	board_texture = App->textures->Load("pinball/Pinball_GameBoard.png");
	flipperR_texture = App->textures->Load("pinball/FlipperR.png");
	flipperL_texture = App->textures->Load("pinball/FlipperL.png");
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

	//Add physic bodies flippers to flippers list
	right_flipper = App->physics->CreateChain(184, 666, FlipperR, 20, b2_dynamicBody);
	left_flipper = App->physics->CreateChain(112, 666, FlipperL, 20, b2_dynamicBody);

	//Create launcher
	launcher_top = App->physics->CreateRectangle(354, 624, 16, 16);
	launcher_base = App->physics->CreateRectangle(356, 710, 16, 16);
	launcher_base->body->SetType(b2_staticBody);

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	b2Body* b = App->physics->world->CreateBody(&bd);

	b2PolygonShape dshape;
	dshape.SetAsBox(PIXEL_TO_METERS(64)*0.5f, PIXEL_TO_METERS(35)*0.5f);

	b2FixtureDef dummyfix;
	dummyfix.shape = &dshape;
	dummyfix.density = 1.0f;

	b->CreateFixture(&dummyfix);

	b2MassData* massdata = new b2MassData();
	b->GetMassData(massdata);
	left_flipper->body->SetMassData(massdata);
	right_flipper->body->SetMassData(massdata);


	b2RevoluteJointDef flipperRdef;
	flipperRdef.bodyA = board_body.getFirst()->data->body;
	flipperRdef.bodyB = right_flipper->body;
	flipperRdef.Initialize(flipperRdef.bodyA, flipperRdef.bodyB, {PIXEL_TO_METERS(232), PIXEL_TO_METERS(676)});
	flipperRdef.enableLimit = true;
	flipperRdef.lowerAngle = -0.15f * b2_pi; // -90 degrees
	flipperRdef.upperAngle = 0.15f * b2_pi; // 45 degrees
	flipperRdef.enableMotor = true;
	flipperRdef.maxMotorTorque = 10.0f;
	flipperRdef.motorSpeed = -10.0f;
	right_flipper_joint = (b2RevoluteJoint*)App->physics->world->CreateJoint(&flipperRdef);
	

	b2RevoluteJointDef flipperLdef;
	flipperLdef.bodyA = board_body.getFirst()->data->body;
	flipperLdef.bodyB = left_flipper->body;
	flipperLdef.Initialize(flipperLdef.bodyA, flipperLdef.bodyB, { PIXEL_TO_METERS(119), PIXEL_TO_METERS(676) });
	flipperLdef.enableLimit = true;
	flipperLdef.lowerAngle = -0.15f * b2_pi; 
	flipperLdef.upperAngle = 0.15f * b2_pi; 
	flipperLdef.enableMotor = true;
	flipperLdef.maxMotorTorque = 10.0f;
	flipperLdef.motorSpeed = 10.0f;
	left_flipper_joint = (b2RevoluteJoint*)App->physics->world->CreateJoint(&flipperLdef);

	b2DistanceJointDef launcherDef;
	launcherDef.collideConnected = true;
	launcherDef.frequencyHz = 30;
	launcherDef.dampingRatio = 0.5f;
	launcherDef.Initialize(launcher_top->body, launcher_base->body, { PIXEL_TO_METERS(357), PIXEL_TO_METERS(636) }, { PIXEL_TO_METERS(357), PIXEL_TO_METERS(710) });
	launcher_joint = (b2DistanceJoint*)App->physics->world->CreateJoint(&launcherDef);
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
		right_flipper->body->ApplyAngularImpulse(100, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		left_flipper->body->ApplyAngularImpulse(-100, true);
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
	App->renderer->Blit(flipperR_texture, METERS_TO_PIXELS(right_flipper->body->GetPosition().x), METERS_TO_PIXELS(right_flipper->body->GetPosition().y), NULL, 1.0F, right_flipper->GetRotation(), PIXEL_TO_METERS(42), PIXEL_TO_METERS(10));
	App->renderer->Blit(flipperL_texture, METERS_TO_PIXELS(left_flipper->body->GetPosition().x), METERS_TO_PIXELS(left_flipper->body->GetPosition().y), NULL, 1.0F, left_flipper->GetRotation(), PIXEL_TO_METERS(10), PIXEL_TO_METERS(10));

	for (p2List_item <PhysBody*>* ball = balls.getFirst(); ball; ball = ball->next)
	{
		int x, y;
		ball->data->GetPosition(x, y);
		App->renderer->Blit(ball_texture, x, y, NULL, 1.0F);
	}

	return UPDATE_CONTINUE;
}

void ModuleMainScene::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	App->audio->PlayFx(bonus_fx);
}