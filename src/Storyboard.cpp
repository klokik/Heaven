#include "AEDebug.h"

#include "Storyboard.hpp"


namespace heaven
{
		Storyboard::Storyboard(void)
		{
			throw 0;
		}

		void Storyboard::applyAction(Action &action)
		{
			switch(action.type)
			{
			case A_LOAD_STORYBOARD:
				loadFromFile(action.data);
				break;
			case A_ADD_SIDE:
				addSide(action.data);
				break;
			case A_ADD_ISLAND:
				addIsland(action.data);
				break;
			case A_ADD_SHIP:
				addShip(action.data);
				break;
			case A_SHOW_MSG:
				static_cast<HGUI*>(world_instance->gui)->showMessage(action.data);
				break;
			case A_GOTO_ISLAND:
				try
				{
					Island *isl = world_instance->islands.at(action.island_uid);
					world_instance->selected_island = isl;
				}
				catch(...)
				{
					AEPrintLog("island not found");
				}
				break;
			}
		}

		void Storyboard::addIsland(std::string data)
		{
			throw 0;
		}

		void Storyboard::addShip(std::string data)
		{
			throw 0;
		}

		void Storyboard::addSide(std::string data)
		{
			throw 0;
		}

		bool Storyboard::loadFromFile(std::string filename)
		{
			throw 0;
		}

		void Storyboard::bind(HevenWorld *world)
		{
			if(!world)
			{
				AEPrintLog("world not bound to Storyboard");
				throw 1;
			}
			world_instance = world;
		}

		void Storyboard::handleEvent(Event eve)
		{
			try
			{
				std::vector<Action> &act = actions.at(eve);
				for(Action &action:act)
				{
					applyAction(action);
				}
			}
			catch(std::out_of_range &ex)
			{ /* no action bound */}
		}

		Storyboard::~Storyboard(void)
		{
		}
}