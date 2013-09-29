#include "AEDebug.h"
#include "AEResourceManager.h"

#include "JSON.h"

#include "HGUI.hpp"
#include "HeavenWorld.hpp"
#include "Player.hpp"
#include "Storyboard.hpp"


namespace heaven
{
	using namespace aengine;

	Storyboard::Storyboard(void)
	{
	}

	void Storyboard::applyAction(Action &action)
	{
		switch(action.type)
		{
		case A_LOAD_STORYBOARD:
			{
				JSONValue *value = JSON::Parse(action.data.c_str());
				if(!value)	break;
				JSONObject item = value->AsObject();
				if(item.find(L"filename")!=item.end()&&item[L"filename"]->IsString())
				{
					loadFromFile(item[L"filename"]->AsString());
					handleEvent({E_START,0,0});
				}
				else
					AEPrintLog("filename unspecified");

				delete value;
			}
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
			showMessage(action.data);
			break;
		case A_GOTO_ISLAND:
			try
			{
				JSONValue *value = JSON::Parse(action.data.c_str());
				if(!value)	break;
				JSONObject item = value->AsObject();
				if(item.find(L"island_uid")!=item.end()&&item[L"island_uid"]->IsNumber())
				{
					size_t island_uid = (size_t)item[L"island_uid"]->AsNumber();

					Island *isl = world_instance->islands.at(island_uid);
					world_instance->selected_island = isl;
				}
				else
					AEPrintLog("island_uid unspecified");

				delete value;
			}
			catch(...)
			{
				AEPrintLog("island not found");
			}
			break;
		case A_FINISH_WIN:
			throw 0;
			break;
		case A_FINISH_LOSE:
			throw 0;
			break;
		}
	}

	void Storyboard::addIsland(std::wstring data)
	{
		JSONValue *value = JSON::Parse(data.c_str());
		if(!value) throw 1;
		JSONObject jisland = value->AsObject();
		if(
			jisland.find(L"side_uid")!=jisland.end()
			&&jisland.find(L"radius")!=jisland.end()
			&&jisland.find(L"island_uid")!=jisland.end()
			&&jisland.find(L"island_type")!=jisland.end()
			&&jisland.find(L"translate")!=jisland.end()
			&&jisland[L"side_uid"]->IsNumber()
			&&jisland[L"radius"]->IsNumber()
			&&jisland[L"island_uid"]->IsNumber()
			&&jisland[L"island_type"]->IsString()
			&&jisland[L"translate"]->IsString())
		{
			Island *new_island;

			auto stype = jisland[L"island_type"]->AsString();
			if(stype==L"factory")
			{
				if(jisland.find(L"factory_product")!=jisland.end()&&jisland[L"factory_product"]->IsString())
				{
					FactoryIsland::Type ftype;
					auto sftype = jisland[L"factory_product"]->AsString();
					if(sftype==L"iron")
						ftype=FactoryIsland::IRON;
					else if(sftype==L"food")
						ftype=FactoryIsland::FOOD;
					else if(sftype==L"glider")
						ftype=FactoryIsland::GLIDER;
					else if(sftype==L"plane")
						ftype=FactoryIsland::PLANE;
					else if(sftype==L"zeppelin")
						ftype=FactoryIsland::ZEPPELIN;
					else
					{
						AEPrintLog("Incorrect factory product");
						delete value;
						return;
					}

					new_island = new FactoryIsland(ftype);
				}
				else
				{
					AEPrintLog("No factory product");
					delete value;
					return;
				}
			}
			else if(stype==L"town")
				new_island = new TownIsland();
			else
			{
				AEPrintLog("Incorrect island type");
				delete value;
				return;
			}

			new_island->uid = (size_t)jisland[L"island_uid"]->AsNumber();
			new_island->side_uid = (size_t)jisland[L"side_uid"]->AsNumber();

			Vec3f trans;
			std::wstringstream wsstr(jisland[L"translate"]->AsString());
			wsstr>>trans.X>>trans.Y>>trans.Z;

			new_island->SetTranslate(trans);

			new_island->radius = jisland[L"radius"]->AsNumber();

			world_instance->addIsland(new_island);
		}
		else
			AEPrintLog("Invalid island definition");

		delete value;
	}

	void Storyboard::addShip(std::wstring data)
	{
		throw 0;
	}

	void Storyboard::addSide(std::wstring data)
	{
		JSONValue *value = JSON::Parse(data.c_str());
		if(!value) throw 1;
		JSONObject jside = value->AsObject();
		if(
			jside.find(L"side_uid")!=jside.end()
			&&jside.find(L"side_color")!=jside.end()
			&&jside.find(L"side_name")!=jside.end()
			&&jside[L"side_uid"]->IsNumber()
			&&jside[L"side_color"]->IsString()
			&&jside[L"side_name"]->IsString())
		{
			Side new_side(jside[L"side_name"]->AsString(),0);

			new_side.uid = (uint32_t)jside[L"side_uid"]->AsNumber();
			std::wstringstream wsstr(jside[L"side_color"]->AsString());
			wsstr>>new_side.color.R>>new_side.color.G>>new_side.color.B;
			new_side.color.A = 1.0f;

			if(jside.find(L"ai")!=jside.end() && jside[L"ai"]->IsNumber())
				new_side.setAI((int)jside[L"ai"]->AsNumber());
			else
				new_side.setAI(1);

			if(jside.find(L"iron")!=jside.end() && jside[L"iron"]->IsNumber())
				new_side.resources["iron"] = ((int)jside[L"iron"]->AsNumber());

			if(jside.find(L"food")!=jside.end() && jside[L"food"]->IsNumber())
				new_side.resources["food"] = ((int)jside[L"food"]->AsNumber());

			world_instance->players.emplace(new_side.uid,std::move(new_side));
		}
		else
			AEPrintLog("Invalid side definition");

		delete value;
	}

	void Storyboard::showMessage(std::wstring data)
	{
		JSONValue *value = JSON::Parse(data.c_str());
		if(!value)	return;
		JSONObject item = value->AsObject();
		if(item.find(L"text")!=item.end()&&item[L"text"]->IsString())
		{
			std::string msg(item[L"text"]->AsString().begin(),item[L"text"]->AsString().end());
			static_cast<HGUI*>(world_instance->gui)->showInGameMessage(msg);

			if(item.find(L"pause")!=item.end() && item[L"pause"]->IsNumber() && item[L"pause"]->AsNumber()!=0)
			{
				world_instance->pause();
			}
		}
		else
			AEPrintLog("text unspecified");

		delete value;
	}

	bool Storyboard::loadFromFile(std::wstring filename)
	{
		std::string sfilename(filename.begin(),filename.end());
		std::string json_str = AEResourceManager::LoadString(sfilename);

		JSONValue *value = JSON::Parse(json_str.c_str());

		if(value == nullptr)
		{
			AEPrintLog("Level code corrupted!");
			return false;
		}

		if(!value->IsObject())
		{
			AEPrintLog("Root element is not an object");
			return false;
		}

		JSONObject root = value->AsObject();

		if(root.find(L"level")!=root.end() && root[L"level"]->IsNumber())
		{
			// dbgout()<<"Level: "/*<<root[L"level"]->AsNumber()*/<<" - "<<root[L"lavel_name"]->AsString()<<std::endl;

			if(root.find(L"events")!=root.end() && root[L"events"]->IsArray())
			{
				JSONArray events = root[L"events"]->AsArray();

				for(JSONValue *&jevent:events)
				{
					if(jevent->IsObject())
					{
						auto event = jevent->AsObject();

						Event new_event = {E_START,0,0};
						auto e_type = event[L"call"]->AsString();

						if(e_type == L"on_start")
							new_event.type = E_START;
						else if(e_type == L"on_capture_island")
							new_event.type = E_CAPTURE_ISLAND;
						else if(e_type == L"on_lose_island")
							new_event.type = E_LOSE_ISLAND;
						else if(e_type == L"on_destroy_ship")
							new_event.type = E_DESTROY_SHIP;
						else if(e_type == L"on_capture_all_islands")
							new_event.type = E_CAPTURE_ALL_ISLANDS;
						else if(e_type == L"on_lose_all_islands")
							new_event.type = E_LOSE_ALL_ISLANDS;
						else
						{
							AEPrintLog("invalid call event");
							continue;
						}

						if(event.find(L"side_uid")!=event.end() && event[L"side_uid"]->IsNumber())
							new_event.side_uid = (size_t)event[L"side_uid"]->AsNumber();

						if(event.find(L"item_uid")!=event.end() && event[L"item_uid"]->IsNumber())
							new_event.item_uid = (size_t)event[L"item_uid"]->AsNumber();

						if(event.find(L"actions")!=event.end() && event[L"actions"]->IsArray())
						{
							JSONArray jactions = event[L"actions"]->AsArray();

							std::vector<Action> vactions;

							for(JSONValue *&action_val:jactions)
							{
								auto jaction = action_val->AsObject();
								Action new_action;
								auto a_type = jaction[L"action_type"]->AsString();

								if(a_type == L"add_side")
									new_action.type = A_ADD_SIDE;
								else if(a_type == L"add_island")
									new_action.type = A_ADD_ISLAND;
								else if(a_type == L"goto_island")
									new_action.type = A_GOTO_ISLAND;
								else if(a_type == L"add_ship")
									new_action.type = A_ADD_SHIP;
								else if(a_type == L"show_message")
									new_action.type = A_SHOW_MSG;
								else if(a_type == L"load_storyboard")
									new_action.type = A_LOAD_STORYBOARD;

								new_action.data = action_val->Stringify();
								vactions.push_back(new_action);
							}
							this->actions[new_event] = vactions;
						}
						else
						{
							AEPrintLog("no/incorrect actions bound to the event");
							continue;
						}
					}
					else
					{
						AEPrintLog("event corrupted");
						continue;
					}
				}
			}
		}

		if(root.find(L"player_side_uid")!=root.end()&&root[L"player_side_uid"]->IsNumber())
		{
			if(world_instance->gui)
				static_cast<HGUI*>(world_instance->gui)->interface = (uint32_t)root[L"player_side_uid"]->AsNumber();
		}
		else
			AEPrintLog("Player uid unspecified");

		delete value;

		return true;
	}

	void Storyboard::bind(HeavenWorld *world)
	{
		if(!world)
		{
			AEPrintLog("world not bound to Storyboard");
			throw 1;
		}
		world_instance = world;
	}

	void Storyboard::clear(void)
	{
		actions.clear();

		last_event = {E_NONE,0,0};
		last_event_offset = 0;
	}

	void Storyboard::handleEvent(Event eve)
	{
		try
		{
			bool t_cont = (eve.type == E_CONTINUE);

			std::vector<Action> &act = actions.at(t_cont?last_event:eve);
			for(size_t q=(t_cont?last_event_offset+1:0);q<act.size();q++)
			{
				Action &action = act[q];

				applyAction(action);
				if(action.type == A_SHOW_MSG)
				{
					if(q==(act.size()-1))
					{
						last_event_offset = 0;
						last_event = {E_NONE,0,0};
					}
					else
					{
						if(!t_cont)
							last_event = eve;
						last_event_offset = q;
						return;
					}
				}
			}

			actions.erase(eve);
		}
		catch(std::out_of_range &ex)
		{ /* no action bound */}
	}

	Storyboard::~Storyboard(void)
	{
	}
}