#include <map>
#include <string>
#include <algorithm>
#include <random>
#include <functional>

#include "AEObjectMesh.h"
#include "AEVectorMath.h"

#include "Island.hpp"
#include "HeavenWorld.hpp"
#include "Ship.hpp"

namespace heaven
{
	using namespace aengine;

	std::default_random_engine generator;

	Island::Island(void)
	{
		side_uid = NEUTRAL;

		loadMesh("town","res/models/town.obj");
		loadMesh("iron_factory","res/models/iron_factory.obj");
		loadMesh("food_factory","res/models/food_factory.obj");
		loadMesh("ship_factory","res/models/ship_factory.obj");

		material = HeavenWorld::instance->engine.scene->materials.New();

		static uint32_t seed_uid = 0;
		uid = seed_uid++;

		radius = 1.0f;

		createWaypoints();
	}

	IslandProduct Island::update(float dt_ms)
	{
		try
		{
			material->diffuse = HeavenWorld::instance->players.at(side_uid).color;
		}catch(...){};

		return {IslandProduct::NONE};
	}

	void Island::requestSupport(float power)
	{
		for(auto linked:linked_with_uid)
			HeavenWorld::instance->transfer(linked,uid,power);
	}

	void Island::linkTo(uint32_t isl_uid)
	{
		Island *isl = HeavenWorld::instance->islands[isl_uid];
		auto &linked_with = isl->linked_with_uid;

		if(isl->side_uid != side_uid)
			return;

		//if not already linked
		if(std::find(linked_with.begin(),linked_with.end(),uid) == linked_with.end())
		{
			linked_with.push_back(uid);
		}
	}

	void Island::createWaypoints(void)
	{
		int waypoints_num = 10;
		std::uniform_real_distribution<float> distribution(-2,2);
		float radius = 5.0f;
		float orbit_height = 5.0f;

		for(int q=0;q<waypoints_num;q++)
		{
			auto obj = std::static_pointer_cast<AEObject>(std::make_shared<AEObjectEmpty>());
			waypoints.push_back(obj);
			AddChild(obj.get());

			obj->SetTranslate(vec3f(0,orbit_height,0) + vec3f(
				cos(2*3.1415926f*q/waypoints_num), 0.0f,
				sin(2*3.1415926f*q/waypoints_num))*(radius/*+distribution(generator)*/));
		}
	}

	Island::~Island(void)
	{
	}

	FactoryIsland::FactoryIsland(Type product_type)
	{
		this->product_type = product_type;
		std::map<Type,std::string> m_name = {
			{FOOD,"food_factory"},
			{IRON,"iron_factory"},
			{GLIDER,"ship_factory"},
			{PLANE,"ship_factory"},
			{ZEPPELIN,"ship_factory"}};

		AEObjectMesh *island_mesh_obj = new AEObjectMesh;
		island_mesh_obj->name = m_name[product_type];
		island_mesh_obj->mesh = getMesh(m_name[product_type]);
		island_mesh_obj->material = material;
		island_mesh_obj->SetScale(vec3f(1.0f,1.0f,1.0f)*0.1f);

		this->name = island_mesh_obj->name + "_base";

		AddChild(island_mesh_obj);

		time_per_item = 2000.0f;
		max_alive = 4;
	}

	IslandProduct FactoryIsland::produce(void)
	{
		IslandProduct product = {IslandProduct::NONE,0,nullptr};

		if(time_elapsed >= time_per_item)
		{
			auto countAliveShips = std::bind(
					static_cast<decltype(HeavenWorld::instance->warships)::const_iterator::difference_type(&)(
						decltype(HeavenWorld::instance->warships)::const_iterator,
						decltype(HeavenWorld::instance->warships)::const_iterator,
						std::function<bool(std::pair<uint32_t,std::shared_ptr<Ship> >)>)>(std::count_if),
					HeavenWorld::instance->warships.begin(),
					HeavenWorld::instance->warships.end(),
					[this](std::pair<uint32_t,std::shared_ptr<Ship> > uid_ship) { return uid_ship.second->manufacturer==this && uid_ship.second->side_uid==side_uid; });

			switch(product_type)
			{
			case FOOD:
				product.prod_type = IslandProduct::FOOD;
				product.amount = 10.0f*time_elapsed/time_per_item;
				break;
			case IRON:
				product.prod_type = IslandProduct::IRON;
				product.amount = 7.25f*time_elapsed/time_per_item;
				break;
			case GLIDER:
				if(countAliveShips()<max_alive)
					product.ship = new Ship("glider",side_uid);
				break;
			case PLANE:
				if(countAliveShips()<max_alive)
					product.ship = new Ship("plane",side_uid);
				break;
			case ZEPPELIN:
				if(countAliveShips()<max_alive)
					product.ship = new Ship("zeppelin",side_uid);
				break;
			}

			if(product.ship)
			{
				// // count alive ships
				// size_t alive = std::count_if(
				// 	HeavenWorld::instance->warships.begin(),
				// 	HeavenWorld::instance->warships.end(),
				// 	[this](std::pair<uint32_t,Ship*> uid_ship) { return uid_ship.second->manufacturer==this && uid_ship.second->side_uid==side_uid; });
				// if(alive>=max_alive)
				// {
				// 	delete product.ship;
				// 	product.ship = nullptr;
				// 	return product;
				// }

				// ship construction cost
				product.prod_type = IslandProduct::IRON;
				product.amount = -10.0f;

				product.ship->manufacturer = this;
				product.ship->goToIsland(this);
				// product.ship->SetTranslate(this->translate);

				// randomize ship speed a bit
				std::uniform_real_distribution<float> distribution(-2.0f,2.0f);
				product.ship->speed += distribution(generator);

				product.ship->SetTranslate(this->GetAbsPosition());
			}
			else
				return product;

			time_elapsed -= time_per_item;
		}

		return product;
	}

	IslandProduct FactoryIsland::update(float dt_ms)
	{
		Island::update(dt_ms);

		time_elapsed += dt_ms;

		return produce();
	}

	FactoryIsland::~FactoryIsland(void)
	{
	}

	TownIsland::TownIsland(void)
	{
		AEObjectMesh *island_mesh_obj = new AEObjectMesh;

		island_mesh_obj->mesh = getMesh("town");
		island_mesh_obj->material = material;
		island_mesh_obj->name = "town";
		island_mesh_obj->SetScale(vec3f(1.0f,1.0f,1.0f)*0.1f);

		AddChild(island_mesh_obj);

		name = "town_base";
	}

	IslandProduct TownIsland::update(float dt_ms)
	{
		Island::update(dt_ms);

		return {IslandProduct::NONE};
	}

	TownIsland::~TownIsland(void)
	{
	}
}