#include <algorithm>
#include <sstream>
#include <cstring>

#include "AEObjectText.h"
#include "AEObjectSprite.h"
#include "AEVectorMath.h"
#include "AEMaterialManager.h"

#include "HGUI.hpp"
#include "HeavenWorld.hpp"
#include "Island.hpp"

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
		HGUI::interface = interface;

		active_window = nullptr;

		name = "HGUI";

		res_info = new AEObjectText;	res_info->name = "res_info";
		isl_info = new AEObjectText;	isl_info->name = "isl_info";

		// btn_quit = new HButton;
		// btn_quit->name = "btnQuit";
		// static_cast<AEObjectSprite*>(btn_quit)->material->diffuse.vec = vec4f(1.0f,0.0f,0.0f,0.5f);
		// static_cast<HButton*>(btn_quit)->on_click = iBtnQuitClick;

		// btn_pause = new HButton;
		// btn_pause->name = "btnPause";
		// static_cast<AEObjectSprite*>(btn_pause)->material->diffuse.vec = vec4f(0.0f,0.0f,1.0f,0.5f);
		// static_cast<HButton*>(btn_pause)->on_click = iBtnPauseClick;

		// static_cast<AEObjectText*>(res_info)->text = "Resource info";
		// static_cast<AEObjectText*>(isl_info)->text = "Island info";

		// static_cast<AEObjectText*>(res_info)->alignment = AE_LEFT;
		// static_cast<AEObjectText*>(isl_info)->alignment = AE_RIGHT;

		isl_info->SetScale(vec3f(8.0f,8.0f,8.0f));

		for(int q=0;q<20;q++)
		{
			AEObject *obj = new AEObjectEmpty;
			cursor.push_back(obj);
			this->AddChild(obj);
		}

		// this->AddChild(res_info);
		// this->AddChild(isl_info);
		// this->AddChild(btn_quit);
		// this->AddChild(btn_pause);

		last_normal_view_state = {20.0f,0.0f,-30.0f,0.0f};
		last_top_view_state = {30.0f,0.0f,-90.0f,0.0f};
		is_top_view = false;
		is_normal_view = true;

		setNormalView();

		initWindows();
	}

	void HGUI::setValues(void)
	{
		std::stringstream res_str;

		res_str<<"Iron: "<<world_instance->players[MINE].resources["iron"]<<"\n"
			<<"Food: "<<world_instance->players[MINE].resources["food"];

		if(world_instance->selected_island)
			static_cast<AEObjectText*>(isl_info)->text = "Island \""+world_instance->selected_island->name+"\"";
		static_cast<AEObjectText*>(res_info)->text = res_str.str();
	}

	Line HGUI::getScreenRay(Vec2f screen_pos,AEObjectCamera &camera)
	{
		Line ray = {vec3f(0,0,0),vec3f(0,0,0)};

		Vec2f frag_pos = (screen_pos-size*0.5); // shift coordinates to screen center
		ray.direction = vec3f(
			frag_pos.X,
			frag_pos.Y,
			-(size.X/2)/tan(camera.angle/57.2957795131f/2));
		AEMatrix4f4 wmat = camera.GetWorldMatrix();

		Vec4f dir4 = wmat*vec4f(ray.direction,1.0f);

		ray = {
			camera.GetAbsPosition(),
			normalize(vec3f(dir4.X,dir4.Y,dir4.Z)-camera.GetAbsPosition())
		};

		return ray;
	}

	bool HGUI::attemptToSelect(Vec2f pos)
	{
		Line ray = getScreenRay(pos,*world_instance->engine.curCamera);

		std::vector<Island*> intersected;
		for(auto &uid_island:world_instance->islands)
			if(distance(uid_island.second->translate,ray)<uid_island.second->bounding_sphere.radius)
				intersected.push_back(uid_island.second);

		for(int q=0;q<20;q++)
			cursor[q]->SetTranslate(ray.position+ray.direction*q*3);

		if(intersected.empty())
			return false;

		Island *isl = *std::min_element(intersected.begin(),intersected.end(),
			[&ray](Island *a,Island *b)	{ return
				Length(ray.position-a->translate)<
				Length(ray.position-b->translate);
			});

		world_instance->selected_island = isl;

		return true;
	}

	void HGUI::initWindows(void)
	{
		HWindow *wnd;

		wnd = new HMainMenuWindow;
		wnd->name = "main";
		windows.push_back(wnd);
		this->AddChild(wnd);

		wnd = new HLocalGameWindow;
		wnd->name = "local";
		windows.push_back(wnd);
		this->AddChild(wnd);

		wnd = new HNetworkGameWindow;
		wnd->name = "network";
		windows.push_back(wnd);
		this->AddChild(wnd);

		wnd = new HOptionsWindow;
		wnd->name = "options";
		windows.push_back(wnd);
		this->AddChild(wnd);

		wnd = new HAboutWindow;
		wnd->name = "about";
		windows.push_back(wnd);
		this->AddChild(wnd);

		wnd = new HInGameWindow;
		wnd->name = "in_game";
		windows.push_back(wnd);
		this->AddChild(wnd);

		wnd = new HPauseWindow;
		wnd->name = "pause";
		windows.push_back(wnd);
		this->AddChild(wnd);

		showWindow("main");
	}

	void HGUI::showWindow(std::string name,HTextInput *ti)
	{
		if(active_window)
		{
			active_window->visible = false;
			active_window = nullptr;
		}

		if(name=="exit")
			world_instance->engine.Stop();

		for(HWindow *&wnd:windows)
		{
			if(wnd->name==name)
			{
				wnd->visible = true;
				active_window = wnd;
				break;
			}
			else
				continue;//wnd->visible = false;
		}
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

		// btn_quit->SetTranslate(vec3f(132.0f,size.Y-64.0f,0.0f));
		// btn_pause->SetTranslate(vec3f(84.0f,size.Y-64.0f,0.0f));

		for(HWindow *&wnd:windows)
			wnd->realign(size);
	}

	void HGUI::mouseDown(Vec2f pos)
	{
		if(active_window)
		{
			if(!active_window->click(pos)&&active_window->name=="in_game")
			{
				if(attemptToSelect(pos))
					isl_from = world_instance->selected_island;
				else
					isl_from = nullptr;
				world_instance->selected_island = nullptr;
			}
		}
		else
			AEPrintLog("No active window");
	}

	void HGUI::mouseUp(Vec2f pos)
	{
		if(active_window&&active_window->name=="in_game")
		{
			if(isl_from)
			{
				if(attemptToSelect(pos))
				{
					isl_target = world_instance->selected_island;
					world_instance->transfer(isl_from->uid,isl_target->uid,1.0f);
				}

				isl_from = nullptr;
			}
		}
	}

	void HGUI::mouseMove(Vec2f pos,Vec2f delta,int key)
	{
		// Any key
		if(key > 0 && isl_from == nullptr)
		{
			world_instance->view_target->relYaw(-delta.X);
			world_instance->view_target->relPitch(delta.Y);
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

	void HGUI::setNormalView(void)
	{
		if(is_top_view)
		{
			auto ct = HeavenWorld::instance->view_target;
			last_top_view_state = {ct->getDistance(),0,ct->getPitch(),ct->getYaw()};

			ct->setDistance(last_normal_view_state.distance);
			ct->setPitch(last_normal_view_state.pitch);
			ct->setYaw(last_normal_view_state.yaw);
		}
		is_normal_view = true;
		is_top_view = false;
	}

	void HGUI::setTopView(void)
	{
		if(is_normal_view)
		{
			auto ct = HeavenWorld::instance->view_target;
			last_normal_view_state = {ct->getDistance(),0,ct->getPitch(),ct->getYaw()};

			ct->setDistance(last_top_view_state.distance);
			ct->setPitch(last_top_view_state.pitch);
			ct->setYaw(last_top_view_state.yaw);
		}

		is_normal_view = false;
		is_top_view = true;
	}

	HButton::HButton(void)
	{
		name = "h_button";
		projection = AE_ORTHOGRAPHIC;

		on_click = nullptr;
		on_hover = nullptr;

		label = new HLabel;
		label->projection = AE_ORTHOGRAPHIC;
		label->text = "Button";
		AddChild(label);

		sprite = new AEObjectSprite;
		sprite->projection = AE_ORTHOGRAPHIC;
		sprite->material = HeavenWorld::instance->engine.scene->materials.New();
		sprite->material->diffuse.vec = vec4f(0.3f,0.3f,0.3f,0.9f);
		sprite->material->transparent = true;
		sprite->SetScale(vec3f(128.0f,64.0f,32.0f));
		AddChild(sprite);
		position = vec2f(0.0f,0.0f);
	}

	void HButton::realign(Vec2f size)
	{
		SetTranslate(vec3f(size*position,0.0f));
	}

	bool HButton::click(Vec2f pos)
	{
		if(on_click)
			on_click(reinterpret_cast<int*>(&pos));

		return true;
	}

	void HButton::hover(Vec2f pos)
	{
		if(on_hover)
			on_hover(reinterpret_cast<int*>(&pos));
	}

	bool HButton::isOver(Vec2f pos)
	{
		// too rough but who cares?

		return (SqrLength(vec3f(pos,0.0f)-GetAbsPosition())<=SqrLength(getSprite().GetAbsScale())*0.25); // no further then one half of scale vector
	}

	HLabel &HButton::getLabel(void)
	{
		return *label;
	}

	AEObjectSprite &HButton::getSprite(void)
	{
		return *sprite;
	}

	HButton::~HButton(void)
	{
		delete label;
		delete sprite;
	}

	HTextInput::HTextInput(void)
	{
		name = "h_text_input";
		label->text = "HTextInput";
	}

	std::string HTextInput::getText(void)
	{
		return label->text;
	}

	HWindow::HWindow(void)
	{
		name = "h_window";
		visible = false;

		btn_back = new HButton;
		btn_ok = new HButton;

		btn_back->position = vec2f(-0.4f,0.4f);
		btn_ok->position = vec2f(0.4f,0.4f);

		btn_back->getLabel().text = "back";
		btn_ok->getLabel().text = "ok";

		btn_back->on_click = goBackWindow;

		sprite->material->diffuse.vec = vec4f(0.0f,0.0f,0.0f,0.6f);

		// label->position = vec2f(0.0f,0.4f);

		controls.push_back(btn_back);
		controls.push_back(btn_ok);

		AddChild(btn_back);
		AddChild(btn_ok);
	}

	void HWindow::setText(std::string text)
	{
		label->text = text;
	}

	bool HWindow::click(Vec2f pos)
	{
		for(HButton *&control:controls)
		{
			if(!control)
			{
				AEPrintLog("invalid control");
				continue;
			}

			if(control->isOver(pos)&&control->visible)
			{
				AEPrintLog("click");
				control->click(pos);
				return true;
			}
		}

		return false;
	}

	void HWindow::realign(Vec2f size)
	{
		HButton::realign(size);

		sprite->SetScale(vec3f(size,1.0f));
		this->SetTranslate(vec3f(size*0.5f,0.0f));

		for(HButton *&control:controls)
		{
			if(!control)
			{
				AEPrintLog("invalid control");
				continue;
			}

			control->realign(size);
		}

		label->SetTranslate(vec3f(size*vec2f(0.0f,0.4f),0.0f));
	}

	HWindow::~HWindow(void)
	{
		delete btn_back;
		delete btn_ok;
	}
}