#include <map>
#include <string>
#include <algorithm>

#include "AEObjectMesh.h"
#include "AEVectorMath.h"

#include "Island.hpp"
#include "HeavenWorld.hpp"
#include "Ship.hpp"

namespace heaven
{
	using namespace aengine;

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
	}

	IslandProduct Island::update(float dt_ms)
	{
		switch(side_uid)
		{
		case MINE:
			material->diffuse.vec = vec4f(0.0f,1.0f,0.0f,1.0f);	// green
			break;
		case EVIL:
			material->diffuse.vec = vec4f(1.0f,0.0f,0.0f,1.0f);	// red
			break;
		case NEUTRAL:
			material->diffuse.vec = vec4f(0.5f,0.5f,0.5f,1.0f); // lite gray
			break;
		default:
			//undefined
			material->diffuse.vec = vec4f(0.3f,0.3f,0.3f,1.0f); // dark gray
			break;
		}

		return {IslandProduct::NONE};
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
		island_mesh_obj->mesh = getMesh(m_name[product_type]);
		island_mesh_obj->material = material;

		AddChild(island_mesh_obj);

		time_per_item = 2000.0f;
		max_alive = 4;
	}

	IslandProduct FactoryIsland::produce(void)
	{
		IslandProduct product = {IslandProduct::NONE,0,nullptr};

		if(time_elapsed >= time_per_item)
		{
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
				product.ship = new Ship("glider",side_uid);
				break;
			case PLANE:
				product.ship = new Ship("plane",side_uid);
				break;
			case ZEPPELIN:
				product.ship = new Ship("zeppelin",side_uid);
				break;
			}

			if(product.ship)
			{
				// count alive ships
				size_t alive = std::count_if(
					HeavenWorld::instance->warships.begin(),
					HeavenWorld::instance->warships.end(),
					[this](std::pair<uint32_t,Ship*> uid_ship) { return uid_ship.second->manufacturer==this && uid_ship.second->side_uid==side_uid; });
				if(alive>=max_alive)
				{
					delete product.ship;
					product.ship = nullptr;
					return product;
				}
				product.prod_type = IslandProduct::IRON;
				product.ship->manufacturer = this;
				product.ship->target = this;
				product.ship->SetTranslate(this->translate);
				product.amount = -10.0f;
			}

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

		AddChild(island_mesh_obj);
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