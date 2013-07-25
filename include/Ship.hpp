
#ifndef SHIP_H_
#define SHIP_H_

#include <map>
#include <vector>
#include <string>

#include "AEObjectEmpty.h"
#include "AEObjectMesh.h"

#include "Ownership.hpp"
#include "StaticMeshLibrary.hpp"

namespace heaven
{
	class Island;

	class Ship: public AEObjectEmpty, public StaticMeshLibrary
	{
	public:
		uint32_t uid;
		Island *target;
		float speed;
		float max_health;
		float health;
		float gun_power;
		float attack_range;
		uint32_t side_uid;
		Island *manufacturer;

		std::vector<Ship*> (*getIslandShips)(uint32_t island_uid);

		Ship(std::string type,uint32_t _side_uid);

		void update(float dt_ms);
		void move(float dt_ms);
		void attack(void);

	protected:
		float attack_dt;
		float attack_time_limit;

		bool inRange(Ship *ship);
	};
}

#endif /* SHIP_H_ */