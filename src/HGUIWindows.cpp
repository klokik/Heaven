#include <string.h>
#include <sstream>

#include "AEVectorMath.h"

#include "HGUI.hpp"
#include "HeavenWorld.hpp"


namespace heaven
{
	HMainMenuWindow::HMainMenuWindow(void)
	{
		setText("Welcome to the\nHeaven World");

		btn_local	= new HButton;
		btn_network	= new HButton;
		btn_options	= new HButton;
		btn_about	= new HButton;

		prev_window = "exit";

		btn_local->position = vec2f(0.0f, 0.2f);
		btn_network->position = vec2f(0.0f, 0.0f);
		btn_options->position = vec2f(0.0f,-0.2f);
		btn_about->position = vec2f(0.0f,-0.4f);

		btn_local->getSprite().SetScale(vec3f(240.0f,64.0f,1.0f));
		btn_network->getSprite().SetScale(vec3f(240.0f,64.0f,1.0f));
		btn_options->getSprite().SetScale(vec3f(240.0f,64.0f,1.0f));
		btn_about->getSprite().SetScale(vec3f(240.0f,64.0f,1.0f));

		btn_local->getLabel().text = "Local";
		btn_network->getLabel().text = "Network";
		btn_options->getLabel().text = "Options";
		btn_about->getLabel().text = "About";

		btn_local->on_click = showLocalGameWindow;
		btn_network->on_click = showNetworkGameWindow;
		btn_options->on_click = showOptionsWindow;
		btn_about->on_click = showAboutWindow;

		controls.push_back(btn_local);
		controls.push_back(btn_network);
		controls.push_back(btn_options);
		controls.push_back(btn_about);

		this->AddChild(btn_local);
		this->AddChild(btn_network);
		this->AddChild(btn_options);
		this->AddChild(btn_about);

		btn_back->getSprite().material->diffuse.vec = vec4f(1.0f,0.0f,0.0f,0.6f);
		btn_back->getLabel().text = "Exit";
	}

	HTextInputWindow::HTextInputWindow(HTextInput *input)
	{
		text = input;

		if(!text)
			return;

		orig_text_position = text->position;
		orig_text_parent = text->parent;
		text->position = vec2f(0.0f,0.1f);
		AddChild(text);
		controls.push_back(text);

		char symbols[] = "1234567890";
		size_t count = strlen(symbols);
		for(size_t q=0;q<count;q++)
		{
			HButton *btn = new HButton;
			btn->getLabel().text = "a";

			btn->position = vec2f((q-count/2)/count,-0.1f);

			keyboard.push_back(btn);
			controls.push_back(btn);
			AddChild(btn);
		}
	}

	HLocalGameWindow::HLocalGameWindow(void)
	{
		setText("Local game");

		prev_window = "main";

		btn_continue = new HButton;
		btn_new_game = new HButton;

		btn_continue->position = vec2f(0.0f,0.2f);
		btn_new_game->position = vec2f(0.0f,0.0f);

		btn_continue->getSprite().SetScale(vec3f(240.0f,64.0f,1.0f));
		btn_new_game->getSprite().SetScale(vec3f(240.0f,64.0f,1.0f));

		btn_continue->getLabel().text = "Continue";
		btn_new_game->getLabel().text = "New game";

		btn_continue->on_click = loadAndStartGame;
		btn_new_game->on_click = startNewGame;

		controls.push_back(btn_continue);
		controls.push_back(btn_new_game);

		this->AddChild(btn_continue);
		this->AddChild(btn_new_game);
	}

	HNetworkGameWindow::HNetworkGameWindow(void)
	{
		setText("Network game");

		prev_window = "main";

		btn_start_server = new HButton;
		btn_connect_to_server = new HButton;

		btn_start_server->position = vec2f(0.0f,0.2f);
		btn_connect_to_server->position = vec2f(0.0f,0.0f);

		btn_start_server->getLabel().text = "Start new server";
		btn_connect_to_server->getLabel().text = "Connect to existing server";

		controls.push_back(btn_start_server);
		controls.push_back(btn_connect_to_server);

		this->AddChild(btn_start_server);
		this->AddChild(btn_connect_to_server);
	}

	HOptionsWindow::HOptionsWindow(void)
	{
		setText("Player options");

		prev_window = "main";

		ti_name = new HTextInput;
		btn_color = new HButton;

		ti_name->position = vec2f(0.0f,0.2f);
		btn_color->position = vec2f(0.0f,0.0f);

		ti_name->getLabel().text = "Name";
		btn_color->getLabel().text = "color";

		btn_ok->on_click = goBackWindow;

		controls.push_back(ti_name);
		controls.push_back(btn_color);

		this->AddChild(ti_name);
		this->AddChild(btn_color);
	}

	HAboutWindow::HAboutWindow(void)
	{
		setText("About Heaven World");

		prev_window = "main";

		btn_ok->on_click = goBackWindow;

		lb_text = new HLabel;
		lb_text->text = "\u00A9 2013 klO Ltd.";

		this->AddChild(lb_text);
	}

	HInGameWindow::HInGameWindow(void)
	{
		setText("In game");
		label->visible = false;

		prev_window = "pause";

		btn_back->getLabel().text = "Pause";
		btn_ok->getLabel().text = "View";

		sprite->visible = false;

		btn_back->on_click = pauseBtnClick;
		btn_ok->on_click = toggleViewMode;

		btn_message = new HButton;
		btn_message->position = vec2f(0.0f,0.4);
		btn_message->getSprite().SetScale(vec3f(240.0f,64.0f,32.0f));
		btn_message->getLabel().text = "msg";
		btn_message->on_click = messageBtnClick;
		btn_message->visible = false;

		btn_25p = new HButton;
		btn_50p = new HButton;
		btn_100p = new HButton;

		btn_25p->position = vec2f(-0.4f,0.2f);
		btn_50p->position = vec2f(-0.4f,0.0f);
		btn_100p->position = vec2f(-0.4f,-0.2f);

		btn_25p->on_click = p25BtnClick;
		btn_50p->on_click = p50BtnClick;
		btn_100p->on_click = p100BtnClick;

		btn_25p->getLabel().text = "25%";
		btn_50p->getLabel().text = "50%";
		btn_100p->getLabel().text = "100%";

		btn_info = new HButton;
		btn_info->getLabel().text = "info";
		btn_info->position = vec2f(0.35f,-0.25f);
		btn_info->getSprite().SetScale(vec3f(240.0f,240.0f,1.0f));

		controls.push_back(btn_message);
		controls.push_back(btn_25p);
		controls.push_back(btn_50p);
		controls.push_back(btn_100p);
		controls.push_back(btn_info);

		AddChild(btn_message);
		AddChild(btn_25p);
		AddChild(btn_50p);
		AddChild(btn_100p);
		AddChild(btn_info);
	}

	void HInGameWindow::showMessage(std::string text)
	{
		btn_message->getLabel().text = text;
		btn_message->visible = true;
	}

	void HInGameWindow::update(float dt_ms)
	{
		try
		{
			if(HeavenWorld::instance->players.empty())
				return;

			std::stringstream sstr;
			sstr<<"FPS:"<<floor((1000/dt_ms))<<"\n";

			if(HeavenWorld::instance->selected_island)
				sstr<<"Island\n\""+HeavenWorld::instance->selected_island->name<<"\"\n";
			sstr<<"Iron "<<HeavenWorld::instance->players[static_cast<HGUI*>(HeavenWorld::instance->gui)->interface].resources["iron"]<<"\n";
			sstr<<"Food "<<HeavenWorld::instance->players[static_cast<HGUI*>(HeavenWorld::instance->gui)->interface].resources["food"]<<"";
			btn_info->getLabel().text = sstr.str();
		}
		catch(...)
		{
			// no sides
		}
	}

	void HInGameWindow::realign(Vec2f size)
	{
		HWindow::realign(size);
		btn_message->getSprite().SetScale(vec3f(size.X*0.6f,128.0f,32.0f));
		// btn_info->getSprite().SetScale(vec3f(size.X*0.2f,240.0f,1.0f));
	}

	HPauseWindow::HPauseWindow(void)
	{
		setText("Paused");

		prev_window = "in_game";

		btn_ok->getLabel().text = "Finish";
		btn_back->getLabel().text = "Resume";

		btn_back->on_click = resumeBtnClick;
		btn_ok->on_click = showMainMenuWindow;
	}

	HMainMenuWindow::~HMainMenuWindow(void)
	{
		delete btn_local;
		delete btn_network;
		delete btn_options;
		delete btn_about;
	}

	HTextInputWindow::~HTextInputWindow(void)
	{
		for(HButton *&key:keyboard)
			delete key;

		text->position = orig_text_position;
		if(orig_text_parent)
			orig_text_parent->AddChild(text);
	}

	HLocalGameWindow::~HLocalGameWindow(void)
	{
		delete btn_continue;
		delete btn_new_game;
	}

	HNetworkGameWindow::~HNetworkGameWindow(void)
	{
		delete btn_start_server;
		delete btn_connect_to_server;
	}

	HOptionsWindow::~HOptionsWindow(void)
	{
		delete ti_name;
		delete btn_color;
	}

	HAboutWindow::~HAboutWindow(void)
	{
		delete lb_text;
	}

	HInGameWindow::~HInGameWindow(void)
	{
		delete btn_message;
	}
}