#include <random>
#include <algorithm>
#include <cmath>

#include "AEObjectMesh.h"
#include "AEVectorMath.h"
#include "AEObjectLight.h"
#include "AEDebug.h"

#include "HeavenWorld.hpp"
#include "HGUI.hpp"
#include "CameraTarget.hpp"
#include "Player.hpp"

void LoadObjFile(AEMesh *&mesh, const char *path);

namespace heaven
{
	using namespace std;


	HeavenWorld *HeavenWorld::world = nullptr;
	HeavenWorld * const &HeavenWorld::instance = HeavenWorld::world;

	HeavenWorld::HeavenWorld(void):
		server(this)
	{
		// Check if world've been already created
		if(HeavenWorld::world)
		{	
			aengine::AEPrintLog("World have been already created!");
			throw 0;
		}

		HeavenWorld::world = this;
		dt_ms = 0.0f;

		gui = nullptr;

		this->selected_island = nullptr;
		storyboard.bind(this);

		paused = false;
	}

	int HeavenWorld::init(void)
	{
		if(!engine.Init(AE_INIT_WINDOW|AE_INIT_RENDER_GL|AE_INIT_SCENE|AE_INIT_CAMERA))
			return AE_ERR;

		engine.Refresh = iOnRefresh;
		engine.OnStart = iOnStart;
		engine.OnKeyDown = iOnKeyDown;
		engine.OnResize = iOnResize;
		engine.OnMouseDown = iOnMouseDown;
		engine.OnMouseUp = iOnMouseUp;
		engine.OnMouseMove = iOnMouseMove;

		//init camera
		view_target = new CameraTarget(engine.curCamera);
		view_target->setDistance(10.0f);
		view_target->setPitch(-30.0f);
		engine.curCamera->z_far = 200;

		engine.scene->AddObject(view_target);

		initEnvironment();

		// create server and several clients
		aengine::AEPrintLog("Starting server");
		// server.start();
		aengine::AEPrintLog("Server started");

		aengine::AEPrintLog("Initialization complete");
		
		return AE_OK;
	}

	void HeavenWorld::run(void)
	{
		engine.Run();

		aengine::AEPrintLog("Finish");
	}

	void HeavenWorld::pause(void)
	{
		paused = true;
		//have to do something with delta time
	}

	void HeavenWorld::resume(void)
	{
		paused = false;
	}

	void HeavenWorld::loadState(void)
	{
		throw 0;
	}

	void HeavenWorld::saveState(void)
	{
		throw 0;
	}

	bool HeavenWorld::isPaused(void)
	{
		return paused;
	}

	void HeavenWorld::reset(void)
	{
		game_time = 0;
		paused = true;

		players.clear();
		islands.clear();
		while(!warships.empty())
			destroyWarship(warships.begin()->first);

		storyboard.clear();
		// storyboard.loadFromFile(L"res/game/level0.json");
		storyboard.loadFromFile(L"res/game/test0.json");
		Storyboard::Event event = {Storyboard::E_START,0,0};
		storyboard.handleEvent(event);
	}

	void HeavenWorld::iOnRefresh(int *param)
	{
		if(!world)
		{
			aengine::AEPrintLog("No World - No Way!");
			throw 0;
		}

		world->updateWorld(param[0]);
	}

	void HeavenWorld::updateWorld(float dt_ms)
	{
		this->dt_ms = dt_ms;

		this->game_time+=dt_ms;

		if(!isPaused())
			updateGame(dt_ms);

		this->environment->update(game_time/1000,dt_ms);
		updateView();
	}

	void HeavenWorld::updateGame(float dt_ms)
	{
		std::map<uint32_t,uint32_t> islands_by_side;

		for(auto uid_island:islands)
		{
			islands_by_side[uid_island.second->side_uid]++;

			IslandProduct product = uid_island.second->update(dt_ms);

			switch(product.prod_type)
			{
			case IslandProduct::FOOD:
				players[uid_island.second->side_uid].resources["food"]+=product.amount;
				break;
			case IslandProduct::IRON:
				players[uid_island.second->side_uid].resources["iron"]+=product.amount;
				break;
			case IslandProduct::NONE:
				break;
			}

			if(product.ship)
			{
				if(players[uid_island.second->side_uid].resources["iron"]>=0)
					addWarship(product.ship);
				else
				{
					delete product.ship;
					players[uid_island.second->side_uid].resources["iron"]-=product.amount;
				}
			}
		}

		{	// check if someone lose
			std::vector<uint32_t> sides_to_delete;

			for(auto &item:players)
			{
				if(islands_by_side[item.first]==0)
				{
					storyboard.handleEvent({Storyboard::E_LOSE_ALL_ISLANDS,item.first,0});
					sides_to_delete.push_back(item.first);
				}
			}

			for(auto uid:sides_to_delete)
			{
				players.erase(uid);
			}
		}
		// check if someone win
		if(islands_by_side.size()==1)
		{
			storyboard.handleEvent({Storyboard::E_CAPTURE_ALL_ISLANDS,islands_by_side.begin()->first,islands_by_side.begin()->second});
		}

		std::vector<uint32_t> to_delete;
		for(auto uid_ship:warships)
		{
			uid_ship.second->update(dt_ms);
			if(uid_ship.second->canBeDisposed())
				to_delete.push_back(uid_ship.first);
		}

		for(uint32_t item:to_delete)
		{
			destroyWarship(item);
		}
		to_delete.clear();

		for(auto &side:players)
		{
			side.second.update(dt_ms);
		}

		static_cast<HGUI*>(gui)->update(dt_ms);
	}

	void HeavenWorld::iOnStart(int *param)
	{
		if(!world) throw 0;

		world->engineStarted();
	}

	void HeavenWorld::engineStarted(void)
	{
		this->gui = new HGUI(this,0);
		engine.scene->AddObject(gui);

		engine.scene->fonts.LoadFont("res/fonts/font_3.png","boundary",16,16);
		aengine::AEPrintLog("Engine started");
		engine.render->CacheScene(engine.scene);

		aengine::AEPrintLog("Work dir: "+aengine::AEResourceManager::GetWorkDirectory());

		resume();
		reset();
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
		case 27: // Escape
			engine.Stop();
			break;
		}
	}

	void HeavenWorld::iOnResize(int *param)
	{
		if(!world) throw 0;

		world->resize(vec2f((float)param[0],(float)param[1]));
	}

	void HeavenWorld::resize(Vec2f size)
	{
		if(gui)
			(static_cast<HGUI*>(gui))->realign(size);
	}

	void HeavenWorld::iOnMouseDown(int *param)
	{
		if(!world) throw 0;

		static_cast<HGUI*>(world->gui)->mouseDown(vec2f(param[0],param[1]));
	}

	void HeavenWorld::iOnMouseUp(int *param)
	{
		if(!world) throw 0;

		static_cast<HGUI*>(world->gui)->mouseUp(vec2f(param[0],param[1]));
	}

	void HeavenWorld::iOnMouseMove(int *param)
	{
		if(!world) throw 0;

		static_cast<HGUI*>(world->gui)->mouseMove(vec2f(param[0],param[1]),vec2f(param[2],param[3]),param[4]);
	}

	void HeavenWorld::initEnvironment(void)
	{
		this->environment = new Environment;
		this->environment->setMaterials(this->engine.scene->materials);

		this->engine.scene->AddObject(this->environment);
	}

	void HeavenWorld::updateView(void)
	{
		float dist_delta = min(exp((view_target->getDistance()-2.0f)*0.05f)-1.0f,view_target->getDistance()/3);
		if(engine.GetKeyState('a'))
			view_target->relYaw(-4.0f);
		if(engine.GetKeyState('d'))
			view_target->relYaw(4.0f);
		if(engine.GetKeyState('w'))
			view_target->relDistance(-dist_delta);
		if(engine.GetKeyState('s'))
			view_target->relDistance(dist_delta);
		if(view_target->getDistance()>40)
			view_target->setDistance(40.0f);

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

		environment->SetTranslate(view_target->GetAbsPosition());
	}

	void HeavenWorld::addWarship(Ship *ship)
	{
		ship->getIslandShips = iGetIslandShips;
		warships[ship->uid] = ship;
		engine.scene->AddObject(ship);
	}

	void HeavenWorld::destroyWarship(uint32_t ship_uid)
	{
		// we need some animation for destroy process
		Ship *ship = warships[ship_uid];
		warships.erase(ship_uid);
		engine.scene->RemoveObject(ship);

		delete ship;
	}

	void HeavenWorld::addIsland(Island *island)
	{
		islands[island->uid] = island;
		engine.scene->AddObject(island);
	}

	void HeavenWorld::transfer(uint32_t from,uint32_t to,float amount)
	{
		Island *i_from = islands[from];
		Island *i_to = islands[to];

		std::cout<<"trans "<<from<<"-"<<to<<std::endl;

		auto island_ships = getIslandShips(from);
		for(size_t q=0;q<floor(island_ships.size()*amount);++q)
		{
			if(island_ships[q]->side_uid==i_from->side_uid)
				island_ships[q]->goToIsland(i_to);
		}
	}

	vector<Ship*> HeavenWorld::getIslandShips(uint32_t island_uid)
	{
		vector<Ship*> result;
		Island *targ = islands[island_uid];

		for(auto uid_ship:warships)
		{
			if(uid_ship.second->target == targ)
				result.push_back(uid_ship.second);
		}

		return result;
	}

	vector<Ship*> HeavenWorld::iGetIslandShips(uint32_t island_uid)
	{
		if(!world) throw 0;

		return world->getIslandShips(island_uid);
	}

	HeavenWorld::~HeavenWorld(void)
	{
		aengine::AEPrintLog("Armagedon!");
		//TODO: just save current state
	}
}