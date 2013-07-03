#ifndef HEAVENWORLD_H_
#define HEAVENWORLD_H_

#include <vector>

#include "AEEngine.h"
#include "AEObjectEmpty.h"

#include "CameraTarget.hpp"

namespace heaven
{
	using namespace aengine;

	class Character
	{
	public:
	};

	class Island: public AEObjectEmpty
	{
	public:
		enum Ownership {MINE,EVIL,NEUTRAL};

		Ownership ownership;
		std::vector<Character> population;

		Island(void);

		bool tryCapture(Island &target);
	};

	class HeavenWorld
	{
	private:
		static HeavenWorld *world;

		CameraTarget *view_target;

	public:
		// Dont add/remove islands after initialization
		std::vector<Island*> islands;

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

		~HeavenWorld(void);

	protected:
		void loadIslands(void);
	};
}

#endif /* HEAVENWORLD_H_ */