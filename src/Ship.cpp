#include "AEVectorMath.h"

#include "Ship.hpp"
#include "HeavenWorld.hpp"

void LoadObjFile(AEMesh *&mesh, const char *path);

namespace heaven
{
	using namespace std;
	using namespace aengine;

	map<string,AEMesh*> Ship::mesh_library;

	Ship::Ship(string type)
	{
		target = nullptr;

		static int counter = 0;
		AEObjectMesh *ship_mesh = new AEObjectMesh;
		ship_mesh->mesh = getMesh(type);
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

		move(dt_ms);
		if(target->ownership == Island::EVIL
			&& attack_dt >= attack_time_limit)
			attack();
	}

	void Ship::move(float dt_ms)
	{
		if(target)
		{
			float max_dist = 0.25f;
			float distance = speed*(dt_ms/1000);

			Vec3f m_vec = target->GetAbsPosition()-this->translate;
			m_vec.Y = 0.0f;
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