#ifndef HGUI_H_
#define HGUI_H_

#include <vector>

#include "AEObjectEmpty.h"
#include "AEObjectText.h"
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
	class Island;
	class HLabel;
	class HButton;
	class HWindow;
	class HTextInput;

	void goToWindow(int *param);
	void goBackWindow(int *param);
	void showMainMenuWindow(int *param);
	void showAboutWindow(int *param);
	void showLocalGameWindow(int *param);
	void showNetworkGameWindow(int *param);
	void showOptionsWindow(int *param);
	void showInGameWindow(int *param);
	void loadAndStartGame(int *param);
	void startNewGame(int *param);
	void showTextWindow(int *param);
	void toggleViewMode(int *param);
	void resumeGame(int *param);
	void pauseBtnClick(int *param);
	void resumeBtnClick(int *param);
	void messageBtnClick(int *param);
	void p25BtnClick(int *param);
	void p50BtnClick(int *param);
	void p100BtnClick(int *param);
	void linkIslandBtnClick(int *param);

	class HGUI: public aengine::AEObjectEmpty
	{
	protected:
		std::vector<aengine::AEObject*> cursor;

		std::vector<HWindow*> windows;

		Vec2f size;

		bool is_top_view;
		bool is_normal_view;
		struct
		{
			float distance;
			float roll;
			float pitch;
			float yaw;
		} last_top_view_state,last_normal_view_state;

		Island *isl_from;
		Island *isl_target;

		//returns a point and direction of the line, that comes from center of camera to point on screen
		Line getScreenRay(Vec2f screen_pos,aengine::AEObjectCamera &camera);

		bool attemptToSelect(Vec2f pos);

		void initWindows(void);

	public:
		HeavenWorld *world_instance;
		static uint32_t interface;

		HWindow *active_window;

		float transfer_amount;

		HGUI(HeavenWorld *instance,uint32_t side_uid);

		void update(float dt_ms);
		void realign(Vec2f size);

		void mouseDown(Vec2f pos);
		void mouseUp(Vec2f pos);
		void mouseMove(Vec2f pos,Vec2f delta,int key);	// key is one of {1..5}

		void showWindow(std::string name,HTextInput *ti=nullptr);

		void setNormalView(void);
		void setTopView(void);

		void showInGameMessage(std::string text);
	};

	class HLabel: public aengine::AEObjectText
	{};

	class HButton: public aengine::AEObjectEmpty
	{
	protected:
		HLabel *label;
		aengine::AEObjectSprite *sprite;

	public:
		Vec2f position;

		AE_EVENT on_click;	// attributes {Vec2f}
		AE_EVENT on_hover;	// attributes {Vec2f}

		HButton(void);

		virtual void realign(Vec2f size);

		virtual bool click(Vec2f pos);
		void hover(Vec2f pos);
		bool isOver(Vec2f pos);

		HLabel &getLabel(void);
		aengine::AEObjectSprite &getSprite(void);

		virtual ~HButton(void);
	};

	class HTextInput: public HButton
	{
	public:
		HTextInput(void);

		std::string getText(void);
	};

	class HWindow: public HButton
	{
	protected:
		std::vector<HButton*> controls;

		HButton *btn_back;
		HButton *btn_ok;

	public:

		std::string prev_window;

		HWindow(void);

		void setText(std::string text);

		virtual bool click(Vec2f pos);

		virtual void realign(Vec2f size);

		virtual ~HWindow(void);
	};

	class HTextInputWindow: public HWindow
	{
	protected:
		std::vector<HButton*> keyboard;
		HTextInput *text;

		Vec2f orig_text_position;
		aengine::AEObject *orig_text_parent;
	public:
		HTextInputWindow(HTextInput *input);

		virtual ~HTextInputWindow(void);
	};

	class HLoadWindow: public HWindow
	{
	protected:
		HLabel *lb_progress;
	public:
		HLoadWindow(void);
		virtual ~HLoadWindow();
	};

	class HMainMenuWindow: public HWindow
	{
	protected:
		HButton *btn_local;
		HButton *btn_network;
		HButton *btn_options;
		HButton *btn_about;
	public:
		HMainMenuWindow(void);
		virtual ~HMainMenuWindow();
	};

	class HLocalGameWindow: public HWindow
	{
	protected:
		HButton *btn_continue;
		HButton *btn_new_game;
	public:
		HLocalGameWindow(void);
		virtual ~HLocalGameWindow();
	};

	class HNetworkGameWindow: public HWindow
	{
	protected:
		HButton *btn_start_server;
		HButton *btn_connect_to_server;
	public:
		HNetworkGameWindow(void);
		virtual ~HNetworkGameWindow();
	};

	class HOptionsWindow: public HWindow
	{
	protected:
		HTextInput *ti_name;
		HButton *btn_color;
	public:
		HOptionsWindow(void);
		virtual ~HOptionsWindow();
	};

	class HAboutWindow: public HWindow
	{
	protected:
		HLabel *lb_text;

	public:
		HAboutWindow(void);
		virtual ~HAboutWindow();
	};

	class HServerSetupWindow: public HWindow
	{
	protected:
		HTextInput *ti_name;
		HTextInput *ti_port;
		HTextInput *ti_number_of_players;
	public:
		HServerSetupWindow(void);
		virtual ~HServerSetupWindow();
	};

	class HClientSetupWindow: public HWindow
	{
	protected:
		HTextInput *ti_address;
		HTextInput *ti_port;
	public:
		HClientSetupWindow(void);
		virtual ~HClientSetupWindow();
	};

	class HPauseWindow: public HWindow
	{
	protected:
	public:
		HPauseWindow(void);
//		virtual ~HPauseWindow();
	};

	class HInGameWindow: public HWindow
	{
	protected:
		HButton *btn_message;

		HButton *btn_25p;
		HButton *btn_50p;
		HButton *btn_100p;

		HButton *btn_info;
		HButton *btn_link;

		friend void messageBtnClick(int *param);
		friend void p25BtnClick(int *param);
		friend void p50BtnClick(int *param);
		friend void p100BtnClick(int *param);
		friend void linkIslandBtnClick(int *param);
	public:
		HInGameWindow(void);

		void showMessage(std::string text);
		void update(float dt_ms);
		virtual void realign(Vec2f size);

		virtual ~HInGameWindow();
	};
}

#endif /* HGUI_H_ */