
#ifndef SHIP_H_
#define SHIP_H_

#include <map>
#include <vector>
#include <string>

#include "AEObjectEmpty.h"
#include "AEObjectMesh.h"
#include "AEObjectParticleSystem.h"

#include "Ownership.hpp"
#include "StaticMeshLibrary.hpp"
#include "BezierCurve.hpp"
#include "Swarm.hpp"


namespace heaven
{
	class Island;

	class Ship: public aengine::AEObjectDynamicEmpty, public StaticMeshLibrary, public SwarmItem
	{
	protected:
		Island *i_target;
		BezierQuadCurve<Vec3f> path_take_off;
		BezierQuadCurve<Vec3f> path_orbit;
		BezierQuadCurve<Vec3f> path_transfer;
		BezierQuadCurve<Vec3f> path_fall_down;

		void constructOrbitPath(void);
		void constructTakeOffPath(void);
		void constructTransferPath(Island &to);
		void constructFallDownPath(void);

		float path_position;

		void orientAlongVector(Vec3f vec);

	public:
		uint32_t uid;
		Island *const &target;
		// float speed;
		float max_health;
		float health;
		float gun_power;
		float attack_range;
		float orbit_radius;
		float orbit_height;
		uint32_t side_uid;
		Island *manufacturer;

		bool is_taking_off;
		bool is_on_orbit;
		bool is_transfering;
		bool is_falling_down;
		bool is_chasing;
		bool exploded;

		uint32_t chased_ship_uid;

		aengine::AEObjectParticleSystem bullets;

		std::vector<Ship*> (*getIslandShips)(uint32_t island_uid);

		Ship(std::string type,uint32_t _side_uid);

		void update(float dt_ms);
		void move(float dt_ms);
		void attack(Ship *aim);
		void fire(Ship *aim);
		void damage(float power);

		void goToIsland(Island *targ);
		bool canBeDisposed();

	protected:
		float attack_dt;
		float attack_time_limit;

		bool inRange(Ship *ship);
	};
}

#endif /* SHIP_H_ */