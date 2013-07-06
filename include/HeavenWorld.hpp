#ifndef HEAVENWORLD_H_
#define HEAVENWORLD_H_

#include <vector>

#include "AEEngine.h"

#include "CameraTarget.hpp"
#include "Ship.hpp"
#include "Island.hpp"

namespace heaven
{
	using namespace aengine;


	class HeavenWorld
	{
	private:
		static HeavenWorld *world;

		CameraTarget *view_target;

		float dt_ms;

	public:
		std::vector<Island*> islands;
		std::vector<Ship*> warships;

		AEEngine engine;

		Island *selected_island;

		HeavenWorld(void);

		void init();
		void run();

		static void iOnRefresh(int *param);
		static void iOnStart(int *param);
		static void iOnKeyDown(int *param);

		void updateWorld(float dt_ms);
		void engineStarted(void);
		void keyDown(int keycode);

		void updateView(void);

		void addWarship(Ship *ship);

		~HeavenWorld(void);

	protected:
		void loadIslands(void);
	};
}

#endif /* HEAVENWORLD_H_ */