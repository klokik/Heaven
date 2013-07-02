#include <vector>

#include "AEEngine.h"

namespace heaven
{
	using namespace aengine;

	class Character
	{
	public:
	};

	class Island
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

	public:
		std::vector<Island> islands;

		AEEngine engine;

		HeavenWorld(void);

		void init();
		void run();

		static void iOnRefresh(int *param);
		static void iOnStart(int *param);

		void updateWorld(float dt_ms);
		void engineStarted(void);
	};
}