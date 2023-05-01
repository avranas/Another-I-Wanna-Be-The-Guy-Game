//All data used in the actual game goes here
#ifndef MAIN_GAME_H_
#define MAIN_GAME_H_
#include <SDL.h>
#include "The Kid.h"
#include "Hazards.h"
#include "Worlds and Maps.h"
#include "Text_Box.h"

class MainGame
{
public:
	MainGame();
	~MainGame();
	void go_here(int world, int screen_x, int screen_y, double kid_position_x, double kid_position_y);
	void load_save_data(string file_name);
	void update_timer_display();

	void handle_events();
	void logic();
	void render();
	void finish();
private:
	double bullet_buffer_;
	int time_since_last_reset_;
	Text_Box* mouse_position_x_;
	Text_Box* mouse_position_y_;
	Text_Box* timer_display;
	Text_Box* timer_display_shadow;
	//Displayable_Object* star_hud_empty_;
	Displayable_Object* star_hud_filled_;
	//Displayable_Object* teleporter_hud_empty_;
	Displayable_Object* teleporter_hud_filled_;
};
namespace Game
{
	extern Game_Over_Text* game_over_text;
}

inline void put_camera_in_position();
void update_death_counter_image();

#endif	