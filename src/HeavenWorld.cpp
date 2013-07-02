#include <iostream>
#include <random>
#include <functional>

#include "AEVectorMath.h"

#include "HeavenWorld.hpp"

#include "CameraTarget.hpp"

namespace heaven
{
	using namespace std;

	Island::Island(void)
	{
		ownership = NEUTRAL;
	}

	bool Island::tryCapture(Island &target)
	{
		switch(target.ownership)
		{
		case MINE:
			break;
		case EVIL:
			cout<<"Evil ";
			break;
		case NEUTRAL:
			cout<<"Neutral ";
			break;
		}

		cout<<"island captured"<<endl;
		target.ownership = ownership;

		return true;
	}

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
	}

	void HeavenWorld::init(void)
	{
		engine.Init(AE_INIT_WINDOW|AE_INIT_RENDER_GL|AE_INIT_SCENE|AE_INIT_CAMERA);

		engine.Refresh = iOnRefresh;
		engine.OnStart = iOnStart;

		//init camera
		view_target = new CameraTarget(engine.curCamera);
		view_target->setDistance(30.0f);
		//view_target->setPitch(30.0f);

		engine.scene->AddObject(view_target);

		loadIslands();
	}

	void HeavenWorld::run(void)
	{
		engine.Run();
		
		default_random_engine rengine;
		uniform_int_distribution<int> distrib(0,9);
		auto rnd = bind(distrib,rengine);
		for(int q=0;q<10;q++)
		{
			islands[rnd()].tryCapture(islands[rnd()]);
		}
		cout<<"Finish"<<endl;
	}

	void HeavenWorld::iOnRefresh(int *param)
	{
		if(!world) throw 0;

		world->updateWorld(param[0]);
	}

	void HeavenWorld::updateWorld(float dt_ms)
	{
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

	void HeavenWorld::loadIslands(void)
	{
		for(int q=0;q<10;q++)
		{
			Island island;

			island.SetTranslate(vec3f((q-5)*10.0f,0.0f,0.0f));

			islands.push_back(island);
		}

		islands[0].ownership = Island::MINE;
		islands[9].ownership = Island::EVIL;

		for(size_t q=0;q<islands.size();q++)
		{
			engine.scene->AddObject(&islands[q]);
		}

		selected_island = &islands[0];
	}

	void HeavenWorld::updateView(void)
	{
		if(selected_island)
			view_target->SetTranslate(selected_island->GetAbsPosition());
	}

	HeavenWorld::~HeavenWorld(void)
	{
		//TODO: just save current state
	}
}