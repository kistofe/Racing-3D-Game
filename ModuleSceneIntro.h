#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"


struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void LoadMap(pugi::xml_node& node);

public:
	Cube s;
	PhysBody3D* sensor;

	Cube death;
	PhysBody3D* death_s;

	Cube anticheat;
	PhysBody3D* anticheat_s;

	uint laps_done = 0;
	uint laps_left = 3;
	Timer lap_timer;
	float last_lap_time = 0;
	float best_lap_time = 0;
	uint current_time_sec = 0;

	p2List<Cube> map_elems;

	pugi::xml_document config_file;
	pugi::xml_node config;
};
