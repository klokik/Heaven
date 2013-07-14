#include <iostream>
#include <random>
#include <algorithm>

#include "AEObjectMesh.h"
#include "AEVectorMath.h"

#include "HeavenWorld.hpp"

#include "CameraTarget.hpp"

void LoadObjFile(AEMesh *&mesh, const char *path);

namespace heaven
{
	using namespace std;


	HeavenWorld *HeavenWorld::world = nullptr;

	HeavenWorld::HeavenWorld(void)
	{
		// Check if world've been already created
		if(HeavenWorld::world)
		{	
			cout<<"World have been already created!"<<endl;
			throw 0;
		}

		HeavenWorld::world = this;
		dt_ms = 0.0f;
	}

	void HeavenWorld::init(void)
	{
		engine.Init(AE_INIT_WINDOW|AE_INIT_RENDER_GL|AE_INIT_SCENE|AE_INIT_CAMERA);

		engine.Refresh = iOnRefresh;
		engine.OnStart = iOnStart;
		engine.OnKeyDown = iOnKeyDown;

		//init camera
		view_target = new CameraTarget(engine.curCamera);
		view_target->setDistance(30.0f);
		view_target->setPitch(-30.0f);

		engine.scene->AddObject(view_target);

		loadIslands();
	}

	void HeavenWorld::run(void)
	{
		engine.Run();

		cout<<"Finish"<<endl;
	}

	void HeavenWorld::iOnRefresh(int *param)
	{
		if(!world) throw 0;

		world->updateWorld(param[0]);
	}

	void HeavenWorld::updateWorld(float dt_ms)
	{
		this->dt_ms = dt_ms;

		for(auto island:islands)
		{
			IslandProduct product = island->update(dt_ms);

			switch(product.prod_type)
			{
			case IslandProduct::FOOD:
				food+=product.amount;
				break;
			case IslandProduct::IRON:
				iron+=product.amount;
				break;
			case IslandProduct::SHIP:
				addWarship(product.ship);
				break;
			case IslandProduct::NONE:
				// just to be compelte
				break;
			}
		}

		for(auto ship:warships)
		{
			ship->update(dt_ms);
		}

		for(auto ship=warships.begin();ship!=warships.end();)
		{
			if((*ship)->health<=0)
			{
				ship = destroyWarship(*ship);
			}
			else
				++ship;
		}

		updateView();
	}

	void HeavenWorld::iOnStart(int *param)
	{
		if(!world) throw 0;

		world->engineStarted();
	}

	void HeavenWorld::engineStarted(void)
	{
		engine.render->CacheScene(engine.scene);
	}

	void HeavenWorld::iOnKeyDown(int *param)
	{
		if(!world) throw 0;

		world->keyDown(param[0]);
	}

	void HeavenWorld::keyDown(int keycode)
	{
		static Island *isl_from = nullptr;
		static Island *isl_target = nullptr;

		switch(keycode)
		{
		case SDLK_ESCAPE:
			engine.Stop();
			break;

		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
			selected_island = islands[keycode-'0'];
			break;

		case 'c':
			isl_from = selected_island;
			break;
		case 'f':
			isl_target = selected_island;
			for(auto ship:getIslandShips(isl_from))
				ship->target = isl_target;
			break;
		}	
	}

	void HeavenWorld::loadIslands(void)
	{
		Island *island;


		float dist = 10.0f;
		// towns
		island = new TownIsland;
		island->SetTranslate(vec3f((0-5)*dist,0.0f,0.0f));
		addIsland(island);
		island->ownership = MINE;
		island = new TownIsland;
		island->SetTranslate(vec3f((9-5)*dist,0.0f,0.0f));
		addIsland(island);
		island->ownership = EVIL;

		// farms
		island = new FactoryIsland(FactoryIsland::FOOD);
		island->SetTranslate(vec3f((1-5)*dist,0.0f,0.0f));
		addIsland(island);
		island->ownership = MINE;
		island = new FactoryIsland(FactoryIsland::FOOD);
		island->SetTranslate(vec3f((8-5)*dist,0.0f,0.0f));
		addIsland(island);
		island->ownership = EVIL;

		// mines
		island = new FactoryIsland(FactoryIsland::IRON);
		island->SetTranslate(vec3f((2-5)*dist,0.0f,0.0f));
		addIsland(island);
		island->ownership = MINE;
		island = new FactoryIsland(FactoryIsland::IRON);
		island->SetTranslate(vec3f((7-5)*dist,0.0f,0.0f));
		addIsland(island);
		island->ownership = EVIL;

		// factories
		island = new FactoryIsland(FactoryIsland::GLIDER);
		island->SetTranslate(vec3f((3-5)*dist,0.0f,0.0f));
		addIsland(island);
		island->ownership = MINE;
		island = new FactoryIsland(FactoryIsland::ZEPPELIN);
		island->SetTranslate(vec3f((4-5)*dist,0.0f,0.0f));
		addIsland(island);
		island->ownership = NEUTRAL;
		island = new FactoryIsland(FactoryIsland::ZEPPELIN);
		island->SetTranslate(vec3f((5-5)*dist,0.0f,0.0f));
		addIsland(island);
		island->ownership = NEUTRAL;
		island = new FactoryIsland(FactoryIsland::GLIDER);
		island->SetTranslate(vec3f((6-5)*dist,0.0f,0.0f));
		addIsland(island);
		island->ownership = EVIL;

		islands[0]->ownership = MINE;
		islands[9]->ownership = EVIL;

		selected_island = islands[0];
	}

	void HeavenWorld::updateView(void)
	{
		if(engine.GetKeyState('a'))
			view_target->relYaw(-4.0f);
		if(engine.GetKeyState('d'))
			view_target->relYaw(4.0f);
		if(engine.GetKeyState('w'))
			view_target->relDistance(-1.0f);
		if(engine.GetKeyState('s'))
			view_target->relDistance(1.0f);

		if(selected_island)
		{
			float max_dist = 0.1f;
			float speed = 10.0f; // 10 units/second
			Vec3f m_vec = selected_island->GetAbsPosition()-view_target->translate;
			if(SqrLength(m_vec)>=max_dist*max_dist)
			{
				Vec3f d_vec = vec3f(0.0f,0.0f,0.0f);
				//normalize m_vec
				d_vec = m_vec*(1/Length(m_vec))*speed*(dt_ms/1000);
				view_target->RelTranslate(d_vec);
			}
		}

		//BUG
		view_target->children[0]->InvalidateTransform();
	}

	void HeavenWorld::addWarship(Ship *ship)
	{
		if(!ship)
			throw 0;

		ship->getIslandShips = iGetIslandShips;
		warships.push_back(ship);
		engine.scene->AddObject(ship);
	}

	std::vector<Ship*>::iterator HeavenWorld::destroyWarship(Ship *ship)
	{
		// we need some animation for destroy process
		auto next_item = remove(warships.begin(),warships.end(),ship);
		warships.erase(next_item,warships.end());
		engine.scene->RemoveObject(ship);

		delete ship;

		return next_item;
	}

	void HeavenWorld::addIsland(Island *island)
	{
		if(!island)
			throw 0;

		islands.push_back(island);
		engine.scene->AddObject(island);
	}

	vector<Ship*> HeavenWorld::getIslandShips(Island *island)
	{
		vector<Ship*> result;

		for(auto ship:warships)
		{
			if(ship->target == island)
				result.push_back(ship);
		}

		return result;
	}

	vector<Ship*> HeavenWorld::iGetIslandShips(Island *island)
	{
		if(!world) throw 0;

		return world->getIslandShips(island);
	}


	HeavenWorld::~HeavenWorld(void)
	{
		//TODO: just save current state
	}
}