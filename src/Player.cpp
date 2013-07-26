#include "Player.hpp"

namespace heaven
{
	HumanPlayer::HumanPlayer(void)
	{
		gui = new HGUI(HeavenWorld::instance,this);
	}

	HumanPlayer::~HumanPlayer(void)
	{
	}
}