#include "stdafx.h"
#include "Boss Health.h"

const double Boss_Health_Bar::BAR_WIDTH = 924;
const double Boss_Health_Bar::BAR_X = 82;
const double Boss_Health_Bar::BAR_Y_OFFSET = 50;

void Boss_Health_Bar::remove_enemy()
{
	current_enemy_ = nullptr;
}

void Boss_Health_Bar::render()
{
	if (current_enemy_ == nullptr)
		return;
	health->modify_width_of_current_pic(static_cast<int>(BAR_WIDTH * current_enemy_->HP_percentage()));
	anti_health->modify_width_of_current_pic(static_cast<int>(BAR_WIDTH - BAR_WIDTH * current_enemy_->HP_percentage()));
	anti_health->new_location_x(BAR_X + (BAR_WIDTH * current_enemy_->HP_percentage()));

	health->simple_render();
	anti_health->simple_render();
	bar_text->simple_render();
	bar_frame->simple_render();
}

void Boss_Health_Bar::update()
{

}

void Boss_Health_Bar::point_to_this_enemy(Enemy* this_one)
{
	current_enemy_ = this_one;
}

namespace Game
{
	Boss_Health_Bar* boss_health_bar = nullptr;
}