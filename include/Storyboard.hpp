#ifndef STORYBOARD_H_
#define STORYBOARD_H_

#include <unordered_map>

namespace heaven
{
	class HeavenWorld;

	class Storyboard
	{
	public:
		enum EventType {E_START,E_CAPTURE_ISLAND,E_LOSE_ISLAND,E_DESTROY_SHIP};
	protected:
		HevenWorld *world_instance;

		typedef struct
		{
			EventType type;
			uint32_t side_uid;
			uint32_t item_uid;
		} Event;

		enum ActionType {A_ADD_SIDE,A_ADD_ISLAND,A_ADD_SHIP,A_SHOW_MSG,A_GOTO_ISLAND,A_LOAD_STORYBOARD};
		typedef struct
		{
			ActionType type;
			uint32_t island_uid;
			std::string data;	// make it JSON
		} Action;

		std::unordered_map<Event,std::vector<Action> > actions;

		void applyAction(Action &action);

		void addIsland(std::string data);
		void addShip(std::string data);
		void addSide(std::string data);
	public:
		Storyboard(void);

		bool loadFromFile(std::string filename);
		void bind(HevenWorld *world);

		void handleEvent(Event eve,uint32_t side_uid,uint32_t item_uid);

		~Storyboard(void);
	};

}

#endif /* STORYBOARD_H_ */