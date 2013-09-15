#include <random>
#include <algorithm>

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

		players[MINE].resources["iron"] = 100;
		players[EVIL].resources["iron"] = 100;
		players[NEUTRAL].resources["iron"] = 20;

		this->gui = nullptr;
		this->selected_island = nullptr;
		storyboard.bind(this);
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
		view_target->setDistance(30.0f);
		view_target->setPitch(-30.0f);

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

		for(auto uid_island:islands)
		{
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

		std::vector<uint32_t> to_delete;
		for(auto uid_ship:warships)
		{
			uid_ship.second->update(dt_ms);
			if(uid_ship.second->health<=0)
				to_delete.push_back(uid_ship.first);
		}

		for(uint32_t item:to_delete)
		{
			destroyWarship(item);
		}
		to_delete.clear();

		this->environment->update(game_time/1000,dt_ms);
		updateView();

		dynamic_cast<HGUI*>(gui)->update();
	}

	void HeavenWorld::iOnStart(int *param)
	{
		if(!world) throw 0;

		world->engineStarted();
	}

	void HeavenWorld::engineStarted(void)
	{
		storyboard.loadFromFile(L"res/game/level1.json");
		Storyboard::Event event = {Storyboard::E_START,0,0};
		storyboard.handleEvent(event);

		engine.scene->AddObject(gui);

		engine.scene->fonts.LoadFont("res/fonts/font_3.png","boundary",16,16);
		aengine::AEPrintLog("Engine started");
		engine.render->CacheScene(engine.scene);

		aengine::AEPrintLog("Work dir: "+aengine::AEResourceManager::GetWorkDirectory());

		game_time = 0;
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

		environment->SetTranslate(view_target->GetAbsPosition());

		//BUG
		view_target->children[0]->InvalidateTransform();
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

		for(auto ship:getIslandShips(from))
		{
			if(ship->side_uid==i_from->side_uid)
				ship->target = i_to;
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