#include "Player.hpp"
#include "HeavenWorld.hpp"

namespace heaven
{
	HumanPlayer::HumanPlayer(void)
	{
		gui = new HGUI(HeavenWorld::instance,uid);
	}
}