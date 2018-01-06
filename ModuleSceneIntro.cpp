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
	App->audio->PlayMusic("Music.ogg");
	Mix_VolumeMusic(20);

	pugi::xml_node temp = config.child("plane");
	while (temp)
	{
		LoadMap(temp);
		temp = temp.next_sibling("plane");
	}

	death.size = vec3(1500, 0.05f, 1800);
	death.SetPos(400, 5, 280);
	
	anticheat.size = vec3(0.05f, 6, 30);
	anticheat.SetPos(15, 35, -244);

	constraint_axis.radius = 2;
	constraint_axis.SetPos(0, 33, -50);
	constraint_axis_p = App->physics->AddBody(constraint_axis, 0.0f);

	constraint_rot.size = vec3(16, 2, 2);
	constraint_rot.SetPos(0, 33, -50);
	constraint_rot_p = App->physics->AddBody(constraint_rot, 0.5f);

	App->physics->AddConstraintHinge(*constraint_axis_p, *constraint_rot_p, { 0, 0, 0 }, { 0,0,0 }, { 0, 1, 0 }, { 0, 1, 0 }, true);

	AddSensors();
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

	constraint_axis.Render();
	constraint_rot.Render();

	current_time_sec = lap_timer.Read() / 1000;

	UpdateVisualWorld();
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1 == death_s)
		App->player->vehicle->SetTransform(App->player->start_location), App->player->collided_with_anticheating = false;
	
	if (body1 == anticheat_s)
		App->player->collided_with_anticheating = true;

	if (body1 == goal_sensor && App->player->collided_with_anticheating)
	{
		last_lap_time = current_time_sec;
		if (laps_done == 1 || last_lap_time < best_lap_time)
			best_lap_time = last_lap_time;
		laps_done++;
		if (laps_done > total_laps)
		{
			laps_done = 1;
			last_lap_time = 0;
			best_lap_time = 0;
		}
		lap_timer.Start();
		App->player->collided_with_anticheating = false;
	}
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

void ModuleSceneIntro::UpdateVisualWorld()
{
	float matrix[16];
	constraint_rot_p->GetTransform(&constraint_rot.transform);
}

void ModuleSceneIntro::AddSensors()
{
	//Death sensor
	pugi::xml_node d_sensor = config.child("death_sensor");
	death.size = vec3(d_sensor.child("size").attribute("x").as_float(), d_sensor.child("size").attribute("y").as_float(), d_sensor.child("size").attribute("z").as_float());
	death.SetPos(d_sensor.child("pos").attribute("x").as_float(), d_sensor.child("pos").attribute("y").as_float(), d_sensor.child("pos").attribute("z").as_float());
	death_s = App->physics->AddBody(death, 0.0f);
	death_s->SetAsSensor(true);
	death_s->collision_listeners.add(this);

	//Mid-circuit sensor to prevent cheating
	pugi::xml_node a_sensor = config.child("anticheat_sensor");
	anticheat.size = vec3(a_sensor.child("size").attribute("x").as_float(), a_sensor.child("size").attribute("y").as_float(), a_sensor.child("size").attribute("z").as_float());
	anticheat.SetPos(a_sensor.child("pos").attribute("x").as_float(), a_sensor.child("pos").attribute("y").as_float(), a_sensor.child("pos").attribute("z").as_float());
	anticheat_s = App->physics->AddBody(anticheat, 0.0f);
	anticheat_s->SetAsSensor(true);
	anticheat_s->collision_listeners.add(this);

	//Goal sensor
	pugi::xml_node g_sensor = config.child("goal_sensor");
	goal_sensor_shape.size = vec3(g_sensor.child("size").attribute("x").as_float(), g_sensor.child("size").attribute("y").as_float(), g_sensor.child("size").attribute("z").as_float());
	goal_sensor_shape.SetPos(g_sensor.child("pos").attribute("x").as_float(), g_sensor.child("pos").attribute("y").as_float(), g_sensor.child("pos").attribute("z").as_float());
	goal_sensor = App->physics->AddBody(goal_sensor_shape, 0.0f);
	goal_sensor->SetAsSensor(true);
	goal_sensor->collision_listeners.add(this);
}
