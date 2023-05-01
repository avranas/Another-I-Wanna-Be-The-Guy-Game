#include "stdafx.h"
#include "Simple Object.h"
#include "The Kid.h"
#include <vector>
const double Gate::BUTTON_WIDTH = 9;
const double Gate::BUTTON_HEIGHT = 16;
const double Gate::OPENING_TIME = 300;

void Gate::logic()
{
	switch (the_state_)
	{
	case Closed:
	{
		for (auto iter = Game::the_bullets.begin(); iter != Game::the_bullets.end(); ++iter)
		{
			if (rect_rect_collision(button_, (*iter)->the_box()))
			{
				switch_to_animation(1);
				the_state_ = Opening;
				//This will obviously need to be reworked if I plan on adding more of these.
				Mix_PlayChannel(-1, Sound::door_open, 3);
				Game::current_screen->the_tile_map->destroy_a_tile(37, 31);
				Game::current_screen->the_tile_map->destroy_a_tile(37, 32);
				Game::current_screen->the_tile_map->destroy_a_tile(37, 33);
				Game::current_screen->the_tile_map->destroy_a_tile(37, 34);
			}
		}
		break;
	}
	case Opening:
		opening_timer_ += g_timer.deltaT();
		if (opening_timer_ >= OPENING_TIME)
		{
			the_state_ = Open;
			switch_to_animation(2);
		}
		break;
	case Open:
		break;
	}
}


const double Axe::WIDTH = 64;
const double Axe::HEIGHT = 64;

void Axe::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (rect_rect_collision(hit_box(), Game::the_kid->hit_box()))
		{
			//Mix_PlayChannel(-1, Sound::disappearing_block, 10);
			the_state_ = Destroying;
			switch_to_animation(1);
			Game::current_screen->the_special_platforms.push_back(new Leaping_Platform(760, 680, 900, 10800, 4200, 8100));
			Game::current_screen->the_special_platforms.push_back(new Leaping_Platform(584, 680, 900, 10800, 4200, 5400));
			Game::current_screen->the_special_platforms.push_back(new Leaping_Platform(400, 680, 900, 10800, 4200, 2700));
			Game::current_screen->the_special_platforms.push_back(new Leaping_Platform(208, 680, 900, 10800, 4200, 0));
		}
		break;
	case Destroying:
		timer_ += g_timer.deltaT();
		//This is good.
		if (did_it_pass(timer_ - g_timer.deltaT(), timer_, tile_counter_ * 30))
		{
			Mix_PlayChannel(-1, Sound::block_break, 0);
			Game::current_screen->the_tile_map->destroy_a_tile(56 - tile_counter_, 32);
			tile_counter_++;
			if (tile_counter_ == 49)
				the_state_ = Done;
		}
		break;
	case Done:
		break;
	}
}

const double Thundercloud::WARNING_TIME = 750;
const double Thundercloud::ACTIVE_TIME = 100;

void Thundercloud::logic()
{
	timer_ += g_timer.deltaT();
	switch (the_cloud_state_)
	{
	case Waiting:
		if (timer_ >= waiting_time)
		{
			the_cloud_state_ = Warning;
			timer_ -= waiting_time;
			switch_to_animation(1);
		}
		break;
	case Warning:
		if (timer_ >= WARNING_TIME)
		{
			the_cloud_state_ = Active;
			timer_ -= WARNING_TIME;
			Mix_PlayChannel(-1, Sound::block_break, 0);
			Game::current_screen->the_hazards_layer_1.push_back(new Thunderbolt(location_.x, location_.y + 23));
			switch_to_animation(2);
		}
		break;
	case Active:
		if (timer_ >= ACTIVE_TIME)
			ready_to_delete_ = true;
		break;
	}
}
const double Big_Fireball::SCREEN_TIME = 1700;
void Big_Fireball::logic()
{
	//I don't like this. This is bad. But I can't think of anything better. Oh well.
	//Warning:: This will cause problems if I decide to add more enemies to the Barry fight,
	//which I'm not planning on doing now. So it's cool!!
	//Also, if I change the entry_point of Barry the values below will also need to change.
	center_over_this_point(*follow_this_);
	timer_ += g_timer.deltaT();
	if (timer_ >= SCREEN_TIME)
		ready_to_delete_ = true;
}

void Link::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= dig_frequency_)
	{
		timer_ -= dig_frequency_;
		double new_velocity = static_cast<double>((rand() % 340 * -1));
		std::cout << new_velocity << std::endl;
		Game::current_screen->the_hazards_layer_1.push_back(new Thrown_Rock(mid_point_x() - 10, mid_point_y(), new_velocity));
	}

}
