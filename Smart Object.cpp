#include "stdafx.h"
#include "Smart Object.h"
#include "The Kid.h"
#include <vector>

void add_some_blood(int this_much, double location_x, double location_y)
{
	double make_this_much = static_cast<double>(this_much);
	make_this_much *= (static_cast<double>(common_save_data->blood_intensity()) / 100);
	int no_really_make_this_much = static_cast<int>(make_this_much);
	for (int x = 0; x != no_really_make_this_much; ++x)
	{
		switch (rand() % 3)
		{
		case 0:
			Game::current_screen->add_a_new_smart_object(new Blood_1(location_x, location_y, rand() % 2000 - 1000, rand() % 2000 - 1000), false, false);
			break;
		case 1:
			Game::current_screen->add_a_new_smart_object(new Blood_2(location_x, location_y, rand() % 2000 - 1000, rand() % 2000 - 1000), false, false);
			break;
		case 2:
			Game::current_screen->add_a_new_smart_object(new Blood_3(location_x, location_y, rand() % 2000 - 1000, rand() % 2000 - 1000), false, false);
			break;
		}
	}
}

const Rect Fake_Mushroom::disappear_box = Rect(9900, 575, 1000, 500);

void Fake_Mushroom::delete_if_in_disappear_box()
{
	if (rect_rect_collision(disappear_box, Game::the_kid->display_outline()))
	{
		switch_to_animation(1);
	}
}

void Game_Over_Text::logic()
{
	timer_ += g_timer.deltaT_in_seconds() * 220;
	location_.x = Game::current_world->current_position_x() * g_screen_width + offset_x_;
	location_.y = Game::current_world->current_position_y() * g_screen_height + offset_y_;
	width_ratio_ = 1 + sin_degrees(timer_) / 10;
	height_ratio_ = 1 + sin_degrees(timer_) / 10;
}

void Congrats::logic()
{
	timer_ += g_timer.deltaT_in_seconds() * 200;
	width_ratio_ = 0.8 + sin_degrees(timer_) / 2;
	height_ratio_ = 0.8 + sin_degrees(timer_) / 2;
}

void Cool_Background_Tile::logic()
{
	location_.y -= g_timer.deltaT_in_seconds() * 50;
	if (location_y() <= top_)
		location_.y = bottom_;
}

void Spark::logic()
{
	if (create_trail_timer_.frequency_timer_logic())
		Game::current_screen->add_a_new_particle(new Particle(Pic::trail, 0.5, location_x(),
			location_y(), 0, 50, true), true, true);
	location_.x += g_timer.deltaT_in_seconds() * the_vector_.magnitude_x();
	location_.y += g_timer.deltaT_in_seconds() * the_vector_.magnitude_y();
	modify_velocity_with_gravity(&extra_velocity_, &force_from_gravity_);
	location_.y += (extra_velocity_ / 10);

	the_vector_.decrease_magnitude_with_zero_minimum(400 * g_timer.deltaT_in_seconds());
	if (time_left_.countdown_logic())
		prepare_to_delete();
}

void Firework::logic()
{
	location_.x += velocity_x_ * g_timer.deltaT_in_seconds();
	location_.y += velocity_y_ * g_timer.deltaT_in_seconds();
	if (countdown_to_explosion_.countdown_logic())
	{
		for (int counter = 0; counter != 29; counter++)
		{
			double new_angle = static_cast<double>(rand() % 360);
			double new_power = static_cast<double>(rand() % 200 + 200);
			Vector new_vector(new_power, new_angle);
			Game::current_screen->add_a_new_smart_object(new Spark(mid_point_x(), mid_point_y(),
				new_vector), true, true);
		}
		Mix_PlayChannel(-1, Sound::boom, 0);
		Game::camera->bump(3, 500);
		prepare_to_delete();
	}
}

void Alex::logic()
{
	if (going_left_)
	{
		location_.x -= 40 * g_timer.deltaT_in_seconds();
		angle_ -= 60 * g_timer.deltaT_in_seconds();
		if (location_.x <= left_edge)
			going_left_ = false;
	}
	else
	{
		location_.x += 40 * g_timer.deltaT_in_seconds();
		angle_ += 60 * g_timer.deltaT_in_seconds();
		if (location_.x >= right_edge)
			going_left_ = true;
	}
}

void Portal::logic()
{
	angle_ += g_timer.deltaT_in_seconds();
}

void Star::logic()
{
	if (rect_rect_collision(display_outline(), Game::the_kid->display_outline()))
	{
		prepare_to_delete();
		save_file->get_star_and_save();
		set_next_track(Game::background_music->current_track());
		Game::background_music->play_this_track_now(Music::star_get);
	}
}

void Teleporter::logic()
{
	if (!got_)
	{
		timer_ += g_timer.deltaT_in_seconds() * 80;
		angle_ = 0 + sin_degrees(timer_) * 20;
		if (rect_rect_collision(display_outline(), Game::the_kid->display_outline()))
		{
			save_file->get_teleporter_and_save();
			switch_to_animation(1);
			got_ = true;
			location_.y += 14;
			angle_ = 0;
			Mix_PlayChannel(-1, Sound::oh_boy, 0);
		}
	}
}

void Blood::logic()
{
	if (velocity_x_ == 0 && velocity_y_ == 0)
		return;
	double new_position_x = location_.x += (velocity_x_ * g_timer.deltaT_in_seconds());
	modify_velocity_with_gravity(&velocity_y_, &force_from_gravity_);
	double new_position_y = location_.y += velocity_y_;
	Point send_this;
	send_this.x = new_position_x;
	send_this.y = new_position_y;
	Game::current_world->the_tile_map()->test_for_sticky_collision(&display_outline(), send_this, &velocity_x_, &velocity_y_);
	for (auto iter = Game::current_screen->extra_walls.begin(); iter != Game::current_screen->extra_walls.end(); iter++) {
		if (rect_rect_collision(display_outline(), (*iter)->display_outline()) && (*iter)->sticky())
		{
			velocity_x_ = 0;
			velocity_y_ = 0;
		}
	}
	location_.x = new_position_x;
	location_.y = new_position_y;
}

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
				//I need to change these values
				Game::current_world->the_tile_map()->destroy_a_tile(37, 31);
				Game::current_world->the_tile_map()->destroy_a_tile(37, 32);
				Game::current_world->the_tile_map()->destroy_a_tile(37, 33);
				Game::current_world->the_tile_map()->destroy_a_tile(37, 34);
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
		if (rect_rect_collision(hit_box(), Game::the_kid->kid_hit_box()))
		{
			//Mix_PlayChannel(-1, Sound::disappearing_block, 10);
			the_state_ = Destroying;
			switch_to_animation(1);
			Game::current_screen->add_a_new_platform(new Leaping_Platform(760, 680, 900, 10800, 4200, 8100), false, true);
			Game::current_screen->add_a_new_platform(new Leaping_Platform(584, 680, 900, 10800, 4200, 5400), false, true);
			Game::current_screen->add_a_new_platform(new Leaping_Platform(400, 680, 900, 10800, 4200, 2700), false, true);
			Game::current_screen->add_a_new_platform(new Leaping_Platform(208, 680, 900, 10800, 4200, 0), false, true);
		}
		break;
	case Destroying:
		timer_ += g_timer.deltaT();
		//This is good.
		if (did_it_pass(timer_ - g_timer.deltaT(), timer_, tile_counter_ * 30))
		{
			//I need to change these values
			Mix_PlayChannel(-1, Sound::block_break, 0);
			Game::current_world->the_tile_map()->destroy_a_tile(56 - tile_counter_, 32);
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
			Game::current_screen->add_a_new_smart_hazard(new Thunderbolt(location_.x, location_.y + 23), false, true);
			switch_to_animation(2);
		}
		break;
	case Active:
		if (timer_ >= ACTIVE_TIME)
			prepare_to_delete();
		break;
	}
}
const double Big_Fireball::SCREEN_TIME = 1700;

void Big_Fireball::logic()
{
	//Warning:: This will cause problems if I decide to add more enemies to the Barry fight,
	//which I'm not planning on doing now.
	//Also, if I change the entry_point of Barry the values below will also need to change.
	center_over_this_point(*follow_this_);
	timer_ += g_timer.deltaT();
	if (timer_ >= SCREEN_TIME)
		prepare_to_delete();
}

void Link::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= dig_frequency_)
	{
		timer_ -= dig_frequency_;
		//Here I came up with a weird formula so Link throws rocks at the kid.
		double kid_position = Game::the_kid->location_x() - 998;
		double new_velocity = static_cast<double>((kid_position * 0.77));
		if (new_velocity <= -525)
			new_velocity = -525;
		Game::current_screen->add_a_new_smart_hazard(new Thrown_Rock(mid_point_x() - 10, mid_point_y(), new_velocity), false, true);
	}
}

const double Explosion::EXPLOSION_TIME = 200;

Explosion::Explosion(double x, double y) :
	Smart_Object(x, y),
	timer_(0)
{
	Mix_PlayChannel(-1, Sound::bomb, 0);
	int decision = rand() % 5;
	switch (decision)
	{
	case 0:
		add_new_texture(Pic::explosion1);
		break;
	case 1:
		add_new_texture(Pic::explosion2);
		break;
	case 2:
		add_new_texture(Pic::explosion3);
		break;
	case 3:
		add_new_texture(Pic::explosion4);
		break;
	case 4:
		add_new_texture(Pic::explosion5);
		break;
	default:
		cout << "Something went wrong in Explosion::Explosion(double x, double y) Fix now!" << endl;
		break;
	}
}

void Explosion::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= EXPLOSION_TIME)
	{
		prepare_to_delete();
	}
}

Texture* get_random_explosion()
{
	int decision = rand() % 5;
	switch (decision)
	{
	case 0:
		return Pic::explosion1;
	case 1:
		return Pic::explosion2;
	case 2:
		return Pic::explosion3;
	case 3:
		return Pic::explosion4;
	case 4:
		return Pic::explosion5;
	default:
		cout << "Something went wrong in get_random_explosion() Fix now!" << endl;
		return Pic::explosion5;
	}
}



void randomly_place_explosion_in_a_box(Rect box)
{
	double entry_x = rand() % static_cast<int>(box.w) + static_cast<int>(box.x);
	double entry_y = rand() % static_cast<int>(box.h) + static_cast<int>(box.y);
	Game::current_screen->add_a_new_smart_object(new Explosion(entry_x, entry_y), false, true);
}

Mini_Axe::Mini_Axe(double x, double y, bool left) :
	Smart_Object(x, y),
	left_(left)
{
	hit_box_ = Rect(x, y, 32, 32);
	add_new_texture(Pic::mini_axe);
}

void Mini_Axe::logic()
{
	for (auto iter = Game::the_bullets.begin(); iter != Game::the_bullets.end(); ++iter)
	{
		if (rect_rect_collision(hit_box_, (*iter)->the_box()))
		{
			Game::current_screen->add_a_new_event(new Destroy_Bridge_Event(left_));
			(*iter)->prepare_to_delete();
			Game::current_screen->grab_a_smart_hazard(0)->prepare_to_delete();
			Game::current_screen->grab_a_smart_hazard(1)->prepare_to_delete();
		}
	}
}

Metroid_Door::Metroid_Door(double x, double y, SDL_RendererFlip direction) :
	Smart_Object(x, y),
	HP_(5),
	flash_time_left(0),
	door_opening_(false),
	top_tile_x_(static_cast<int>(x / G_TILE_WIDTH)),
	top_tile_y_(static_cast<int>(y / G_TILE_HEIGHT))
{
	direction_ = direction;
	add_new_texture(Pic::metroid_door_idle);
	add_new_texture(Pic::metroid_door_flashing);
	add_new_texture(Pic::metroid_door_opening);
	hit_box_.x = top_tile_x_ * G_TILE_WIDTH;
	hit_box_.y = top_tile_y_ * G_TILE_HEIGHT;
	hit_box_.w = G_TILE_WIDTH;
	hit_box_.h = DOOR_HEIGHT;
}

void Metroid_Door::logic()
{
	if (door_opening_)
	{
		flash_time_left -= g_timer.deltaT();
		if (flash_time_left <= 0)
			prepare_to_delete();
		return;
	}
	else if (flash_time_left > 0)
	{
		flash_time_left -= g_timer.deltaT();
		if (flash_time_left <= 0)
		{
			flash_time_left = 0;
			switch_to_animation(0);
			return;
		}
	}
	else
	{
		if (box_collides_with_bullet(&hit_box_))
		{
			HP_--;
			if (HP_ == 0)
			{
				//Now I'll use this for the "door opening" animation. The value I set this\
				to now is the amount of time from NOW, until the door is removed
				flash_time_left = 200;
				door_opening_ = true;
				switch_to_animation(2);
				//4 because it takes up 4 tiles
				for (int counter = 0; counter != 4; counter++)
					Game::current_world->the_tile_map()->destroy_a_tile(top_tile_x_, top_tile_y_ + counter);
				return;
			}
			switch_to_animation(1);
			flash_time_left += FLASH_TIME;
		}
	}
}
