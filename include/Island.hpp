#ifndef ISLAND_H_
#define ISLAND_H_

#include <memory>

#include "AEObjectEmpty.h"

#include "Ownership.hpp"
#include "IslandProduct.hpp"
#include "StaticMeshLibrary.hpp"

namespace heaven
{
	// using namespace aengine;


	class Island: public aengine::AEObjectEmpty, public StaticMeshLibrary
	{
	protected:
		AEMaterial *material;

	public:
		uint32_t uid;
		uint32_t side_uid;
		uint32_t linked_to_uid;
		std::vector<uint32_t> linked_with_uid;
		std::vector<std::shared_ptr<AEObject> > waypoints;
		uint32_t takeoff_offset = 1;

		float radius;

		Island(void);

		virtual IslandProduct update(float dt_ms);

		void requestSupport(float power);
		void linkTo(uint32_t isl_uid);

		virtual void createWaypoints(void);

		virtual ~Island() = 0;
	};

	class FactoryIsland: public Island
	{
	protected:
		float time_elapsed;
		float time_per_item;

		size_t max_alive;

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