#include "stdafx.h"
#include "Hazards.h"
#include "globals.h"
#include "Worlds and Maps.h"
#include "The Kid.h"
#include <map>

const double Moon::RADIUS = 143;
//const double Spinning_Saw::RADIUS = 40;
//const double Spinning_Saw::SPEED = 1.8;
//const double Chain::SPEED = 1.8;
//const double Spinning_Saw::DISTANCE = 300;
const double Troll_Spike::SIZE = 32;
const double Troll_Spike::FALLING_VELOCITY = 300;
const double Troll_Spike::SHAKING_FPS = 10;
//const double Pendulum::RADIUS = 40;
//const double Pendulum::SPEED = 1.8;
//const double Pendulum::DISTANCE = 300;
//const double Pendulum_Chain::SPEED = 1.8;
const double Magic_Projectile::SPEED = 360;
const double Magic_Projectile::HEIGHT = 16;
const double Magic_Projectile::WIDTH = 8;
const double Magic_Projectile::MAX_TIME_ON_SCREEN = 2500;

namespace Game
{
	vector<Smart_Hazard*> enemy_hazards;
}

bool Hazard::test_collision_with_the_kid()
{
	if (direction_ == SDL_FLIP_NONE)
	{
		if (collision_data_->test_for_collision(Game::the_kid->kid_hit_box(), location_.x, location_.y, angle_, width_ratio_, height_ratio_))
			return true;
	}
	else if (collision_data_->test_for_mirrored_collision(Game::the_kid->kid_hit_box(), location_.x, location_.y, angle_, width_ratio_, height_ratio_))
		return true;
	return false;
}

//This is the radius of the pic I'm using for the saw. 
const double BASE_SAW_RADIUS = 40;
const double BASE_CHAIN_RADIUS = 10;

Chain::Chain(double x, double y, double distance, double speed, double radius) :
	Smart_Object(x, y),
	starting_location_(x, y),
	timer_(0),
	distance_(distance),
	speed_(speed),
	radius_(radius)
{
	add_new_texture(Pic::chain);
	width_ratio_ = radius / BASE_CHAIN_RADIUS;
	height_ratio_ = radius / BASE_CHAIN_RADIUS;
}

Spinning_Saw::Spinning_Saw(double x, double y, double distance, double speed, double radius) :
	Smart_Hazard(x, y, Collision::spinning_saw),
	starting_location_(Point(x, y)),
	timer_(0),
	distance_(distance),
	speed_(speed),
	radius_(radius)
{
	add_new_texture(Pic::buzzsaw);
	width_ratio_ = radius / BASE_SAW_RADIUS;
	height_ratio_ = radius / BASE_SAW_RADIUS;
}

Pendulum::Pendulum(double x, double y, double distance, double speed, double radius) :
	Smart_Hazard(x, y, Collision::spinning_saw),
	starting_location_(Point(x, y)),
	timer_(0),
	distance_(distance),
	speed_(speed),
	radius_(radius)
{
	add_new_texture(Pic::buzzsaw);
	width_ratio_ = radius / BASE_SAW_RADIUS;
	height_ratio_ = radius / BASE_SAW_RADIUS;
}

Pendulum_Chain::Pendulum_Chain(double x, double y, double distance, double speed, double radius) :
	Smart_Object(x, y),
	starting_location_(x, y),
	timer_(0),
	distance_(distance),
	speed_(speed),
	radius_(radius)
{
	add_new_texture(Pic::chain);
	width_ratio_ = radius / BASE_CHAIN_RADIUS;
	height_ratio_ = radius / BASE_CHAIN_RADIUS;
}

void Moon::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (rect_rect_collision(Game::the_kid->kid_hit_box(), activation_box))
			the_state_ = Activated;
		break;
	case Activated:
	{
		Game::current_world->the_tile_map()->dislodge_tiles_in_box(&display_outline());
		double distance_this_frame = speed[current_instruction_] * g_timer.deltaT_in_seconds();
		distance_through_instruction_ += distance_this_frame;
		if (distance_through_instruction_ >= distance[current_instruction_])
		{
			location_.x = location_at_start_of_instruction_x_ + distance[current_instruction_] * cos_degrees(direction_angle[current_instruction_]);
			location_.y = location_at_start_of_instruction_y_ + distance[current_instruction_] * sin_degrees(direction_angle[current_instruction_]);
			current_instruction_++;
			distance_through_instruction_ = 0;
			location_at_start_of_instruction_x_ = location_.x;
			location_at_start_of_instruction_y_ = location_.y;
			if (current_instruction_ == direction_angle.size())
			{
				the_state_ = Done;
				return;
			}
		}
		else
		{
			if (direction_angle[current_instruction_] < 180)
				angle_ += speed[current_instruction_] * g_timer.deltaT_in_seconds() * 4; //Some const
			else
				angle_ -= speed[current_instruction_] * g_timer.deltaT_in_seconds() * 4; //Some const

			location_.x += distance_this_frame * cos_degrees(direction_angle[current_instruction_]);
			location_.y += distance_this_frame * sin_degrees(direction_angle[current_instruction_]);
		}
	}
	break;
	case Done:
		prepare_to_delete();
		break;
	}
}

Magic_Projectile::Magic_Projectile(double x, double y, double angle) :
	Smart_Hazard(x, y, Collision::magic_projectile),
	time_on_screen_(0),
	object_vector_(SPEED, angle)
{
	angle_ = angle;
	height_ratio_ = 1.5;
	width_ratio_ = 1.5;
	add_new_texture(Pic::magic_projectile);
}

void Magic_Projectile::logic()
{
	time_on_screen_ += g_timer.deltaT();
	location_.x += object_vector_.magnitude_x() * g_timer.deltaT_in_seconds();
	location_.y += object_vector_.magnitude_y() * g_timer.deltaT_in_seconds();
	//delete if time is up
	if (time_on_screen_ >= MAX_TIME_ON_SCREEN)
		prepare_to_delete();
}

void Troll_Spike::logic()
{
	switch (its_state_)
	{
	case Waiting:
		if (rect_rect_collision(Game::the_kid->kid_hit_box(), activation_box_)
			)
		{
			its_state_ = Activated;
		}
		break;
	case Activated:
		//The spike starts shaking in this state.
		shaking_time_ += g_timer.deltaT();
		mini_shaking_time_ += g_timer.deltaT();
		if (mini_shaking_time_ >= SHAKING_FPS)
		{
			mini_shaking_time_ -= SHAKING_FPS;
			switch (shaking_side_)
			{
			case 0:
				location_.x += 2;
				shaking_side_ = 1;
				break;
			case 1:
				location_.x -= 4;
				shaking_side_ = 2;
				break;
			case 2:
				location_.x += 4;
				shaking_side_ = 1;
				break;
			}
		}
		if (shaking_time_ >= max_shaking_time)
		{
			its_state_ = Falling;
		}
		break;
	case Falling:
	{
		location_.x += g_timer.deltaT_in_seconds() * cos_degrees(moving_angle_) * velocity_;
		location_.y += g_timer.deltaT_in_seconds() * sin_degrees(moving_angle_) * velocity_;

		if (location_.y > 9000)
			its_state_ = Gone;
		break;
	}
	case Gone:
		break;
	}
}

const double Hadouken::SPEED = 500;
const double Hadouken::HEIGHT = 36;
const double Hadouken::WIDTH = 24;

Hadouken::Hadouken(double x, double y, double angle) :
	Smart_Hazard(x, y, Collision::hadouken),
	timer_(0),
	base_velocity_x(SPEED* cos(angle* g_pi / 180)),
	base_velocity_y(SPEED* sin(angle* g_pi / 180))
{
	angle_ = angle;
	add_new_texture(Pic::hadouken);
}

void Hadouken::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= 5000)
		prepare_to_delete();
	//Copied from the_kid.cpp.
	double new_position_x = location_.x + base_velocity_x * g_timer.deltaT_in_seconds();
	double new_position_y = location_.y + base_velocity_y * g_timer.deltaT_in_seconds();
	for (int x = static_cast<int>(new_position_x / G_TILE_WIDTH); x <= static_cast<int>((new_position_x + width()) / G_TILE_WIDTH); ++x)
		for (int y = static_cast<int>(new_position_y / G_TILE_HEIGHT); y <= static_cast<int>((new_position_y + height()) / G_TILE_WIDTH); ++y)
		{
			if (Game::current_world->the_tile_map()->is_this_tile_sticky(x, y) && !Game::current_world->the_tile_map()->is_this_tile_passable(x, y))
			{
				Game::current_world->the_tile_map()->destroy_a_tile(x, y);
				prepare_to_delete();
			}
			if (!Game::current_world->the_tile_map()->is_this_tile_passable(x, y))
				prepare_to_delete();
		}
	location_.x = new_position_x;
	location_.y = new_position_y;
}

const double Fireball::SPEED = 0.4;
const double Fireball::HEIGHT = 13;
const double Fireball::WIDTH = 13;

Fireball::Fireball(double x, double y, double angle, bool use_gravity) :
	Smart_Hazard(x, y, Collision::fireball),
	timer_(0),
	base_velocity_x(SPEED* cos(angle* g_pi / 180)),
	base_velocity_y(SPEED* sin(angle* g_pi / 180)),
	use_gravity_(use_gravity),
	force_from_gravity_(base_velocity_y)
{
	if (use_gravity_)
	{
		base_velocity_x = static_cast<double>((rand() % 1200 - 600));
		base_velocity_y = 0;
		force_from_gravity_ = -600;
	}
	angle_ = angle;
	add_new_texture(Pic::fireball);
}

void Fireball::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= 2000)
		prepare_to_delete();
	//Copied from the_kid.cpp
	double new_position_x = 0;
	double new_position_y = 0;
	if (use_gravity_)
	{
		modify_velocity_with_gravity(&base_velocity_y, &force_from_gravity_);
		new_position_y = location_.y + base_velocity_y;
		new_position_x = location_.x + base_velocity_x * g_timer.deltaT_in_seconds();
	}
	else
	{
		new_position_x = location_.x + base_velocity_x * g_timer.deltaT();
		new_position_y = location_.y + base_velocity_y * g_timer.deltaT();
	}
	location_.x = new_position_x;
	location_.y = new_position_y;
}

const double Thunderbolt::SCREEN_TIME = 100;
void Thunderbolt::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= SCREEN_TIME)
		prepare_to_delete();
}

void Thrown_Rock::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= 2500)
		prepare_to_delete();
	if (rotating_left_)
		angle_ += 2;
	else
		angle_ -= 2;

	//Copied from Fireball::logic()
	double new_position_x = 0;
	double new_position_y = 0;
	modify_velocity_with_gravity(&base_velocity_y, &force_from_gravity_);
	new_position_y = location_.y + base_velocity_y;
	new_position_x = location_.x + base_velocity_x * g_timer.deltaT_in_seconds();
	location_.x = new_position_x;
	location_.y = new_position_y;
}

void Windmill::logic()
{
	angle_ += speed_ * g_timer.deltaT_in_seconds();
}

void Shockwave::logic()
{
	location_.x += 400 * g_timer.deltaT_in_seconds();
	time_left_ -= g_timer.deltaT();
	if (time_left_ <= 0)
		prepare_to_delete();
}

void Spike_Wall::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (rect_rect_collision(activation_box_, Game::the_kid->kid_hit_box()))
			the_state_ = Chasing;
		break;
	case Chasing:
		location_.x -= g_timer.deltaT_in_seconds() * 40;
		time_left_ -= g_timer.deltaT();
		if (time_left_ <= 0)
			the_state_ = Done;
		break;
	case Done:
		break;
	}
}

void Missile::logic()
{
	double initial_angle = angle_;
	double new_angle = calculate_angle_between_points_in_degrees(middle_of_box(), Game::the_kid->middle_of_box()) + 90;
	double angle_difference = new_angle - angle_;
	if (angle_difference > 180 || new_angle < angle_ && angle_ - new_angle < 180)
	{
		if (angle_velocity_ > 0)
			angle_velocity_ = 0;
		angle_velocity_ = -100 * g_timer.deltaT_in_seconds();
	}
	else
	{
		if (angle_velocity_ < 0)
			angle_velocity_ = 0;
		angle_velocity_ = 100 * g_timer.deltaT_in_seconds();
	}
	angle_ += angle_velocity_;
	if (did_it_pass(initial_angle, angle_, 360))
		angle_ -= 360;
	else if (did_it_pass_neg(initial_angle, angle_, 0))
		angle_ += 360;
	location_.x += cos_degrees(angle_ - 90) * missile_speed_ * g_timer.deltaT_in_seconds();
	location_.y += sin_degrees(angle_ - 90) * missile_speed_ * g_timer.deltaT_in_seconds();
}

void Falling_Spike::logic()
{
	modify_velocity_with_gravity(&yVelocity_, &force_from_gravity_);
	location_.y += yVelocity_;
	if (location_.y > g_screen_height)
		prepare_to_delete();
}

void Moving_Spike::logic()
{
	location_.y += g_timer.deltaT_in_seconds() * yVelocity_;
	time_left_ -= g_timer.deltaT();
	if (time_left_ <= 0)
		prepare_to_delete();
}

void Crushing_Spikes::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (rect_rect_collision(Game::the_kid->kid_hit_box(), activation_box_))
			the_state_ = Crushing;
		break;
	case Crushing:
		location_.y += g_timer.deltaT_in_seconds() * 20;
		crushing_time_left_ -= g_timer.deltaT();
		if (crushing_time_left_ <= 0)
			the_state_ = Done;
		break;
	case Done:
		break;
	}
}

void Chasing_Spikes::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (rect_rect_collision(Game::the_kid->kid_hit_box(), activation_box_))
		{
			for (int counter = 0; counter != 8; counter++)
				Game::current_world->the_tile_map()->destroy_a_tile(300 + counter, 24);
			the_state_ = Turning;
		}
		break;
	case Turning:
		angle_ += 100 * g_timer.deltaT_in_seconds();
		location_.x += 80 * g_timer.deltaT_in_seconds();
		location_.y -= 40 * g_timer.deltaT_in_seconds();
		if (angle_ >= 90)
		{
			angle_ = 90;
			the_state_ = Waiting2;
		}
		break;
	case Waiting2:
		timer_ += g_timer.deltaT();
		if (timer_ >= 900)
		{
			the_state_ = Chasing;
			timer_ = 0;
		}
		break;
	case Chasing:
		timer_ += g_timer.deltaT();
		location_.x += 2000 * g_timer.deltaT_in_seconds();
		if (timer_ >= 5000)
			prepare_to_delete();
		break;
	}
}

void Rising_Spikes::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (rect_rect_collision(activation_box_, Game::the_kid->kid_hit_box()))
			the_state_ = Rising;
		break;
	case Rising:
		location_.y -= g_timer.deltaT_in_seconds() * 100;
		if (location_.y <= -100)
			prepare_to_delete();
		break;
	}
}

const double Rainbow_Spike::RADIUS = 237;
void Rainbow_Spike::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (rect_rect_collision(Game::the_kid->kid_hit_box(), activation_box_))
			the_state_ = Active;
		break;
	case Active:
		angle_ += 280 * g_timer.deltaT_in_seconds();
		timer_ += g_timer.deltaT();
		location_.x = center_.x + cos_degrees(angle_ - 180) * RADIUS;
		location_.y = center_.y + sin_degrees(angle_ - 180) * RADIUS;
		if (timer_ >= 1000)
			prepare_to_delete();
		break;
	}
}

void Poking_Spike::logic()
{
	timer_ += g_timer.deltaT_in_seconds() * frequency_;
	location_.y = starting_location_y_ + sin(timer_) * distance_;
}
