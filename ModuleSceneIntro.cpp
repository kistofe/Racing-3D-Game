#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "ModulePlayer.h"
#include "ModulePhysics3D.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	
	Cube test2(10, 10, 10);
	sensor = App->physics->AddBody(test2, 0.0f);
	sensor->SetPos(0, 0, 20);
	sensor->SetAsSensor();
	sensor->collision_listeners.add(App->player);

	Cube test(2, 4, 5);
	test.SetPos(0, 5, 10);
	test.axis = true;

	App->physics->AddBody(test, 0.0f);
	map.add(test);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	p2List_item<Cube>* map_list_iterator = map.getFirst();
	while (map_list_iterator)
	{
		map_list_iterator->data.Render();
		map_list_iterator = map_list_iterator->next;
	}
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysVehicle3D* body2, PhysBody3D* body1)
{
	if (body2 == App->player->vehicle && body1 == sensor)
	{
		laps_done++;
	}
}

