#ifndef HGUI_H_
#define HGUI_H_

#include "AEObjectEmpty.h"
#include "AEObjectSprite.h"

namespace heaven
{
	//using namespace aengine;


	class HeavenWorld;

	class HGUI: public AEObjectEmpty
	{
	protected:
		AEObject *minimap;
		AEObject *balance_bar;
		AEObject *isl_info;
		AEObject *res_info;
		AEObject *btn_restart;
		AEObject *btn_pause;
		AEObject *btn_quit;

		Vec2f size;

		void setValues(void);

	public:
		HeavenWorld *world_instance;

		HGUI(HeavenWorld *instance);

		void update(void);
		void realign(Vec2f size);

		void mouseDown(Vec2f pos);
		void mouseMove(Vec2f pos,Vec2f delta,int key);	// key is one of {1..5}

		static void iBtnRestartClick(int *param);
		static void iBtnPauseClick(int *param);
		static void iBtnQuitClick(int *param);
	};

	class HButton: public AEObjectSprite
	{
	public:
		AE_EVENT on_click;	// attributes {Vec2f}
		AE_EVENT on_hover;	// attributes {Vec2f}

		HButton(HGUI *gui);

		void click(Vec2f pos);
		void hover(Vec2f pos);
		bool isOver(Vec2f pos);
	};
}

#endif /* HGUI_H_ */