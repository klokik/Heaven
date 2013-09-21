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
		enum EventType {E_START,E_CAPTURE_ISLAND,E_CAPTURE_ALL_ISLANDS,E_LOSE_ALL_ISLANDS,E_LOSE_ISLAND,E_DESTROY_SHIP,E_CONTINUE,E_NONE};
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

		enum ActionType {A_ADD_SIDE,A_ADD_ISLAND,A_ADD_SHIP,A_SHOW_MSG,A_GOTO_ISLAND,A_LOAD_STORYBOARD,A_FINISH_WIN,A_FINISH_LOSE};
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

		Event last_event;
		size_t last_event_offset;

		std::unordered_map<Event,std::vector<Action>,EventHash> actions;

		void applyAction(Action &action);

		void addIsland(std::wstring data);
		void addShip(std::wstring data);
		void addSide(std::wstring data);
		void showMessage(std::wstring data);
	public:
		Storyboard(void);

		bool loadFromFile(std::wstring filename);
		void bind(HeavenWorld *world);

		void clear(void);

		void handleEvent(Event eve);

		~Storyboard(void);
	};

}

#endif /* STORYBOARD_H_ */