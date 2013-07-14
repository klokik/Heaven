#include <map>
#include <string>
#include <iostream>

#include "AEObjectMesh.h"

#include "Island.hpp"
#include "Ship.hpp"

namespace heaven
{
	using namespace aengine;

	Island::Island(void)
	{
		ownership = NEUTRAL;

		loadMesh("town","res/models/town.obj");
		loadMesh("iron_factory","res/models/iron_factory.obj");
		loadMesh("food_factory","res/models/food_factory.obj");
		loadMesh("ship_factory","res/models/ship_factory.obj");
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

		AddChild(island_mesh_obj);

		time_per_item = 2000.0f;
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
				break;
			case IRON:
				product.prod_type = IslandProduct::IRON;
				break;
			case GLIDER:
				product.ship = new Ship("glider",ownership);
				product.prod_type = IslandProduct::SHIP;
				break;
			case PLANE:
				product.ship = new Ship("plane",ownership);
				product.prod_type = IslandProduct::SHIP;
				break;
			case ZEPPELIN:
				product.ship = new Ship("zeppelin",ownership);
				product.prod_type = IslandProduct::SHIP;
				break;
			}

			if(product.ship)
			{
				product.ship->target = this;
				product.ship->SetTranslate(this->translate);
			}

			product.amount = time_elapsed/time_per_item;

			time_elapsed -= time_per_item;
		}

		return product;
	}

	IslandProduct FactoryIsland::update(float dt_ms)
	{
		time_elapsed += dt_ms;

		return produce();
	}

	TownIsland::TownIsland(void)
	{
		AEObjectMesh *island_mesh_obj = new AEObjectMesh;

		island_mesh_obj->mesh = getMesh("town");

		AddChild(island_mesh_obj);
	}

	IslandProduct TownIsland::update(float dt_ms)
	{
		return {IslandProduct::NONE};
	}
}