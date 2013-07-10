#include <iostream>
#include <random>
#include <functional>

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

		Ship *ship01 = new Ship("glider");
		ship01->target = islands[0];
		addWarship(ship01);
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
			IslandResult result = island->update(dt_ms);

			switch(result)
			{
			case IslandResult::FOOD:
				food+=result.amount;
				break;
			case IslandResult::IRON:
				iron+=result.amount;
				break;
			case IslandResult::SHIP:
				addWarship(result.ship);
				break;
			}
		}

		for(auto ship:warships)
			ship->update(dt_ms);

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
		switch(keycode)
		{
		case SDLK_ESCAPE:
			engine.Stop();
			break;

		case '1':
		case '2':
		case '3':
			selected_island = islands[keycode-'1'];
			break;
		}	
	}

	void HeavenWorld::loadIslands(void)
	{
		for(int q=0;q<10;q++)
		{
			Island *island = new TownIsland;

			island->SetTranslate(vec3f((q-5)*10.0f,0.0f,0.0f));

			islands.push_back(island);
		}

		islands[0]->ownership = Island::MINE;
		islands[9]->ownership = Island::EVIL;

		for(size_t q=0;q<islands.size();q++)
		{
			engine.scene->AddObject(islands[q]);
		}

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

		warships.push_back(ship);
		engine.scene->AddObject(ship);
	}

	void HeavenWorld::addIsland(Island *island)
	{
		if(!island)
			throw 0;

		islands.push_back(island);
		engine.scene->AddObject(island);
	}

	HeavenWorld::~HeavenWorld(void)
	{
		//TODO: just save current state
	}
}