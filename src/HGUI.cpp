#include "AEObjectText.h"
#include "AEObjectSprite.h"
#include "AEVectorMath.h"

#include "HGUI.hpp"
#include "HeavenWorld.hpp"

namespace heaven
{
	using namespace aengine;


	HGUI::HGUI(HeavenWorld *instance)
	{
		projection = AE_ORTHOGRAPHIC;

		world_instance = instance;

		res_info = new AEObjectText;
		isl_info = new AEObjectText;

		btn_quit = new HButton(this);
		static_cast<AEObjectSprite*>(btn_quit)->material->diffuse.vec = vec4f(1.0f,0.0f,0.0f,0.5f);
		static_cast<HButton*>(btn_quit)->on_click = iBtnQuitClick;

		btn_pause = new HButton(this);
		static_cast<AEObjectSprite*>(btn_pause)->material->diffuse.vec = vec4f(0.0f,0.0f,1.0f,0.5f);
		static_cast<HButton*>(btn_pause)->on_click = iBtnPauseClick;

		static_cast<AEObjectText*>(res_info)->text = "Resource info";
		static_cast<AEObjectText*>(isl_info)->text = "Island info";

		static_cast<AEObjectText*>(res_info)->alignment = AE_LEFT;
		static_cast<AEObjectText*>(isl_info)->alignment = AE_RIGHT;

		isl_info->SetScale(vec3f(8.0f,8.0f,8.0f));

		this->AddChild(res_info);
		this->AddChild(isl_info);
		this->AddChild(btn_quit);
		this->AddChild(btn_pause);
	}

	void HGUI::setValues(void)
	{
		static_cast<AEObjectText*>(isl_info)->text = "Island \""+world_instance->selected_island->name+"\"";
		static_cast<AEObjectText*>(res_info)->text = "Iron: "+std::to_string((int)world_instance->resources[MINE]["iron"])+"\n"+
			"Food: "+std::to_string((int)world_instance->resources[MINE]["food"]);
	}

	void HGUI::update(void)
	{
		setValues();
	}

	void HGUI::realign(Vec2f size)
	{
		this->size = size;
		
		res_info->SetTranslate(vec3f(16.0f,size.Y - 25.0f,0.0f));
		isl_info->SetTranslate(vec3f(size.X - 16.0f,128.0f,0.0f));

		btn_quit->SetTranslate(vec3f(132.0f,size.Y-64.0f,0.0f));
		btn_pause->SetTranslate(vec3f(84.0f,size.Y-64.0f,0.0f));
	}

	void HGUI::mouseDown(Vec2f pos)
	{
		if(static_cast<HButton*>(btn_quit)->isOver(pos))
			static_cast<HButton*>(btn_quit)->click(pos);
		if(static_cast<HButton*>(btn_pause)->isOver(pos))
			static_cast<HButton*>(btn_pause)->click(pos);
	}

	void HGUI::mouseMove(Vec2f pos,Vec2f delta,int key)
	{
		// Middle key
		if(key == 2)
		{
			HeavenWorld::instance->view_target->relYaw(-delta.X);
			HeavenWorld::instance->view_target->relPitch(delta.Y);
		}
	}

	void HGUI::iBtnRestartClick(int *param)
	{

	}

	void HGUI::iBtnPauseClick(int *param)
	{
		std::cout<<"Pause"<<std::endl;
	}

	void HGUI::iBtnQuitClick(int *param)
	{
		HeavenWorld::instance->engine.Stop();
	}


	HButton::HButton(HGUI *gui)
	{
		on_click = nullptr;
		on_hover = nullptr;

		material = gui->world_instance->engine.scene->materials.New();
		SetScale(vec3f(32.0f,32.0f,32.0f));
	}

	void HButton::click(Vec2f pos)
	{
		if(on_click)
			on_click(reinterpret_cast<int*>(&pos));
	}

	void HButton::hover(Vec2f pos)
	{
		if(on_hover)
			on_hover(reinterpret_cast<int*>(&pos));
	}

	bool HButton::isOver(Vec2f pos)
	{
		// too rough but who cares?
		return (SqrLength(vec3f(pos,0.0f)-translate)<=SqrLength(scale)*0.25); // no further then one half of scale vector
	}
}