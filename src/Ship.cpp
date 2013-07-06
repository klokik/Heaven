#include "AEVectorMath.h"

#include "Ship.hpp"
#include "HeavenWorld.hpp"

void LoadObjFile(AEMesh *&mesh, const char *path);

namespace heaven
{
	using namespace std;
	using namespace aengine;

	map<string,AEMesh*> Ship::mesh_library;

	Ship::Ship(void)
	{
		target = nullptr;

		static int counter = 0;
		AEObjectMesh *ship_mesh = new AEObjectMesh;
		ship_mesh->mesh = getMesh("glider");
		ship_mesh->name = "warship " + counter;

		AddChild(ship_mesh);

		SetTranslate(vec3f(0.0f,3.0f,0.0f));

		attack_time_limit = 200;
		attack_dt = attack_time_limit;

		speed = 20.0f;
	}

	void Ship::update(float dt_ms)
	{
		attack_dt += dt_ms;

		moveToTarget(dt_ms);
		if(target->ownership == Island::EVIL
			&& attack_dt >= attack_time_limit)
			attack();
	}

	void Ship::moveToTarget(float dt_ms)
	{
		if(target)
		{
			float max_dist = 0.1f;
			Vec3f m_vec = target->GetAbsPosition()-this->translate;
			if(SqrLength(m_vec)>=max_dist*max_dist)
			{
				Vec3f d_vec = vec3f(0.0f,0.0f,0.0f);
				//normalize m_vec
				d_vec = m_vec*(1/Length(m_vec))*speed*(dt_ms/1000);
				d_vec.Y = 0.0f;
				RelTranslate(d_vec);
			}
		}
	}

	void Ship::attack(void)
	{
		//TODO
	}

	void Ship::loadMeshLibrary(void)
	{
		AEMesh *mesh;

		LoadObjFile(mesh,"res/models/glider.obj");
		mesh_library.insert(pair<string,AEMesh*>("glider",mesh));

		LoadObjFile(mesh,"res/models/plane.obj");
		mesh_library.insert(pair<string,AEMesh*>("plane",mesh));

		LoadObjFile(mesh,"res/models/zeppelin.obj");
		mesh_library.insert(pair<string,AEMesh*>("zeppelin",mesh));
	}

	AEMesh *Ship::getMesh(string name)
	{
		static bool loaded = false;

		if(!loaded)
		{
			loadMeshLibrary();
			loaded = true;
		}

		return mesh_library.at(name);
	}
}