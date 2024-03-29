#include "HGUI.hpp"
#include "HeavenWorld.hpp"

namespace heaven
{
	void goToWindow(int *param)
	{
	}

	void goBackWindow(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);

		if(hgui->active_window)
			hgui->showWindow(hgui->active_window->prev_window);
	}

	void showMainMenuWindow(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);

		hgui->showWindow("main");
	}

	void showAboutWindow(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);

		hgui->showWindow("about");
	}

	void showLocalGameWindow(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);

		hgui->showWindow("local");
	}

	void showNetworkGameWindow(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);

		hgui->showWindow("network");
	}

	void showOptionsWindow(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);

		hgui->showWindow("options");
	}


	void showInGameWindow(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);

		hgui->showWindow("in_game");
	}

	void loadAndStartGame(int *param)
	{
		HeavenWorld::instance->reset();
		HeavenWorld::instance->resume();

		showInGameWindow(nullptr);
	}

	void startNewGame(int *param)
	{
		HeavenWorld::instance->reset();
		HeavenWorld::instance->resume();

		showInGameWindow(nullptr);
	}

	void showTextWindow(int *param)
	{
		// HTextInput *ti = static_cast<HTextInput*>(param);
		// HWindow *wnd = new HTextInputWindow(hgui->active_window,ti);

		// hgui->showWindow(wnd);
	}

	void toggleViewMode(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);

		static bool top_view = false;

		if(top_view)
			hgui->setNormalView();
		else
			hgui->setTopView();

		top_view=!top_view;
	}

	void resumeGame(int *param)
	{
		HeavenWorld::instance->resume();
	}

	void messageBtnClick(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);
		if(hgui->active_window && hgui->active_window->name == "in_game")
			static_cast<HInGameWindow*>(hgui->active_window)->btn_message->visible = false;

		HeavenWorld::instance->storyboard.handleEvent({Storyboard::E_CONTINUE,0,0});

		resumeGame(nullptr);
	}

	void pauseBtnClick(int *param)
	{
		HeavenWorld::instance->pause();
		goBackWindow(nullptr);
	}

	void resumeBtnClick(int *param)
	{
		resumeGame(nullptr);
		goBackWindow(nullptr);
	}

	void p25BtnClick(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);
		hgui->transfer_amount = 0.25f;

		if(hgui->active_window && hgui->active_window->name == "in_game")
		{
			HInGameWindow *wnd = static_cast<HInGameWindow*>(hgui->active_window);

			wnd->btn_25p->getSprite().material->diffuse.vec = vec4f(0.1f,0.1f,0.1f,0.95f);
			wnd->btn_50p->getSprite().material->diffuse.vec = vec4f(0.5f,0.5f,0.5f,0.5f);
			wnd->btn_100p->getSprite().material->diffuse.vec = vec4f(0.5f,0.5f,0.5f,0.5f);
		}
	}

	void p50BtnClick(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);
		hgui->transfer_amount = 0.5f;

		if(hgui->active_window && hgui->active_window->name == "in_game")
		{
			HInGameWindow *wnd = static_cast<HInGameWindow*>(hgui->active_window);

			wnd->btn_25p->getSprite().material->diffuse.vec = vec4f(0.5f,0.5f,0.5f,0.5f);
			wnd->btn_50p->getSprite().material->diffuse.vec = vec4f(0.1f,0.1f,0.1f,0.95f);
			wnd->btn_100p->getSprite().material->diffuse.vec = vec4f(0.5f,0.5f,0.5f,0.5f);
		}
	}

	void p100BtnClick(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);
		hgui->transfer_amount = 1.0f;

		if(hgui->active_window && hgui->active_window->name == "in_game")
		{
			HInGameWindow *wnd = static_cast<HInGameWindow*>(hgui->active_window);

			wnd->btn_25p->getSprite().material->diffuse.vec = vec4f(0.5f,0.5f,0.5f,0.5f);
			wnd->btn_50p->getSprite().material->diffuse.vec = vec4f(0.5f,0.5f,0.5f,0.5f);
			wnd->btn_100p->getSprite().material->diffuse.vec = vec4f(0.1f,0.1f,0.1f,0.95f);
		}
	}

	void linkIslandBtnClick(int *param)
	{
		HGUI *hgui = static_cast<HGUI*>(HeavenWorld::instance->gui);
		HInGameWindow *wnd = static_cast<HInGameWindow*>(hgui->active_window);

		static Island *first_selected = nullptr;

		if(!HeavenWorld::instance->selected_island)
		{
			first_selected = nullptr;
			wnd->btn_link->getLabel().text = "Link";
			return;
		}


		if(first_selected)
		{
			first_selected->linkTo(HeavenWorld::instance->selected_island->uid);
			first_selected = nullptr;
			wnd->btn_link->getLabel().text = "Link";
			return;
		}
		else
		{
			hgui->showInGameMessage("Select second island to link");
			first_selected = HeavenWorld::instance->selected_island;
			wnd->btn_link->getLabel().text = "Cancel";
		}
	}

}