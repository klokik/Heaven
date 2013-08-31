#ifndef HGUI_H_
#define HGUI_H_

#include <vector>

#include "AEObjectEmpty.h"
#include "AEObjectSprite.h"
#include "AEObjectCamera.h"

#include "Side.hpp"

namespace heaven
{
	//using namespace aengine;

	struct Line
	{
		Vec3f position;
		Vec3f direction;
	};

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
		std::vector<AEObject*> cursor;

		Vec2f size;

		void setValues(void);

		//returns a point and direction of the line, that comes from center of camera to point on screen
		Line getScreenRay(Vec2f screen_pos,AEObjectCamera &camera);

		void attemptToSelect(Vec2f pos);

	public:
		HeavenWorld *world_instance;
		static Side *interface;

		HGUI(HeavenWorld *instance,Side *interface);

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