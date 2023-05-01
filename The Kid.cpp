#include "stdafx.h"
#include "The Kid.h"
#include "main_game.h"
const double The_Kid::THE_KID_WIDTH = 11;
const double The_Kid::THE_KID_HEIGHT = 19;
const double The_Kid::KID_HIT_BOX_OFFSET_X = 8;
const double The_Kid::KID_HIT_BOX_OFFSET_Y = 1;
const double The_Kid::THE_KID_VELOCITY = 150;
const double The_Kid::JUMP_POWER = 330;
const double The_Kid::MAX_JUMPING_TIME = 70;
const double The_Kid::BULLET_OFFSET_X = 15;
const double The_Kid::BULLET_OFFSET_Y = 9;
const double Blood::BLOOD_WIDTH = 1;
const double Blood::BLOOD_HEIGHT = 1;

void The_Kid::test_for_collision(Tiled_Map* the_map)
{
	for (int x = static_cast<int>(projection_left() / G_TILE_WIDTH); x <= static_cast<int>((projection_left() + hit_box_w()) / G_TILE_WIDTH); ++x)
	{
		for (int y = static_cast<int>(projection_top() / G_TILE_HEIGHT); y <= static_cast<int>((projection_top() + hit_box_h()) / G_TILE_WIDTH); ++y)
		{
			if (!the_map->is_this_tile_passable(x, y))
			{
				if (the_map->is_this_tile_passable(x - 1, y))
				{
					if (the_map->get_tile(x, y)->handle_collision_with_tile_to_the_left(&kid_hit_box(), projected_point()))
					{
						set_hit_box_right(the_map->get_tile(x, y)->left_of_tile());
						set_velocity_x(0);
						remove_additional_velocity_x();
					}
				}
				if (the_map->is_this_tile_passable(x + 1, y))
				{
					if (the_map->get_tile(x, y)->handle_collision_with_tile_to_the_right(&kid_hit_box(), projected_point()))
					{
						set_hit_box_left(the_map->get_tile(x, y)->right_of_tile());
						set_velocity_x(0);
						remove_additional_velocity_x();
					}
				}
				if (the_map->is_this_tile_passable(x, y - 1))
				{
					if (the_map->get_tile(x, y)->handle_collision_with_tile_on_top(&kid_hit_box(), projected_point()))
					{
						//std::cout << the_map->get_tile(x ,y)->bottom_of_tile() - KID_HIT_BOX_OFFSET_Y - THE_KID_HEIGHT - location_.y << std::endl;
						set_hit_box_bottom(the_map->get_tile(x, y)->top_of_tile() - 0.001);
						land();
						remove_additional_velocity_y();
					}
				}
				if (the_map->is_this_tile_passable(x, y + 1))
				{
					if (the_map->get_tile(x, y)->handle_collision_with_tile_on_bottom(&kid_hit_box(), projected_point()))
					{
						set_hit_box_top(the_map->get_tile(x, y)->bottom_of_tile());
						bump_head();
						remove_additional_velocity_y();
					}
				}
			}
		}
	}
}

void The_Kid::test_for_collision(std::vector<Tile*>* extra_walls)
{
	for (auto iter = extra_walls->begin(); iter != extra_walls->end(); iter++)
	{
		if (!(*iter)->passable())
		{
			if ((*iter)->handle_collision_with_tile_to_the_left(&kid_hit_box(), projected_point()))
			{
				set_hit_box_right((*iter)->left_of_tile());
				set_velocity_x(0);
				remove_additional_velocity_x();
			}
			if ((*iter)->handle_collision_with_tile_to_the_right(&kid_hit_box(), projected_point()))
			{
				set_hit_box_left((*iter)->right_of_tile());
				set_velocity_x(0);
				remove_additional_velocity_x();
			}
			if ((*iter)->handle_collision_with_tile_on_top(&kid_hit_box(), projected_point()))
			{
				set_hit_box_bottom((*iter)->top_of_tile() - 0.001);
				land();
				remove_additional_velocity_y();
			}
			if ((*iter)->handle_collision_with_tile_on_bottom(&kid_hit_box(), projected_point()))
			{
				set_hit_box_top((*iter)->bottom_of_tile());
				bump_head();
				remove_additional_velocity_y();
			}
		}
	}
}

void The_Kid::handle_movement()
{
	if (dead_)
	{
		return;
	}
	//movement from keys
	if (left_)
	{
		xVelocity_ = -THE_KID_VELOCITY * g_timer.deltaT_in_seconds();
	}
	if (right_)
	{
		xVelocity_ = THE_KID_VELOCITY * g_timer.deltaT_in_seconds();
	}
	//Gravity
	double newVelocity = 0;
	//Gravity behaves differently when I'm holding down the jump button, so\
	I need to do this.
	if (jumping_time_ > 0)
	{
		jumping_time_ += g_timer.deltaT();
		modify_velocity_without_gravity(&yVelocity_, &force_from_gravity_);
		if (jumping_time_ > MAX_JUMPING_TIME)
			jumping_time_ = 0;
	}
	else
	{
		modify_velocity_with_gravity_and_limit(&yVelocity_, &force_from_gravity_);
	}
	if (yVelocity_ != 0)
	{
		first_jump_ = true;
	}
	//Here I'm going to handle collision tests with moving platforms
	for (auto platform_iter = Game::current_screen->the_special_platforms.begin();
		platform_iter != Game::current_screen->the_special_platforms.end();
		++platform_iter)
	{
		if (test_collision_with_platform(*(*platform_iter)) && (*platform_iter)->test_for_collision())
		{
			(*platform_iter)->collision_found();
			additional_velocity_x_ = (*platform_iter)->x_velocity();
			additional_velocity_y_ =
				((*platform_iter)->top()) - KID_HIT_BOX_OFFSET_Y - THE_KID_HEIGHT - location_.y;
			land();
		}
	}
	//Collision detection with tilemap takes place here
	test_for_collision(Game::current_world->the_tile_map());
	test_for_collision(&Game::current_screen->extra_walls);
	if (immobilized_ > 0)
	{
		immobilized_ -= g_timer.deltaT();
	}
	else
	{
		set_hit_box_left(projection_left());
		immobilized_ = 0;
	}
	set_hit_box_top(projection_top());
	additional_velocity_y_ = 0;
	additional_velocity_x_ = 0;
	//Now that we have a new position, we can test boundaries on the screen.
	if (hit_box_center_x() > Game::camera->x_right_main())
	{
		Game::current_world->right_one_screen();
	}
	if (hit_box_center_x() < Game::camera->x_main())
	{
		Game::current_world->left_one_screen();
	}
	if (hit_box_center_y() > Game::camera->y_bottom_main())
	{
		Game::current_world->down_one_screen();
	}
	if (hit_box_center_y() < Game::camera->y_main())
	{
		Game::current_world->up_one_screen();
	}
}

void The_Kid::kill_the_kid()
{
	Game::background_music->loop_this_track(Music::death_theme);
	dead_ = true;
	Mix_PlayChannel(-1, Sound::the_kid_dies, 0);
	save_file->increment_deaths_and_save();
	update_death_counter_image();
	add_some_blood(200, hit_box_center_x(), hit_box_center_y());
	Game::current_screen->add_a_new_smart_object(new Dead_Kid_1(hit_box_center_x(), hit_box_center_y(), rand() % 2000 - 1000, rand() % 2000 - 1000),
		false, false);
	Game::current_screen->add_a_new_smart_object(new Dead_Kid_2(hit_box_center_x(), hit_box_center_y(), rand() % 2000 - 1000, rand() % 2000 - 1000),
		false, false);
	Game::current_screen->add_a_new_smart_object(new Dead_Kid_3(hit_box_center_x(), hit_box_center_y(), rand() % 2000 - 1000, rand() % 2000 - 1000),
		false, false);
}