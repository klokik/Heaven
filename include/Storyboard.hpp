#ifndef STORYBOARD_H_
#define STORYBOARD_H_

#include <vector>
#include <string>
#include <unordered_map>


namespace heaven
{
	class HeavenWorld;

	class Storyboard
	{
	public:
		enum EventType {E_START,E_CAPTURE_ISLAND,E_LOSE_ISLAND,E_DESTROY_SHIP};
		typedef struct Event
		{
			EventType type;
			uint32_t side_uid;
			uint32_t item_uid;

			bool operator==(Event const&eve) const
			{
				return (type==eve.type)&&(side_uid==eve.side_uid)&&(item_uid==eve.item_uid);
			}
		} Event;

		enum ActionType {A_ADD_SIDE,A_ADD_ISLAND,A_ADD_SHIP,A_SHOW_MSG,A_GOTO_ISLAND,A_LOAD_STORYBOARD};
		typedef struct
		{
			ActionType type;
			std::wstring data;
		} Action;

		class EventHash
		{
			public:
				size_t operator()(Event const&eve) const
				{
					return ((int)eve.type)
					|(eve.side_uid<<(sizeof(size_t)/2+4))
					|(eve.item_uid<<4);
				}
		};
	protected:
		HeavenWorld *world_instance;

		std::unordered_map<Event,std::vector<Action>,EventHash> actions;

		void applyAction(Action &action);

		void addIsland(std::wstring data);
		void addShip(std::wstring data);
		void addSide(std::wstring data);
	public:
		Storyboard(void);

		bool loadFromFile(std::wstring filename);
		void bind(HeavenWorld *world);

		void handleEvent(Event eve);

		~Storyboard(void);
	};

}

#endif /* STORYBOARD_H_ */