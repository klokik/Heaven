#include <iostream>
#include <random>
#include <functional>
#include "HeavenWorld.hpp"

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
		for(int q=0;q<10;q++)
		{
			Island island;
			islands.push_back(island);
		}

		islands[0].ownership = Island::MINE;
		islands[9].ownership = Island::EVIL;

		engine.Init(AE_INIT_WINDOW|AE_INIT_RENDER_GL|AE_INIT_SCENE|AE_INIT_CAMERA);

		engine.Refresh = iOnRefresh;
		engine.OnStart = iOnStart;	
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
		//TODO
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
}