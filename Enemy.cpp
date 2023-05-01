#include "stdafx.h"
#include "Enemy.h"
#include "Boss Health.h"
#include "The Kid.h"

Enemy_Part::Enemy_Part(double x, double y, Enemy* parent_object) :
	Displayable_Object(x, y),
	temp_texture_(nullptr),
	parent_object_(parent_object),
	dying_(false),
	sine_wave_time_(0),
	rotating_time_(0),
	circling_time_(0),
	circling_angle_(0),
	moving_time_(0),
	acceling_time_(0),
	x_accel_force_(0),
	y_accel_force_(0),
	x_velocity_(0),
	y_velocity_(0),
	sine_start_x_(0),
	sine_start_y_(0),
	harmless_(false),
	particle_generator_(new Particle_Generator)
{}

bool Enemy_Part::kid_part_collision()
{
	//Animations are connected with collision data.
	if (direction_ == SDL_FLIP_NONE)
	{
		if (collision_data_[current_animation_]->test_for_collision(Game::the_kid->kid_hit_box(), location_.x, location_.y, angle_, width_ratio_, height_ratio_))
			return true;
	}
	else if (collision_data_[current_animation_]->test_for_mirrored_collision(Game::the_kid->kid_hit_box(), location_.x, location_.y, angle_, width_ratio_, height_ratio_))
		return true;
	return false;
}

void Enemy::take_damage()
{
	if (HP_ == -1)
		return;
	--HP_;
	if (HP_ <= 0 && !dying_)
	{
		//immortal enemies and those with 1 HP don't need alt_pics
		kill_this_enemy();
		for (auto iter = the_parts_.begin(); iter != the_parts_.end(); ++iter)
			(*iter)->kill_this_part();

		return;
	}
	if (time_left_in_alt_pic == 0)
	{
		for (auto iter = the_parts_.begin(); iter != the_parts_.end(); ++iter)
			(*iter)->flip_to_alt_pic();
		time_left_in_alt_pic = 133;
	}
}

void Enemy::modify_health_based_on_difficulty()
{
	if (save_file->difficulty() == 0)
		HP_ /= 2;
	else if (save_file->difficulty() == 2)
		HP_ = static_cast<int>(1.5 * HP_);
	MAX_HP_ = HP_;
}

bool Enemy_Part::part_bullet_collision(Bullet* bullet)
{
	bool collision_found = false;
	if (direction_ == SDL_FLIP_NONE)
	{
		if (collision_data_[current_animation_]->test_for_collision(bullet->the_box(), location_.x, location_.y, angle_, width_ratio_, height_ratio_))
		{
			//So enemies don't take damage when the kid is dead.
			if (!Game::the_kid->dead())
				collision_found = true;
			bullet->prepare_to_delete();
		}
	}
	else
	{
		if (collision_data_[current_animation_]->test_for_mirrored_collision(bullet->the_box(), location_.x, location_.y, angle_, width_ratio_, height_ratio_))
		{
			//So enemies don't take damage when the kid is dead.
			if (!Game::the_kid->dead())
				collision_found = true;
			bullet->prepare_to_delete();
		}
	}
	return collision_found;
}

const double Frog_Body::SPEED = 260;

void Frog_Body::part_logic()
{
	double xVelocity = 0;
	if (y_velocity != 0)
	{
		if (direction_ == SDL_FLIP_HORIZONTAL)
		{
			xVelocity = g_timer.deltaT_in_seconds() * SPEED;
		}
		else
		{
			xVelocity = g_timer.deltaT_in_seconds() * SPEED;
		}
	}
	if (timer_ >= current_frequency)
	{
		Mix_PlayChannel(-1, Sound::quack, 0);
		switch_to_animation(1);
		force_from_gravity = static_cast<double>(-1 * rand() % MAX_JUMP_POWER - MIN_JUMP_POWER);
		if (Game::the_kid->hit_box_left() >= location_.x)
			direction_ = SDL_FLIP_HORIZONTAL;
		else
			direction_ = SDL_FLIP_NONE;
		current_frequency = rand() % (MAX_JUMP_FREQUENCY - MIN_JUMP_FREQUENCY) + MIN_JUMP_FREQUENCY;
		timer_ -= current_frequency;
	}
	modify_velocity_with_gravity(&y_velocity, &force_from_gravity);
	//Game::current_world->the_tile_map()->test_for_collision(&display_outline(), &Point(location_.x + xVelocity, location_.y + y_velocity), &xVelocity, &y_velocity); 
	location_.y += y_velocity;
	location_.x += xVelocity;

}

void Frog::kill_this_enemy()
{
	add_some_blood(3, the_parts_[0]->mid_point_x(), the_parts_[0]->mid_point_y());
	prepare_to_delete();
}

const double Carock_Body::SCREEN_1_CASTING_FREQUENCY = 200;
const double Carock_Body::CASTING_TIME = 160;
const double Carock_Body::SCREEN_3_ALTERNATING_TIMER = 500;
const double Carock_Body::starting_postion_x_ = 930;
const Rect Carock_Body::finish_activation_box = Rect(9185, 434, 2880, 112);

void Carock_Body::part_logic()
{
	if (rect_rect_collision(Game::the_kid->kid_hit_box(), finish_activation_box) || Game::the_kid->location_x() > 10000)
		the_position_state_ = Moving_Away;
	switch (the_position_state_)
	{
	case Do_Nothing:
		if (Game::the_kid->location_x() > 7168)
			the_position_state_ = Screen_1;
		return;
	case Screen_1:
		position_timer_ += g_timer.deltaT_in_seconds() * speed;
		location_.y = starting_location_y_ + sin(position_timer_) * distance;
		if (Game::the_kid->location_x() > start_screen_ * g_screen_width + 845)
		{
			the_position_state_ = Screen_Transition_1;
			make_harmless();
		}
		break;
	case Screen_Transition_1:
		transition_distance_ += 3000 * g_timer.deltaT_in_seconds();
		location_.x += (3000 * g_timer.deltaT_in_seconds());
		if (transition_distance_ >= g_screen_width)
		{
			make_harmfull();
			location_.x = starting_postion_x_ + g_screen_width * (start_screen_ + 1);
			the_position_state_ = Screen_2;
			transition_distance_ = 0;
		}
		return;
	case Screen_2:
		location_.y = Game::the_kid->hit_box_top();
		if (Game::the_kid->location_x() > ((start_screen_ + 1) * g_screen_width) + 845)
		{
			the_position_state_ = Screen_Transition_2;
			make_harmless();
		}
		break;
	case Screen_Transition_2:
		transition_distance_ += 3000 * g_timer.deltaT_in_seconds();
		location_.x += (3000 * g_timer.deltaT_in_seconds());
		if (transition_distance_ >= g_screen_width - 120)
		{
			make_harmfull();
			location_.x = starting_postion_x_ - 200 + g_screen_width * (start_screen_ + 2);
			the_position_state_ = Waiting_In_Screen_3;
			transition_distance_ = 0;
			speed *= 2; // Moves twice as fast in the last state.
			starting_location_y_ -= 146; //and in a different position
			the_casting_state_ = Idle; //Unflap wings
		}
		return;
	case Waiting_In_Screen_3:
		if (Game::the_kid->location_x() > ((start_screen_ + 2) * g_screen_width) + 348 ||
			rect_rect_collision(Game::the_kid->kid_hit_box(), finish_activation_box))
			the_position_state_ = Screen_3;

		return; //Carock doesn't cast in this state
	case Screen_3:
		position_timer_ += g_timer.deltaT_in_seconds() * speed;
		left_or_right_timer_ += g_timer.deltaT();
		location_.y = starting_location_y_ + sin(position_timer_) * distance;
		if (left_or_right_timer_ >= SCREEN_3_ALTERNATING_TIMER)
		{
			//I did this to stop the movement after Carock got to the middle of the screen.
			left_or_right_timer_ -= SCREEN_3_ALTERNATING_TIMER;
			if (direction_ == SDL_FLIP_NONE)
			{
				location_.x += move_this_much;
				direction_ = SDL_FLIP_HORIZONTAL;
			}
			else
			{
				location_.x -= move_this_much;
				direction_ = SDL_FLIP_NONE;
			}
		}
		break;
	case Moving_Away:
		location_.y += 1500 * g_timer.deltaT_in_seconds();
		angle_ += 1000 * g_timer.deltaT_in_seconds();
		if (location_.y > g_screen_height)
			parent_object_->prepare_to_delete();
		return;
	}
	//Do position calculation here.
	state_timer_ += g_timer.deltaT();
	switch (the_casting_state_)
	{
	case Idle:
		if (state_timer_ >= SCREEN_1_CASTING_FREQUENCY)
		{
			state_timer_ -= SCREEN_1_CASTING_FREQUENCY;
			the_casting_state_ = Casting;
			switch_to_animation(1);
			//Create magic projectile here.
			double new_angle = angle_;
			if (direction_ == SDL_FLIP_HORIZONTAL)
				new_angle += 180;
			Game::current_world->add_a_new_smart_hazard(new Magic_Projectile(location_.x, location_.y, new_angle), false, true);
			Mix_PlayChannel(-1, Sound::pew, 0);
		}
		break;
	case Casting:
		if (state_timer_ >= CASTING_TIME)
		{
			state_timer_ -= SCREEN_1_CASTING_FREQUENCY;
			the_casting_state_ = Idle;
			switch_to_animation(0);
		}
		break;
	}
}
void Carock::kill_this_enemy()
{
	//Carock can't be killed but this function is pure virtual so that's why this is here.
}

Point const Barry_Body::BARRY_HAZARD_ENTRY = Point(42, 240);
const double Barry_Body::HADOUKEN_FREQUENCY = 42;
const double Barry_Body::FIRESPIN_FREQUENCY = 50;
const double Barry_Body::FIREBALL_RAIN_FREQUENCY = 100;
const double Barry_Body::WAIT_TIME = 800;
const double Barry_Body::FIRESPIN_WARNING_TIME = 750;

void Barry_Body::pick_random_attack()
{
	action_timer_ = 0;
	turns_until_next_hadouken--;
	if (turns_until_next_hadouken == 0)
	{
		turns_until_next_hadouken = rand() % 4 + 2;
		barry_action = Use_Hadouken;
		previous_action = barry_action;
		return;
	}

	switch (rand() % 3)
	{
	case 0:
		if (previous_action == Use_Fireball_Rain)
		{
			barry_action = Firespin_Warning;
			Mix_PlayChannel(-1, Sound::fireball, 0);
			Game::current_screen->add_a_new_smart_object(new Big_Fireball(&entry_point), false, true);
			fireball_angle = rand() % 360;
		}
		else
			barry_action = Use_Fireball_Rain;
		break;
	case 1:
		if (previous_action == Use_Firespin)
			barry_action = Use_Thundercloud;
		else
		{
			barry_action = Firespin_Warning;
			Mix_PlayChannel(-1, Sound::fireball, 0);
			Game::current_screen->add_a_new_smart_object(new Big_Fireball(&entry_point), false, true);
			fireball_angle = rand() % 360;
		}
		break;
	case 2:
		if (previous_action == Use_Thundercloud)
			barry_action = Use_Fireball_Rain;
		else
			barry_action = Use_Thundercloud;
		break;
	default:
		cout << "Something went wrong in Barry::pick_random_attack();" << endl;
	}
	previous_action = barry_action;
}

void Barry_Body::part_logic()
{
	timer_ += g_timer.deltaT();
	//Face the kid
	if (Game::the_kid->hit_box_center_x() > mid_point_x())
		direction_ = SDL_FLIP_HORIZONTAL;
	else
		direction_ = SDL_FLIP_NONE;
	angle_ = sin(timer_ / 1000 - 0.5) * 10;
	if (direction_ == SDL_FLIP_HORIZONTAL)
		angle_ *= -1;
	action_timer_ += g_timer.deltaT();
	entry_point = BARRY_HAZARD_ENTRY;
	if (direction_ == SDL_FLIP_HORIZONTAL)
		entry_point.x = width() - entry_point.x;
	entry_point = turn_point_in_a_box(entry_point, width() / 2, height() / 2, angle_);
	entry_point.x += location_.x;
	entry_point.y += location_.y;
	if (dying_)
	{
		die_timer_ += g_timer.deltaT();
		blood_frequency_timer_ += g_timer.deltaT();
		if (did_it_pass(blood_frequency_timer_ - g_timer.deltaT(), blood_frequency_timer_, 20))
		{
			randomly_place_explosion_in_a_box(display_outline());
			blood_frequency_timer_ -= 20;
			add_some_blood(2, mid_point_x(), mid_point_y());
		}
		location_.y += 50 * g_timer.deltaT_in_seconds();
		if (die_timer_ >= 5000)
			parent_object_->prepare_to_delete();
		return;
	}
	switch (barry_action)
	{
	case Intro_Walk:
		location_.x -= 100 * g_timer.deltaT_in_seconds();
		if (location_.x <= 392)
			pick_random_attack();
		break;
	case Use_Hadouken:
		if (did_it_pass(action_timer_ - g_timer.deltaT(), action_timer_, HADOUKEN_FREQUENCY))
		{
			Mix_PlayChannel(-1, Sound::meow, 0);
			action_timer_ -= HADOUKEN_FREQUENCY;
			switch_to_animation(1);
			double entry_angle = 0;
			entry_angle = calculate_angle_between_points_in_degrees(entry_point, Game::the_kid->location());
			Game::current_screen->add_a_new_smart_hazard(new Hadouken(entry_point.x, entry_point.y, entry_angle), false, true);
			barry_action = Wait;
		}
		break;
	case Firespin_Warning:
		switch_to_animation(1);
		if (action_timer_ > FIRESPIN_WARNING_TIME)
		{
			previous_action = Use_Firespin;
			barry_action = Use_Firespin;
			action_timer_ = 0;
		}
		break;
	case Use_Firespin:
		if (did_it_pass(action_timer_ - g_timer.deltaT(), action_timer_, FIRESPIN_FREQUENCY))
		{
			action_timer_ -= FIRESPIN_FREQUENCY;
			switch_to_animation(1);
			for (int x = 0; x != 9; x++)
				Game::current_screen->add_a_new_smart_hazard(new Fireball(entry_point.x, entry_point.y, fireball_angle + (40 * x), false), false, true);
			fireball_count++;
			//fireball_angle += 31;
			//Game::current_screen->the_smart_hazards.push_back(new Fireball(entry_point.x, entry_point.y, fireball_angle, false));
		}
		if (fireball_count == FIRESPIN_COUNT)
		{
			finish_attack();
			fireball_count = 0;
			fireball_angle = 0;
		}
		break;
	case Use_Fireball_Rain:
		if (did_it_pass(action_timer_ - g_timer.deltaT(), action_timer_, FIREBALL_RAIN_FREQUENCY))
		{
			action_timer_ -= FIREBALL_RAIN_FREQUENCY;
			switch_to_animation(1);
			//100 degrees per second
			double entry_angle = static_cast<double>(-(rand() % 180 + 30));

			Game::current_screen->add_a_new_smart_hazard(new Thrown_Rock(entry_point.x, entry_point.y, static_cast<double>((rand() % 1000 - 500))), false, true);
			fireball_count++;
		}
		if (fireball_count == FIRERAIN_COUNT)
		{
			finish_attack();
			fireball_count = 0;
		}
		break;
	case Use_Thundercloud:
		if (action_timer_ >= 1500)
		{
			for (int x = 7; x != 0; x--)
			{
				if (Game::current_screen->number_of_smart_objects() >= 23)
					break;
				double wait_time = rand() % (7000 - x * 500) + (6500 - x * 500);
				Game::current_screen->add_a_new_smart_object(new Thundercloud(rand() % 944 + 16, 60, wait_time), false, false);
			}
			finish_attack();
		}
		break;
	case Wait:
		if (action_timer_ >= WAIT_TIME)
			pick_random_attack();
		break;
	}
	//Barry has to close his mouth.
	if (did_it_pass(action_timer_ - g_timer.deltaT(), action_timer_, 200))
		switch_to_animation(0);
}

void Barry::kill_this_enemy()
{
	Game::background_music->pause_music();
	//add_some_blood(100, mid_point_x(), mid_point_y());
	Mix_PlayChannel(-1, Sound::victory, 0);
	Game::boss_health_bar->remove_enemy();
	Game::current_screen->remove_all_events();
	//Game::current_screen->the_events.push_back(new Big_Explosion_Event(display_outline(), 200, 60));
	Game::current_screen->add_a_new_event(new Barry_Dead_Event);
	Game::current_screen->remove_all_smart_objects();
	Game::current_screen->remove_all_smart_hazards();
	Game::current_screen->remove_all_dumb_hazards();
	for (int x = 7; x != 57; x++)
	{
		//I need to change these values
		if (Game::current_world->the_tile_map()->is_this_tile_passable(x, 32))
			break;
		if (x == 56)
			for (int x = 7; x != 57; x++)
				Game::current_world->the_tile_map()->destroy_a_tile(x, 32);
	}
	the_parts_[0]->switch_to_animation(2);
}

void Pounder_Top_Fist::hide_arm()
{
	width_ratio_ = 0.5;
	height_ratio_ = 0.5;
	location_.y += 64;
}

void Pounder_Top_Fist::part_logic()
{
	if (Game::the_kid->location_x() >= location_.x + 8 &&
		Game::the_kid->location_x() <= location_.x + 32 &&
		Game::the_kid->location_y() <= location_.y &&
		activated_timer_ == 0)
	{
		//show_arm();
		location_.y -= 64;
		width_ratio_ = 1;
		height_ratio_ = 1;
		activated_timer_ = 2000;
	}
	else if (activated_timer_ > 0)
	{
		activated_timer_ -= g_timer.deltaT();
		if (activated_timer_ <= 0)
		{
			activated_timer_ = -1;
			hide_arm();
		}
	}
}

const double Pounder_Side_Fist::MAX_DISTANCE = 16;

void Pounder_Side_Fist::part_logic()
{
	switch (the_state_)
	{
	case Waiting:
		timer_ += g_timer.deltaT();
		if (timer_ >= frequency_)
		{
			timer_ -= frequency_;
			the_state_ = Moving_Up;
		}
		break;
	case Moving_Up:
		location_.y -= speed_ * g_timer.deltaT_in_seconds();
		if (location_.y <= starting_location_y - MAX_DISTANCE)
		{
			location_.y = starting_location_y - MAX_DISTANCE;
			the_state_ = Moving_Down;
		}
		break;
	case Moving_Down:
		location_.y += speed_ * 4 * g_timer.deltaT_in_seconds();
		if (location_.y >= starting_location_y)
		{
			location_.y = starting_location_y;
			Game::current_screen->add_a_new_smart_hazard(new Shockwave(location_.x + 24, location_.y - 14), false, true);
			the_state_ = Waiting;
		}
		break;
	}
}

void Pounder_Body::part_logic()
{}

void Pounder::kill_this_enemy()
{
	add_some_blood(10, the_parts_[0]->mid_point_x(), the_parts_[0]->mid_point_y());
	Game::current_screen->add_a_new_smart_object(new Explosion(the_parts_[0]->mid_point_x() - 22, the_parts_[0]->mid_point_y() - 22), false, true);
	prepare_to_delete();
}
