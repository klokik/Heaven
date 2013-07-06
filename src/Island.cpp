#include "Island.hpp"
#include "Ship.hpp"

namespace heaven
{
	Island::Island(void)
	{
		ownership = NEUTRAL;
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
				product.ship = new Ship("glider");
				product.prod_type = IslandProduct::SHIP;
				break;
			case PLANE:
				product.ship = new Ship("plane");
				product.prod_type = IslandProduct::SHIP;
				break;
			case ZEPPELIN:
				product.ship = new Ship("zeppelin");
				product.prod_type = IslandProduct::SHIP;
				break;
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

	IslandProduct TownIsland::update(float dt_ms)
	{
		return {IslandProduct::NONE};
	}
}