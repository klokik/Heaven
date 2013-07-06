#ifndef ISLAND_H_
#define ISLAND_H_

#include "AEObjectEmpty.h"

#include "IslandProduct.hpp"
#include "StaticMeshLibrary.hpp"

namespace heaven
{
	// using namespace aengine;


	class Island: public AEObjectEmpty, public StaticMeshLibrary
	{
	public:
		enum Ownership {MINE,EVIL,NEUTRAL};

		Ownership ownership;

		Island(void);

		virtual IslandProduct update(float dt_ms) = 0;
	};

	class FactoryIsland: public Island
	{
	protected:
		float time_elapsed;
		float time_per_item;

	public:
		enum Type {FOOD,IRON,GLIDER,PLANE,ZEPPELIN};

		Type product_type;

		FactoryIsland(Type product_type);

		IslandProduct produce(void);
		virtual IslandProduct update(float dt_ms);
	};

	class TownIsland: public Island
	{
	public:

		TownIsland(void);
		
		virtual IslandProduct update(float dt_ms);
	};
}

#endif /* ISLAND_H_ */