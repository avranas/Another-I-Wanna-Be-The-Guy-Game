#include "stdafx.h"
#include "Special Platform.h"
#include "Worlds and Maps.h"
#include "Music and Sound.h"
#include "The Kid.h"

void Special_Platform::move_platform_and_stick_to_kid()
{
	location_.x += velocity_x_;
	set_previous_y(location_.y);
	location_.y += velocity_y_;
	if (check_collision() && y_velocity() > 0)
		Game::the_kid->set_additional_velocity_y(y_velocity());
}

void Moving_Platform::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= max_time_[current_instruction_])
	{
		timer_ -= max_time_[current_instruction_];
		current_instruction_++;
		if (current_instruction_ == max_time_.size())
			current_instruction_ = 0;
	}
	set_x_velocity(x_velocity_list_[current_instruction_] * g_timer.deltaT_in_seconds());
	set_y_velocity(y_velocity_list_[current_instruction_] * g_timer.deltaT_in_seconds());
	move_platform_and_stick_to_kid();
}

const double Falling_Platform::HOVERING_TIME = 400;
const double Falling_Platform::TIME_UNTIL_FALLEN = 600;

void Falling_Platform::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (recent_collision_found_)
			the_state_ = Activated;
		break;
	case Activated:
		timer_ += g_timer.deltaT();
		if (timer_ >= HOVERING_TIME)
		{
			if (rand() % 2 == 1)
			{
				switch_to_animation(3);
				fell_left_ = true;
			}
			else
			{
				switch_to_animation(1);
				fell_left_ = false;
			}
			timer_ -= HOVERING_TIME;
			the_state_ = Falling;
		}
		move_platform_and_stick_to_kid();
		break;
	case Falling:
		timer_ += g_timer.deltaT();
		//This will prevent sticking to the falling platform.
		recent_collision_found_ = false;
		//Instead of caling move_platform_and_stick_to_kid(), I do this to prevent the_kid from sticking to the platform.
		location_.y += y_velocity();
		modify_velocity_with_gravity(&velocity_y_, &force_from_gravity_);
		if (animation_over_)
		{
			if (fell_left_)
				switch_to_animation(4);
			else
				switch_to_animation(2);
		}
		if (timer_ >= 9000)
			the_state_ = Fallen;
		break;
	case Fallen:
		break;
	}
}

void Yellow_Switch::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (recent_collision_found_)
		{
			recent_collision_found_ = false;
			the_state_ = Pressed;
			switch_to_animation(1);
			previous_y_ += 16;
			location_.y += 16;
			Game::current_screen->add_a_new_event(new Yellow_Switch_Event);
		}
		break;
	case Pressed:
		break;
	}
}

void Green_Switch::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (recent_collision_found_)
		{
			recent_collision_found_ = false;
			the_state_ = Pressed;
			switch_to_animation(1);
			previous_y_ += 16;
			location_.y += 16;
			for (int counter = 0; counter != 7; ++counter)
				Game::current_world->the_tile_map()->create_a_tile(50, 12 + counter, 'g');
			Mix_PlayChannel(-1, Sound::switch_flip, 0);
		}
		break;
	case Pressed:
		break;
	}
}

void Leaping_Platform::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= jump_frequency_)
	{
		Mix_PlayChannel(-1, Sound::fireball, 0);
		timer_ -= jump_frequency_;
		force_from_gravity_ = -jump_power_;
	}
	if (location_.y < starting_location_ - jump_height_)
	{
		modify_velocity_without_gravity(&velocity_y_, &force_from_gravity_);
	}
	else
	{
		modify_velocity_with_gravity(&velocity_y_, &force_from_gravity_);
	}
	move_platform_and_stick_to_kid();
	if (location_.y > starting_location_)
	{
		velocity_y_ = 0;
		force_from_gravity_ = 0;
		location_.y = starting_location_;
	}
}

const double Spring::ACTIVE_TIME = 500;

void Spring::logic()
{
	timer_ += g_timer.deltaT_in_seconds();
	location_.x = starting_x + sin(timer_ * speed_) * distance_;
	switch (spring_state)
	{
	case Idle:
		if (check_collision())
		{
			location_.y -= 16;
			Game::the_kid->push_up(jump_power_);
			if (!explodes_)
			{
				Mix_PlayChannel(-1, Sound::spring, 0);
				switch_to_animation(1);
				spring_state = Active;
				active_timer_ = ACTIVE_TIME;
			}
			else
			{
				Game::current_screen->add_a_new_smart_object(new Explosion(location_.x, location_.y), false, true);
				prepare_to_delete();
			}
			test_for_collision_ = false;
		}
		break;
	case Active:
		active_timer_ -= g_timer.deltaT();
		if (active_timer_ <= 0)
		{
			active_timer_ = 0;
			spring_state = Idle;
			switch_to_animation(0);
			location_.y += 16;
			test_for_collision_ = true;
		}
		break;
	}
}
const double Super_Spring::ACTIVE_TIME = 500;

void Super_Spring::logic()
{
	switch (spring_state_)
	{
	case Idle:
		if (check_collision())
		{
			if (save_file->has_star())
			{
				location_.y -= 32;
				spring_state_ = Active;
				switch_to_animation(1);
				Game::the_kid->push_up(jump_power_);
				Mix_PlayChannel(-1, Sound::spring, 0);
				active_timer_ = ACTIVE_TIME;
				test_for_collision_ = false;
			}
			else
			{
				location_.y += 12;
				spring_state_ = Fail;
				switch_to_animation(2);
				Game::the_kid->new_location_y(location_.y - Game::the_kid->height() + 1);
				previous_y_ = location_.y;
				test_for_collision_ = true;
				recent_collision_found_ = true;
			}
		}
		break;
	case Active:
		active_timer_ -= g_timer.deltaT();
		if (active_timer_ <= 0)
		{
			active_timer_ = 0;
			spring_state_ = Idle;
			switch_to_animation(0);
			location_.y += 32;
			test_for_collision_ = true;
		}
		break;
	case Fail:
		if (Game::the_kid->in_air())
		{
			spring_state_ = Idle;
			switch_to_animation(0);
			location_.y -= 12;
			previous_y_ -= 12;
			test_for_collision_ = true;
		}
		move_platform_and_stick_to_kid();
		break;
	}
}

const double Trolley::BUTTON_WIDTH = 8;
const double Trolley::BUTTON_HEIGHT = 12;
const double Trolley::TOTAL_DISTANCE = 30;
const double Trolley::MAX_MOVING_TIME = 80;

void Trolley::logic()
{
	Rect left_box;
	left_box.x = location_.x + 9;
	left_box.y = location_.y - 17;
	left_box.w = BUTTON_WIDTH;
	left_box.h = BUTTON_HEIGHT;
	Rect right_box;
	right_box.x = location_.x + 111;
	right_box.y = location_.y - 17;
	right_box.w = BUTTON_WIDTH;
	right_box.h = BUTTON_HEIGHT;
	for (auto iter = Game::the_bullets.begin(); iter != Game::the_bullets.end(); ++iter)
	{
		if (rect_rect_collision(left_box, (*iter)->the_box()))
		{
			(*iter)->prepare_to_delete();
			if ((*iter)->direction() == SDL_FLIP_NONE)
				return;
			switch_to_animation(1);
			the_state_ = Moving_Left;
			timer_ = 0;
			x_at_last_press_ = location_.x;
		}
		if (rect_rect_collision(right_box, (*iter)->the_box()))
		{
			(*iter)->prepare_to_delete();
			if ((*iter)->direction() == SDL_FLIP_HORIZONTAL)
				return;
			switch_to_animation(2);
			the_state_ = Moving_Right;
			timer_ = 0;
			x_at_last_press_ = location_.x;
		}
	}
	switch (the_state_)
	{
	case Idle:
		break;
	case Moving_Left:
		timer_ += g_timer.deltaT();
		if (timer_ >= MAX_MOVING_TIME)
		{
			timer_ = 0;
			switch_to_animation(0);
			the_state_ = Idle;
			velocity_x_ = 0;
			break;
		}
		check_collision();
		velocity_x_ = (x_at_last_press_ - (timer_ / MAX_MOVING_TIME) * TOTAL_DISTANCE) - location_.x;
		location_.x += velocity_x_;
		if (location_.x < far_left_)
		{
			velocity_x_ = 0;
			location_.x = far_left_;
			the_state_ = Idle;
			timer_ = 0;
		}
		break;
	case Moving_Right:
		timer_ += g_timer.deltaT();
		if (timer_ >= MAX_MOVING_TIME)
		{
			timer_ = 0;
			switch_to_animation(0);
			the_state_ = Idle;
			velocity_x_ = 0;
			break;
		}
		check_collision();
		velocity_x_ = (x_at_last_press_ + (timer_ / MAX_MOVING_TIME) * TOTAL_DISTANCE) - location_.x;
		location_.x += velocity_x_;
		if (location_.x > far_right_)
		{
			velocity_x_ = 0;
			the_state_ = Idle;
			timer_ = 0;
			location_.x = far_right_;
		}
		break;
	}
}

void One_Way_Platform::logic() {}
const Point Boss_Platform::mid_point = Point((g_screen_width * 9) + (g_screen_width / 2) - 35, 920);
const double Boss_Platform::distance = 110;
const double Boss_Platform::end_speed = 100;

void Boss_Platform::logic()
{
	if (the_state_ == Normal)
	{
		double previous_angle = position_angle_.angle();
		double new_angle = position_angle_.angle() + 60 * g_timer.deltaT_in_seconds();
		position_angle_.set_angle(new_angle);
		if (boss_->dead())
		{
			if (previous_angle < 270 && new_angle >= 270)
				the_state_ = Part_2;
		}
		set_position();
	}
	else if (the_state_ == Part_2)
	{
		double previous_x = location_.x;
		double previous_y = location_.y;
		set_previous_y(location_.y);
		double new_angle = calculate_angle_between_points_in_degrees(location_, end_point_);
		velocity_x_ = cos_degrees(new_angle) * end_speed * g_timer.deltaT_in_seconds();
		velocity_y_ = sin_degrees(new_angle) * end_speed * g_timer.deltaT_in_seconds();
		double new_position_x = previous_x + velocity_x_;
		double new_position_y = previous_y + velocity_y_;
		bool x_good = false;
		bool y_good = false;
		if ((new_position_x >= end_point_.x && previous_x < end_point_.x) ||
			(new_position_x <= end_point_.x && previous_x > end_point_.x))
			x_good = true;
		if ((new_position_y >= end_point_.y && previous_y < end_point_.y) ||
			(new_position_y <= end_point_.y && previous_y > end_point_.y))
			y_good = true;
		if (y_good && x_good)
		{
			location_.y = end_point_.y;
			location_.x = end_point_.x;
			velocity_x_ = 0;
			velocity_y_ = 0;
			set_previous_y(end_point_.y + 1);
			the_state_ = Done;
		}
		else
		{
			location_.x = new_position_x;
			location_.y = new_position_y;
		}
	}
	this;
	if (check_collision() && y_velocity() > 0)
	{
		Game::the_kid->set_additional_velocity_y(y_velocity());
	}
}

void Boss_Platform::set_position()
{
	double previous_x = location_.x;
	double previous_y = location_.y;
	set_previous_y(location_.y);
	location_.x = mid_point.x + cos_degrees(position_angle_.angle()) * distance;
	location_.y = mid_point.y + sin_degrees(position_angle_.angle()) * distance;
	velocity_x_ = location_.x - previous_x;
	velocity_y_ = location_.y - previous_y;
}

void Moving_Platform_2::logic()
{
	switch (the_state_)
	{
	case Waiting:
		if (check_collision())
			the_state_ = Moving;
		break;
	case Moving:
		timer_ += g_timer.deltaT();
		if (timer_ >= max_time_[current_instruction_])
		{
			timer_ -= max_time_[current_instruction_];
			current_instruction_++;
			if (current_instruction_ == max_time_.size())
			{
				the_state_ = Done;
				set_x_velocity(0);
				set_y_velocity(0);
				break;
			}
		}
		set_x_velocity(x_velocity_list_[current_instruction_] * g_timer.deltaT_in_seconds());
		set_y_velocity(y_velocity_list_[current_instruction_] * g_timer.deltaT_in_seconds());
		break;
	case Done:
		break;
	}
	move_platform_and_stick_to_kid();
}
