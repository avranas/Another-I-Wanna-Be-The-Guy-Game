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
				//This will need to be reworked if I plan on adding more of these.
				Game::current_screen->the_tile_map->destroy_a_tile(55, 39);
				Game::current_screen->the_tile_map->destroy_a_tile(55, 40);
				Game::current_screen->the_tile_map->destroy_a_tile(55, 41);
				Game::current_screen->the_tile_map->destroy_a_tile(55, 42);
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
