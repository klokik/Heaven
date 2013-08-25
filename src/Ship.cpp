#include "AEVectorMath.h"

#include "Ship.hpp"
#include "HeavenWorld.hpp"


namespace heaven
{
	using namespace std;
	using namespace aengine;
	

	Ship::Ship(string type,uint32_t _side_uid)
	{
		target = nullptr;
		getIslandShips = nullptr;
		manufacturer = nullptr;

		loadMesh("glider","res/models/glider.obj");
		loadMesh("plane","res/models/plane.obj");
		loadMesh("zeppelin","res/models/zeppelin.obj");

		static int counter = 0;
		AEObjectMesh *ship_mesh = new AEObjectMesh;
		// AEObjectEmpty *ship_mesh = new AEObjectEmpty;
		ship_mesh->mesh = getMesh(type);
		ship_mesh->name = "warship_"; ship_mesh->name += (char)('a'+counter);

		this->name = ship_mesh->name + "_base";

		uid = counter;
		counter++;

		AddChild(ship_mesh);

		attack_time_limit = 200;
		attack_dt = attack_time_limit;

		speed = 20.0f;
		gun_power = 20.0f;
		attack_range = 5.0f;

		max_health = 100;
		health = max_health;

		side_uid = _side_uid;
	}

	void Ship::update(float dt_ms)
	{
		attack_dt += dt_ms;

		move(dt_ms);
		if(target->side_uid != this->side_uid)
			attack();
	}

	void Ship::move(float dt_ms)
	{
		if(target)
		{
			float max_dist = 0.25f;
			float distance = speed*(dt_ms/1000);

			Vec3f m_vec = target->GetAbsPosition()+vec3f(0.0f,3.0f,0.0f)-this->translate;
			if(SqrLength(m_vec)>=max_dist*max_dist)
			{
				//move to target
				Vec3f d_vec = vec3f(0.0f,0.0f,0.0f);
				//normalize m_vec
				d_vec = m_vec*(1/Length(m_vec))*distance;
				RelTranslate(d_vec);
				SetRotate(vec3f(0.0f,0.0f,0.0f));
			}
			else
			{
				//move on orbit
				float radius = 3.0f;
				children[0]->SetTranslate(vec3f(radius,0.0f,0.0f));
				float phi = (distance/radius)*57.29577951308f;
				RelRotate(vec3f(0.0f,phi,0.0f));
			}
		}
	}

	void Ship::attack(void)
	{
		if(!getIslandShips)
			throw 0; // island is nowhere?

		if(attack_dt >= attack_time_limit)
			attack_dt -= attack_time_limit;
		else
			return;

		bool only_attack_side = true;

		std::vector<Ship*> evil_ships = getIslandShips(target->uid);
		for(auto ship:evil_ships)
		{
			if(ship->side_uid == this->side_uid)
				continue;

			if(inRange(ship))
			{
				ship->health-=gun_power;
				only_attack_side = false;
			}
		}

		if(only_attack_side)
			target->side_uid = this->side_uid;
	}

	bool Ship::inRange(Ship *ship)
	{
		return (SqrLength(ship->translate - this->translate) <= attack_range*attack_range);
	}
}