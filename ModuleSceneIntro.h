#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"

#define MAX_SNAKE 2

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

	void OnCollision(PhysVehicle3D* body1, PhysBody3D* body2);
	void LoadMap(pugi::xml_node& node);
	
public:
	
	PhysBody3D* pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;

	uint laps_done = 0;
	uint laps_left = 3;
	Timer lap_timer;
	float last_lap_time = 0;
	float best_lap_time = 0;
	uint current_time_sec = 0;
	
	PhysBody3D* sensor;

	p2List<Cube> map_elems;

	pugi::xml_document config_file;
	pugi::xml_node config;
};
