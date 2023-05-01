#include "stdafx.h"
#include "Save Box.h"
#include "The Kid.h"

const double Save_Box::HEIGHT = 32;
const double Save_Box::WIDTH = 32;

Save_File* save_file = nullptr;
Common_Save_Data* common_save_data = nullptr;

void Save_Box::logic()
{
	for (auto iter = Game::the_bullets.begin(); iter != Game::the_bullets.end(); ++iter)
	{
		if (rect_rect_collision((*iter)->the_box(), the_box()) && !Game::the_kid->dead() && !saving_)
		{
			saving_ = true;
			switch_to_animation(1);
			save_the_game(Game::the_kid->location_x(), Game::the_kid->location_y());
		}
	}

	if (saving_)
	{
		save_animation_timer_ += g_timer.deltaT();
		if (save_animation_timer_ > SAVE_ANIMATION_DURATION)
		{
			saving_ = false;
			save_animation_timer_ = 0;
			switch_to_animation(0);
		}
	}
}

void Save_File::update_location_and_save(double position_x, double position_y)
{
	character_position_x_ = position_x;
	character_position_y_ = position_y;
	world_ = Game::current_world->world_number();
	screen_x_ = Game::current_world->current_position_x();
	screen_y_ = Game::current_world->current_position_y();
	write_to_file();
}
void Save_File::increment_deaths_and_save()
{
	death_counter_++;
	write_to_file();
}
void Save_File::get_star_and_save()
{
	has_star_ = true;
	write_to_file();
}

void Save_File::get_teleporter_and_save()
{
	has_teleporter_ = true;
	write_to_file();
}


void Save_File::increment_resets_and_save()
{
	reset_counter_++;
	write_to_file();
}

void Save_File::update_time_and_save()
{
	time_ += g_timer.deltaT();
	write_to_file();
}

void save_the_game(double new_position_x, double new_position_y)
{
	Mix_PlayChannel(-1, Sound::save, 0);
	Mix_PlayChannel(-1, Sound::disappearing_block, 0);
	save_file->update_location_and_save(new_position_x, new_position_y);
}

void Save_Switch::logic()
{
	switch (the_state_)
	{
	case Checking:
		if (rect_rect_collision(display_outline(), Game::the_kid->kid_hit_box()))
		{
			the_state_ = Pressed;
			if (!wuss_)
				switch_to_animation(1);
			else
				switch_to_animation(3);
			previous_y_ += 16;
			location_.y += 16;
		}
		else
		{
			the_state_ = Waiting;
			if (!wuss_)
				switch_to_animation(0);
			else
				switch_to_animation(2);
		}
		break;
	case Waiting:
		if (recent_collision_found_)
		{
			recent_collision_found_ = false;
			the_state_ = Pressed;
			if (!wuss_)
				switch_to_animation(1);
			else
				switch_to_animation(3);
			previous_y_ += 16;
			location_.y += 16;
			Game::camera->bump(4, 400);
			save_the_game(location_.x + 11, location_.y - 19);
		}
		break;
	case Pressed:
		break;
	}

}