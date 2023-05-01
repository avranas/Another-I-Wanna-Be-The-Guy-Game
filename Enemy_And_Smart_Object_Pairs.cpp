#include "stdafx.h"
#include "Enemy_And_Smart_Object_Pairs.h"
#include "Music and Sound.h"

void Mushroom::logic()
{
	if (!started_)
	{
		Game::background_music->pause_music();
		started_ = true;
		Mix_PlayChannel(-1, Sound::drop_mushroom, 0);
	}
	//fixed a crash
	if (target_enemy_ == nullptr || target_enemy_->dead())
	{
		prepare_to_delete();
		return;
	}
	location_.y += g_timer.deltaT_in_seconds() * 130;
	if (rect_rect_collision(display_outline(), target_enemy_->the_parts_[0]->display_outline()))
	{
		target_enemy_->use_mushroom();
		prepare_to_delete();
	}
}
