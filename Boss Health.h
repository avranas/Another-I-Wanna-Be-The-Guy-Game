#ifndef BOSS_HEALTH_H_
#define BOSS_HEALTH_H_
#include <SDL.h>
#include "globals.h"
#include "Image Processing.h"
#include "Enemy.h"

class Boss_Health_Bar
{
public:
	Boss_Health_Bar() :
		bar_text(new Displayable_Object(2, g_screen_height - BAR_Y_OFFSET, Pic::boss_bar_text)),
		bar_frame(new Displayable_Object(BAR_X, g_screen_height - BAR_Y_OFFSET, Pic::boss_bar_frame)),
		anti_health(new Displayable_Object(BAR_X, 10 + g_screen_height - BAR_Y_OFFSET, Pic::anti_health)),
		health(new Displayable_Object(BAR_X, 10 + g_screen_height - BAR_Y_OFFSET, Pic::health))
	{}
	void remove_enemy();
	void render();
	void update();
	void point_to_this_enemy(Enemy* this_one);
private:
	Displayable_Object* anti_health;
	Displayable_Object* health;
	Displayable_Object* bar_text;
	Displayable_Object* bar_frame;
	Enemy* current_enemy_;
	const static double BAR_WIDTH;
	const static double BAR_X;
	const static double BAR_Y_OFFSET;
};

namespace Game
{
	extern Boss_Health_Bar* boss_health_bar;
}

#endif