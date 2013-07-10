
#ifndef SHIP_H_
#define SHIP_H_

#include <map>
#include <vector>
#include <string>

#include "AEObjectEmpty.h"
#include "AEObjectMesh.h"

#include "StaticMeshLibrary.hpp"

namespace heaven
{
	class Island;

	class Ship: public AEObjectEmpty, public StaticMeshLibrary
	{
	public:
		Island *target;
		float speed;
		float max_health;
		float health;
		float gun_power;
		float attack_range;

		std::vector<Ship*> (*getIslandShips)(Island *island);

		Ship(std::string type);

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