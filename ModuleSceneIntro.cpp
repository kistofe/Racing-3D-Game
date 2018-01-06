#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	pugi::xml_parse_result result = config_file.load_file("map.xml");

	if (result == NULL)
	{
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	}
	else
	{
		config = config_file.child("map");
	}

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

	pugi::xml_node temp = config.child("plane");
	while (temp)
	{
		LoadMap(temp);
		temp = temp.next_sibling("plane");
	}

	death.size = vec3(1500, 0.05f, 1800);
	death.SetPos(400, 5, 280);

	death_s = App->physics->AddBody(death, 0.0f);
	death_s->SetAsSensor(true);
	death_s->collision_listeners.add(this);
	map_elems.add(death);

	anticheat.size = vec3(0.05f, 6, 30);
	anticheat.SetPos(15, 35, -244);

	map_elems.add(anticheat);

	goal_sensor_shape.size = vec3(30, 20, 10);
	goal_sensor_shape.SetPos(0, 35, 15);
	
	
	goal_sensor = App->physics->AddBody(goal_sensor_shape, 0.0f);
	goal_sensor->SetAsSensor(true);
	goal_sensor->collision_listeners.add(this);
	goal_sensor->name = "Goal";

	lap_timer.Start();
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
	p2List_item<Cube>* map_list_iterator = map_elems.getFirst();
	while (map_list_iterator)
	{
		map_list_iterator->data.Render();
		map_list_iterator = map_list_iterator->next;
	}

	current_time_sec = lap_timer.Read() / 1000;
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1 == death_s)
	{
		App->player->vehicle->SetTransform(App->player->start_location);
	}

	if (body1 == goal_sensor)
		laps_done++, colliding = true;
}

void ModuleSceneIntro::LoadMap(pugi::xml_node& map)
{
	Cube plane(map.child("size").attribute("x").as_float(), map.child("size").attribute("y").as_float(), map.child("size").attribute("z").as_float());
	plane.SetPos(map.child("pos").attribute("x").as_float(), map.child("pos").attribute("y").as_float(), map.child("pos").attribute("z").as_float());
	if (map.child("is_red").attribute("value").as_bool(false) == true)
		plane.color = Red;
	App->physics->AddBody(plane, 0.0f);
	map_elems.add(plane);
}