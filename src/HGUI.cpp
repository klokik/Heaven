#include <algorithm>

#include "AEObjectText.h"
#include "AEObjectSprite.h"
#include "AEVectorMath.h"

#include "HGUI.hpp"
#include "HeavenWorld.hpp"

namespace heaven
{
	using namespace aengine;

	const float distance(Vec3f pt,Line line)
	{
		Vec3f u = line.direction;
		Vec3f A = line.position;

		float t = dot(u,pt-A)/dot(u,u);

		Vec3f B = A+u*t;

		return Length(B-pt);
	}

	Side *HGUI::interface = nullptr;

	HGUI::HGUI(HeavenWorld *instance,Side *interface)
	{
		// projection = AE_ORTHOGRAPHIC;

		world_instance = instance;
		this->interface = interface;

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

		for(int q=0;q<20;q++)
		{
			AEObject *obj = new AEObjectEmpty;
			cursor.push_back(obj);
			this->AddChild(obj);
		}

		this->AddChild(res_info);
		this->AddChild(isl_info);
		this->AddChild(btn_quit);
		this->AddChild(btn_pause);
	}

	void HGUI::setValues(void)
	{
		static_cast<AEObjectText*>(isl_info)->text = "Island \""+world_instance->selected_island->name+"\"";
		static_cast<AEObjectText*>(res_info)->text = "Iron: "+std::to_string((int)world_instance->players[MINE].resources["iron"])+"\n"+
			"Food: "+std::to_string((int)world_instance->players[MINE].resources["food"]);
	}

	Line HGUI::getScreenRay(Vec2f screen_pos,AEObjectCamera *camera)
	{
		Line result = {vec3f(0,0,0),vec3f(0,0,0)};

		if(!camera)
			return result;

		Vec2f d_pos = (screen_pos-size*0.5);
		result.direction = vec3f(d_pos.X,d_pos.Y,-size.X/tan(camera->angle)*sqrt(2)); // for some reason it works (not too acurately)
		result.direction = normalize(result.direction);
		AEMatrix4f4 wmat = camera->GetWorldMatrix();

		Vec4f dir4 = wmat*vec4f(normalize(result.direction),1.0f);

		result = {
			camera->GetAbsPosition(),
			(vec3f(dir4.X,dir4.Y,dir4.Z)-camera->GetAbsPosition())
		};

		return result;
	}

	void HGUI::attemptToSelect(Vec2f pos)
	{
		Line ray = getScreenRay(pos,world_instance->engine.curCamera);

		std::vector<Island*> intersected;
		for(auto &uid_island:world_instance->islands)
			if(distance(uid_island.second->translate,ray)<uid_island.second->bounding_sphere.radius)
				intersected.push_back(uid_island.second);

		for(int q=0;q<20;q++)
			cursor[q]->SetTranslate(ray.position+ray.direction*q*3);

		if(intersected.empty())
			return;

		Island *isl = *std::min_element(intersected.begin(),intersected.end(),
			[&ray](Island *a,Island *b)	{ return
				Length(ray.position-a->translate)<
				Length(ray.position-b->translate);
			});

		world_instance->selected_island = isl;
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

		attemptToSelect(pos);
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
		if(interface)
			interface->send(HPacket::PAUSE);
	}

	void HGUI::iBtnQuitClick(int *param)
	{
		if(interface)
			interface->disconnect();
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