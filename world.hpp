#include <vector>
#include <iostream>
#include <random>
#include <functional>

namespace heaven
{
	using namespace std;

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

		Island(void)
		{
			ownership = NEUTRAL;
		}

		bool tryCapture(Island &target)
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
	};

	class World
	{
	public:
		std::vector<Island> islands;

		void init()
		{
			for(int q=0;q<10;q++)
			{
				Island island;
				islands.push_back(island);
			}

			islands[0].ownership = Island::MINE;
			islands[9].ownership = Island::EVIL;
		}

		void run()
		{
			default_random_engine rengine;
			uniform_int_distribution<int> distrib(0,9);
			auto rnd = bind(distrib,rengine);
			for(int q=0;q<10;q++)
			{
				islands[rnd()].tryCapture(islands[rnd()]);
			}
			cout<<"Finish"<<endl;
		}
	};
}