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

namespace heaven
{
	using namespace aengine;

	class HeavenWorld
	{
	private:
		static HeavenWorld *world;

		float dt_ms;

		Server server;

	public:
		static HeavenWorld * const &instance;
		CameraTarget *view_target;

		std::vector<Island*> islands;
		std::vector<Ship*> warships;

		AEEngine engine;

		Island *selected_island;

		std::map<uint32_t,Side> players;

		AEObject *gui;

		HeavenWorld(void);

		void init();
		void run();

		static void iOnRefresh(int *param);
		static void iOnStart(int *param);
		static void iOnKeyDown(int *param);
		static void iOnResize(int *param);
		static void iOnMouseDown(int *param);
		static void iOnMouseMove(int *param);

		void updateWorld(float dt_ms);
		void engineStarted(void);
		void keyDown(int keycode);
		void resize(Vec2f size);

		void updateView(void);

		void addWarship(Ship *ship);
		void destroyWarship(Ship *ship);
		void addIsland(Island *island);

		void transfer(Island *from,Island *to,float amount);

		static std::vector<Ship*> iGetIslandShips(Island *island);
		std::vector<Ship*> getIslandShips(Island *island);

		~HeavenWorld(void);

	protected:
		void loadIslands(void);
	};
}

#endif /* HEAVENWORLD_H_ */