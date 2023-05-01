#include "stdafx.h"
#include "Enemy_And_Smart_Object_Pairs.h"
#include "The Kid.h"
#include "Boss Health.h"

const double Super_Carock_Body::alternating_sine_wave_position_a_ = 9500;
const double Super_Carock_Body::alternating_sine_wave_position_b_ = 10000;
const Point Super_Carock_Body::right_home_point(9990, 900);
const Point Super_Carock_Body::left_home_point(9450, 900);
const double Super_Carock_Body::default_fire_frequency = 0.5;
const double Super_Carock_Body::default_new_attack_frequency = 4;
const double Super_Carock_Body::follow_velocity = 100;
const double Super_Carock_Body::charge_power = 750;

Super_Carock::Super_Carock() :
	Enemy(60)//80
{
	Super_Carock_Body* super_carock_body = new Super_Carock_Body(10112, 1060, this);
	the_parts_.push_back(super_carock_body);
	super_carock_body_ = super_carock_body;
	//Do this for all bosses!!!
	modify_health_based_on_difficulty();
}

void Super_Carock::use_mushroom()
{
	super_carock_body_->use_mushroom();
}

void Super_Carock_Body::use_mushroom()
{
	Mix_PlayChannel(-1, Sound::use_mushroom, 0);
	current_state_ = Growing;
}

void Super_Carock_Body::part_logic()
{
	if (parent_object_->HP_percentage() <= 0.5)
	{
		speed_multiplier_ = 1.2;
		Game::camera->shaking_minimum(2);
	}
	if (parent_object_->HP_percentage() <= 0.2)
	{
		speed_multiplier_ = 1.5;
		Game::camera->shaking_minimum(8);
	}
	if (parent_object_->HP_percentage() < 0.2 && explode_timer_.frequency_timer_logic())
	{
		Point new_point = get_rand_point_in_box(display_outline());
		particle_generator()->make_a_particle(new Particle(Pic::explosion1, 0.08, new_point.x, new_point.y, 0, 20, false), false);
	}
	switch (current_state_)
	{
	case Circling:
	{
		if (clock_wise_)
		{
			circle_speed_ += 400 * g_timer.deltaT_in_seconds() * speed_multiplier_;
		}
		else
		{
			circle_speed_ -= 400 * g_timer.deltaT_in_seconds() * speed_multiplier_;
		}
		double previous_y = location_.y;
		if (action_circle_around_a_point(circle_around_this_, circle_speed_, 270, 2))
		{
			ready_for_a_new_attack_ = true;
		}
		double new_y = location_.y;
		if (home_point == &right_home_point)
		{
			std::swap(new_y, previous_y);
		}
		if (ready_for_a_new_attack_ && previous_y < home_point->y && new_y >= home_point->y)
		{
			if (home_point == &right_home_point)
			{
				set_home_point_to_left();
				direction_ = SDL_FLIP_NONE;
			}
			else
			{
				set_home_point_to_right();
				direction_ = SDL_FLIP_HORIZONTAL;
			}
			new_attack_timer_.change_max_time(3);
			switch_to_normal_state();
			location_ = *home_point;
		}
		break;
	}
	case Introduction:
		return;
	case Intro_To_Main:
		if (target_a_point(home_point->x, home_point->y, 500))
			switch_to_normal_state();
		break;
	case Growing:
		height_ratio_ += 0.5 * g_timer.deltaT_in_seconds();
		width_ratio_ += 0.5 * g_timer.deltaT_in_seconds();
		if (height_ratio_ >= 1.67)
		{
			height_ratio_ = 1.67;
			width_ratio_ = 1.67;
			Game::boss_health_bar->point_to_this_enemy(parent_object_);
			Game::background_music->play_this_track_now(Music::boss_music_part_one);
			set_next_track(Music::boss_music_part_two);
			change_state(Intro_To_Main);
		}
		return;
	case Normal:
	{
		double previous_y = location_.y;
		action_verticle_sine_wave(normal_sine_wave_speed_ * speed_multiplier_, 150, -1);
		if (new_attack_timer_.frequency_timer_logic())
			ready_for_a_new_attack_ = true;
		double new_y = location_.y;
		if (home_point == &left_home_point)
			swap(new_y, previous_y);
		if (ready_for_a_new_attack_ && previous_y < home_point->y && new_y >= home_point->y)
		{
			pick_attack();
		}
		break;
	}
	case Follow:
		if (Game::the_kid->location_y() > location_.y)
		{
			location_.y += g_timer.deltaT_in_seconds() * follow_velocity * speed_multiplier_;
		}
		else if (Game::the_kid->location_y() < location_.y)
		{
			location_.y -= g_timer.deltaT_in_seconds() * follow_velocity * speed_multiplier_;
		}
		if (new_attack_timer_.frequency_timer_logic())
		{
			change_state(Follow_To_Normal_Transition);
			attack_timer_->change_max_time(default_fire_frequency);
		}
		break;
	case Follow_To_Normal_Transition:
	{
		double previous_y = location_.y;
		if (home_point->y > location_.y)
		{
			location_.y += g_timer.deltaT_in_seconds() * follow_velocity * 2 * speed_multiplier_;
			if (previous_y < home_point->y && location_.y >= home_point->y)
			{
				switch_to_normal_state();
				normal_sine_wave_speed_ = 300;
			}
		}
		else if (home_point->y < location_.y)
		{
			location_.y -= g_timer.deltaT_in_seconds() * follow_velocity * 2 * speed_multiplier_;
			if (previous_y > home_point->y && location_.y <= home_point->y)
			{
				switch_to_normal_state();
				normal_sine_wave_speed_ = -300;
			}
		}
		break;
	}
	case Charge:
		action_rotate(1000, -1);
		if (action_accel(charge_vector_.magnitude_x() * speed_multiplier_, charge_vector_.magnitude_y() * speed_multiplier_, 2))
		{
			//Make this random
			double new_x = 0;
			double new_y = 0;
			if (rand() % 2)
			{
				new_x = rand() % g_screen_width + 9216;
				if (rand() % 2)
					new_y = g_screen_height;
				else
					new_y = g_screen_height * 2;
			}
			else
			{
				new_y = rand() % g_screen_height + 576;
				if (rand() % 2)
					new_x = 9216;
				else
					new_x = 9216 + g_screen_width;
			}
			location_ = Point(new_x, new_y);
			charge_target_ = Game::the_kid->location();
			charge_vector_.set_angle(calculate_angle_between_points_in_degrees(location_, Game::the_kid->location()));
			change_state(Charge_2);
			Mix_PlayChannel(-1, Sound::you_will_die, 0);
		}
		break;
	case Charge_2:
	{
		action_rotate(1000, -1);
		if (action_accel(charge_vector_.magnitude_x() * 2 * speed_multiplier_, charge_vector_.magnitude_y() * 2 * speed_multiplier_, 2))
		{
			if (home_point == &left_home_point)
			{
				set_home_point_to_right();
				location_.x += 1000;
			}
			else
			{
				set_home_point_to_left();
				location_.x -= 1000;
			}
			change_state(Return_From_Charge);
			reset_actions();
		}
		break;
	case Return_From_Charge:
		if (home_point == &left_home_point)
		{
			double previous_location = location_.x;
			action_accel(1500 * speed_multiplier_, 0, -1);
			if (previous_location < home_point->x && location_.x >= home_point->x)
			{
				location_ = *home_point;
				switch_to_normal_state();
			}
		}
		else
		{
			double previous_location = location_.x;
			action_accel(-1500 * speed_multiplier_, 0, -1);
			if (previous_location > home_point->x && location_.x <= home_point->x)
			{
				location_ = *home_point;
				switch_to_normal_state();
			}
		}
		break;
	}
	case Explode:
		if (attack_timer_->recent_cycle())
			for (int counter = 0; counter != 10; counter++)
			{
				Game::camera->bump(40 * speed_multiplier_, static_cast<int>(500 * speed_multiplier_));
				double new_angle = static_cast<double>(counter) * 36; //this and # of new hazards should multiply to 360
				Game::current_screen->add_a_new_smart_hazard(new Magic_Projectile(location_.x, location_.y, new_angle), false, true);
			}
		if (new_attack_timer_.frequency_timer_logic())
			switch_to_normal_state();
		break;
	}
	if (sparkle_timer_->frequency_timer_logic())
	{
		Point new_point = get_rand_point_in_box(display_outline());
		particle_generator()->make_a_particle(new Particle(Pic::sparkle, 0.08, new_point.x, new_point.y, 0, 20, false), false);
	}
	if (parent_object_->HP_percentage() < 0.5 && smoke_timer_.frequency_timer_logic())
	{
		Point new_point = get_rand_point_in_box(display_outline());
		particle_generator()->make_a_particle(new Particle(Pic::smoke, 0.08, new_point.x, new_point.y, 0, 20, false), false);
	}
	if (attack_timer_->frequency_timer_logic())
	{
		double new_angle = angle_;
		if (direction_ == SDL_FLIP_HORIZONTAL)
			new_angle += 180;
		Mix_PlayChannel(-1, Sound::pew, 0);
		Game::current_screen->add_a_new_smart_hazard(new Magic_Projectile(location_.x, location_.y, new_angle), false, true);

		// ^ ^ Ugh. Don't do that.
		switch_to_animation(1);
	}
	//Handle animation
	if (attack_timer_->time_spent() > 0.1)
		switch_to_animation(0);
}
void Super_Carock_Body::set_move_queue()
{
	for (int counter = 0; counter != number_of_moves; counter++)
		move_queue[counter] = counter;
	for (int counter = 0; counter != 50; counter++)
	{
		int decision = rand() % (number_of_moves - 1);
		std::swap(move_queue[decision], move_queue[decision + 1]);
	}
	position_in_move_queue = 0;
}


void Super_Carock_Body::pick_attack()
{
	if (position_in_move_queue == number_of_moves)
		set_move_queue();
	switch (move_queue[position_in_move_queue])
	{
	case 0:
		direction_ = SDL_FLIP_HORIZONTAL;
		change_state(Circling);
		Mix_PlayChannel(-1, Sound::whoa, 0);
		break;
	case 1:
		change_state(Follow);
		attack_timer_->change_max_time(0.1 / speed_multiplier_); //Lots of bullets
		new_attack_timer_.change_max_time(2 / speed_multiplier_);
		break;
	case 2:
		change_state(Charge);
		charge_vector_.set_angle(calculate_angle_between_points_in_degrees(location_, Game::the_kid->location()));
		charge_target_ = Game::the_kid->location();
		Mix_PlayChannel(-1, Sound::you_must_die, 0);
		break;
	case 3:
		change_state(Explode);
		Mix_PlayChannel(-1, Sound::patton, 0);
		break;
	default:
		std::cout << "Something went wrong" << std::endl;
	}
	position_in_move_queue++;
}

void Super_Carock::kill_this_enemy()
{
	Game::boss_health_bar->remove_enemy();
	dying_ = true;
	the_parts_[0]->make_harmless();
	Game::background_music->play_this_track_now(Music::victory);
	set_next_track(Music::tutorial_bgm);
	Mix_PlayChannel(-1, Sound::dag, 0);
}


void Super_Carock_Body::die()
{
	action_rotate(1000, -1);
	action_move(30, 30, 1000);
	if (death_blood_timer_.frequency_timer_logic())
		add_some_blood(2, mid_point_x(), mid_point_y());
	if (death_exploding_timer_.frequency_timer_logic())
	{
		particle_generator()->make_a_particle(new Particle(
			get_random_explosion(),
			0.5,
			get_rand_point_in_box(display_outline()).x,
			get_rand_point_in_box(display_outline()).y,
			0,
			0,
			true),
			false
		);
		Mix_PlayChannel(-1, Sound::sonic_boom, 0);
	}
	if (die_time_.countdown_logic())
	{
		add_some_blood(100, mid_point_x(), mid_point_y());
		for (int x = 0; x != 5; x++)
			randomly_place_explosion_in_a_box(display_outline());
		parent_object_->prepare_to_delete();
	}
}
