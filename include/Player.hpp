#ifndef PLAYER_H_
#define PLAYER_H_

#include "Side.hpp"
#include "HGUI.hpp"

namespace heaven
{
	class HumanPlayer: public Side
	{
	public:
		HGUI *gui;

		HumanPlayer(void);
		~HumanPlayer();
	};

	class DedicatedPlayer: public Side
	{
	public:
		DedicatedPlayer(void);
		~DedicatedPlayer();
	};

	class DummyPlayer: public Side
	{
	public:
		DummyPlayer(void);
		~DummyPlayer();
	};
}

#endif /* PLAYER_H_ */