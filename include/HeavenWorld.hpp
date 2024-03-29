#ifndef HEAVENWORLD_H_
#define HEAVENWORLD_H_

#include <vector>
#include <iostream>

#include "AEEngine.h"

#include "CameraTarget.hpp"
#include "Ship.hpp"
#include "Island.hpp"
#include "Ownership.hpp"
#include "Server.hpp"
#include "Environment.hpp"
#include "Storyboard.hpp"

namespace heaven
{
	using namespace aengine;

	class HeavenWorld
	{
	private:
		static HeavenWorld *world;

		float dt_ms;
		float game_time;

		Server server;

		bool paused;

	public:
		static HeavenWorld * const &instance;
		CameraTarget *view_target;

		std::map<uint32_t,Island*> islands;
		std::map<uint32_t,std::shared_ptr<Ship> > warships;

		AEEngine engine;

		Storyboard storyboard;

		Island *selected_island;

		std::map<uint32_t,Side> players;

		AEObject *gui;

		Environment *environment;

		HeavenWorld(void);

		int init();
		void run();

		void pause(void);
		void resume(void);
		void loadState(void);
		void saveState(void);
		bool isPaused(void);

		void reset(void);

		static void iOnRefresh(int *param);
		static void iOnStart(int *param);
		static void iOnKeyDown(int *param);
		static void iOnResize(int *param);
		static void iOnMouseDown(int *param);
		static void iOnMouseUp(int *param);
		static void iOnMouseMove(int *param);

		void updateWorld(float dt_ms);
		void updateGame(float dt_ms);
		void engineStarted(void);
		void keyDown(int keycode);
		void resize(Vec2f size);

		void updateView(void);

		void addWarship(std::shared_ptr<Ship> ship);
		void destroyWarship(uint32_t ship_uid);
		void addIsland(Island *island);
		void LoadEnvironment(void);

		void transfer(uint32_t from,uint32_t to,float amount);

		static std::vector<std::weak_ptr<Ship> > iGetIslandShips(uint32_t island_uid);
		std::vector<std::weak_ptr<Ship> > getIslandShips(uint32_t island_uid);

		~HeavenWorld(void);

	protected:
		void initEnvironment(void);
	};
}

#endif /* HEAVENWORLD_H_ */