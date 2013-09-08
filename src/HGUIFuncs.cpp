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
}