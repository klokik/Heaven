
#ifndef SHIP_H_
#define SHIP_H_

#include <map>
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

		Ship(std::string type);

		void update(float dt_ms);
		void move(float dt_ms);
		void attack(void);

	protected:
		float attack_dt;
		float attack_time_limit;
	};
}

#endif /* SHIP_H_ */