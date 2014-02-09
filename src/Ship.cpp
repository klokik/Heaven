#include "AEVectorMath.h"

#include "Ship.hpp"
#include "HeavenWorld.hpp"


namespace heaven
{
	using namespace std;
	using namespace aengine;
	

	Ship::Ship(string type,uint32_t _side_uid):
		target(i_target)
	{
		i_target = nullptr;
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
		ship_mesh->material = HeavenWorld::instance->engine.scene->materials.New();
		ship_mesh->material->diffuse.vec = HeavenWorld::instance->players[_side_uid].color.vec*1.5f;

		this->name = ship_mesh->name + "_base";

		uid = counter;
		counter++;

		AddChild(ship_mesh);

		attack_time_limit = 200;
		attack_dt = attack_time_limit;

		SwarmItem::speed = 20.0f;
		gun_power = 20.0f;
		attack_range = 7.0f;

		orbit_radius = 3.0f;
		orbit_height = 3.0f;

		max_health = 100;
		health = max_health;

		side_uid = _side_uid;

		is_taking_off = true;
		is_on_orbit = false;
		is_transfering = false;
		is_falling_down = false;
		is_chasing = false;

		path_position = 0.0f;

		constructTakeOffPath();
		constructOrbitPath();

		AddChild(&bullets);

		class ParticleAffectorDestroyBullets: public AEParticleAffector
		{
		private:
			std::vector<AEObject*> aims;
			Ship *owner;
		public:
			ParticleAffectorDestroyBullets(Ship *_owner): owner(_owner)
			{
			}

			virtual bool Affect(AEParticle &particle,float dt_ms)
			{
				for(auto aim:aims)
				{
					if(SqrLength(aim->GetAbsPosition() - owner->GetAbsPosition()) <= 1)
						return false;
				}

				return true;
			}
		};

		bullets.affectors.push_back(make_shared<ParticleAffectorDestroyBullets>(this));
	}

	void Ship::update(float dt_ms)
	{
		attack_dt += dt_ms;

		move(dt_ms);
		if(is_chasing)
			attack();

		bullets.Update(dt_ms);
	}

	void Ship::constructOrbitPath(void)
	{
		path_orbit.clear();
		path_orbit.addStartPoint(vec3f(orbit_radius,orbit_height,0.0f));

		path_orbit.addSegment(vec3f(0.0f,orbit_height,-orbit_radius),	vec3f(orbit_radius,orbit_height,-orbit_radius));
		path_orbit.addSegment(vec3f(-orbit_radius,orbit_height,0.0f),	vec3f(-orbit_radius,orbit_height,-orbit_radius));
		path_orbit.addSegment(vec3f(0.0f,orbit_height,orbit_radius),	vec3f(-orbit_radius,orbit_height,orbit_radius));
		path_orbit.addSegment(vec3f(orbit_radius,orbit_height,0.0f),	vec3f(orbit_radius,orbit_height,orbit_radius));
	}

	void Ship::constructTakeOffPath(void)
	{
		path_take_off.clear();
		path_take_off.addStartPoint(vec3f(0.0f,0.0f,0.0f));

		path_take_off.addSegment(vec3f(orbit_radius,orbit_height,0.0f),vec3f(orbit_radius,0.0f,0.0f));
	}

	void Ship::constructTransferPath(Island &to)
	{
		Vec3f direct = to.GetAbsPosition() - this->target->GetAbsPosition();
		Vec3f r_ort;

		r_ort = cross(direct,vec3f(0.0f,1.0f,0.0f));
		// if(direct.Y!=0)
		// 	r_ort = vec3f(1.0f,0.0f,-direct.X/direct.Z);
		// else
		// 	r_ort = vec3f(0.0f,0.0f,-direct.X);

		r_ort = normalize(r_ort)*orbit_radius;
		Vec3f v_or_h = vec3f(0.0f,orbit_height,0.0f);

		path_transfer.clear();

		path_transfer.addStartPoint(GetAbsPosition());

		path_transfer.addSegment(target->GetAbsPosition()+r_ort+v_or_h,GetAbsPosition()+r_ort);
		path_transfer.addSegment(to.GetAbsPosition()+path_orbit.getPoint(0,0.0f)*3,target->GetAbsPosition()+direct*0.5f+v_or_h*2);
		path_transfer.addSegment(
			to.GetAbsPosition()+path_orbit.getPoint(0,0.0f),
			to.GetAbsPosition()+path_orbit.getPoint(0,0.0f)*2 + v_or_h*(-1.0f));
	}

	void Ship::constructFallDownPath(void)
	{
		path_fall_down.clear();

		path_fall_down.addStartPoint(GetAbsPosition());
		Vec3f offset = vec3f(10.0f,-20.0f,-15.0f);
		path_fall_down.addSegment(GetAbsPosition()+offset,GetAbsPosition()+vec3f(offset.X,0.0f,10.0f));
	}

	void Ship::orientAlongVector(Vec3f vec)
	{
		static float pi = 3.14159265f;
		float angle;
		Vec3f delta = normalize(vec);
		angle = -(90.0f - acos(delta.X)*180.0f/pi);
		if(delta.Z>0)
			angle = 180 - angle;

		children[0]->SetRotate(vec3f(0.0f,angle,0.0f));
	}

	void Ship::move(float dt_ms)
	{
		// set direction - particle attractor
		if(target)
		{
			if(is_on_orbit || is_taking_off)
			{
				// choose next attractor in range of FOV

				if(!SwarmItem::attractor ||
					Length(SwarmItem::attractor->GetAbsPosition()-Ship::GetAbsPosition() < SwarmItem::radius_gain)
				{
					//TODO: use FOV
					std::dafault_random_generator generator;
					std::uniform_int_distribution distribution(0,Ship::target->waypoints.size()-1);

					SwarmItem::attractor = Ship::target->waypoints[distribution(generator)];
				}
			}

			if(is_falling_down)
				SwarmItem::attractor = nullptr; // create an attractor somewhere on the ground

			if(is_transfering)
			{
				SwarmItem::attractor = Ship::target;

				if(Length(SwarmItem::attractor->GetAbsPosition()-Ship::GetAbsPosition() < SwarmItem::radius_gain))
				{
					is_transfering = false;
					is_on_orbit = true;

					// chase ship to chase if thre is one
					std::vector<Ship*> evil_ships = getIslandShips(target->uid);
					for(auto ship:evil_ships)
						if(ship->side != this->side)
						{
							chased_ship = ship;
							break;
						}
				}
			}

			if(is_chasing)
			{
				SwarmItem::attractor = chased_ship.get();

				if(target->side_uid == this->side_uid)
				{
					is_chasing = false;
					is_on_orbit = true;
				}
			}
		}

		//swarm intelligence
		auto obstacles = getIslandShips(target->uid);
		SwarmSystem<decltype(this),decltype(obstaces)>::swarm_one(this,obstaces,dt_ms);
		orientAlongVector(SwarmItem::direction);
	}

	void Ship::attack(void)
	{
		if(!getIslandShips)
			throw 0; // island is nowhere?

		if(attack_dt >= attack_time_limit)
			attack_dt -= attack_time_limit;
		else
			return;

		if(inRange(chased_ship))
			fire(chased_ship);
		// bool only_attack_side = true;

		// std::vector<Ship*> evil_ships = getIslandShips(target->uid);
		// for(auto ship:evil_ships)
		// {
		// 	if(ship->side_uid == this->side_uid)
		// 		continue;

		// 	only_attack_side = false;

		// 	if(inRange(ship))
		// 	{
		// 		fire(ship);
		// 	}
		// }

		// if(only_attack_side&&Length(target->GetAbsPosition() - GetAbsPosition())<SwarmItem::radius_gain)
		// {
		// 	Storyboard::Event event = {Storyboard::E_LOSE_ISLAND,target->side_uid,target->uid};
		// 	HeavenWorld::instance->storyboard.handleEvent(event);

		// 	i_target->side_uid = this->side_uid;

		// 	event = {Storyboard::E_CAPTURE_ISLAND,target->side_uid,target->uid};
		// 	HeavenWorld::instance->storyboard.handleEvent(event);
		// }
	}

	void Ship::fire(Ship *aim)
	{
		bullets.emitter.direction = aim->translate - translate;
		bullets.EmitNum(1);
		aim->damage(gun_power);
	}

	void Ship::damage(float power)
	{
		this->health-=power;

		// in some case
		//FIXME: specify condition
		if(i_target->side_uid == this->side_uid)
		{
			i_target->requestSupport(0.5f);
		}
	}

	void Ship::goToIsland(Island *targ)
	{
		if(this->target && targ!=this->target)
		{
			is_on_orbit = false;
			is_transfering = true;
			path_position = 0.0f;
			constructTransferPath(*targ);
		}

		i_target = targ;
	}

	bool Ship::inRange(Ship *ship)
	{
		return (SqrLength(ship->translate - this->translate) <= attack_range*attack_range);
	}

	bool Ship::canBeDisposed()
	{
		if(is_falling_down && health<=0 && path_position >= 0.5f)
			return true;
		
		return false;
	}
}