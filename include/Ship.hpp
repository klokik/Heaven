
#ifndef SHIP_H_
#define SHIP_H_

#include <map>
#include <string>

#include "AEObjectEmpty.h"
#include "AEObjectMesh.h"

namespace heaven
{
	class Island;

	class Ship: public AEObjectEmpty
	{
	public:
		Island *target;
		float speed;

		Ship(void);

		void update(float dt_ms);
		void moveToTarget(float dt_ms);
		void attack(void);

	protected:
		float attack_dt;
		float attack_time_limit;

		static std::map<std::string,AEMesh*> mesh_library;
		static void loadMeshLibrary(void);
		static AEMesh *getMesh(std::string name);
	};
}

#endif /* SHIP_H_ */