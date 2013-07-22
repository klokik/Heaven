#ifndef ISLAND_H_
#define ISLAND_H_

#include "AEObjectEmpty.h"

#include "Ownership.hpp"
#include "IslandProduct.hpp"
#include "StaticMeshLibrary.hpp"

namespace heaven
{
	// using namespace aengine;


	class Island: public AEObjectEmpty, public StaticMeshLibrary
	{
	protected:
		AEMaterial *material;

	public:
		uint32_t side_uid;

		Island(void);

		virtual IslandProduct update(float dt_ms);

		virtual ~Island() = 0;
	};

	class FactoryIsland: public Island
	{
	protected:
		float time_elapsed;
		float time_per_item;

		int max_alive;

	public:
		enum Type {FOOD,IRON,GLIDER,PLANE,ZEPPELIN};

		Type product_type;

		FactoryIsland(Type product_type);

		IslandProduct produce(void);
		virtual IslandProduct update(float dt_ms);

		virtual ~FactoryIsland(void);
	};

	class TownIsland: public Island
	{
	public:

		TownIsland(void);
		
		virtual IslandProduct update(float dt_ms);

		virtual ~TownIsland(void);
	};
}

#endif /* ISLAND_H_ */