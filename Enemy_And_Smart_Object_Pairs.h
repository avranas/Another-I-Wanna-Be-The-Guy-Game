#ifndef ENEMY_AND_SMART_OBJECT_PAIRS_H_
#define ENEMY_AND_SMART_OBJECT_PAIRS_H_
#include "Enemy.h"
#include "Smart Object.h"
#include "Special Platform.h"

class Super_Carock_Body : public Enemy_Part
{
public:
	Super_Carock_Body(double x, double y, Enemy* parent_object) :
		Enemy_Part(x, y, parent_object),
		//copied from Boss_Platform
		circle_around_this_(Point((g_screen_width * 9) + (g_screen_width / 2), (g_screen_height + (g_screen_height / 2)) + 50)),
		current_state_(Introduction),
		clock_wise_(true),
		circle_speed_(100),
		sparkle_timer_(new Frequency_Timer(0.03)),
		attack_timer_(new Frequency_Timer(default_fire_frequency)),
		alternating_sine_wave_timer_(1),
		position_a_(false),
		new_attack_timer_(default_new_attack_frequency),
		ready_for_a_new_attack_(false),
		normal_sine_wave_speed_(300),
		charge_target_(0, 0), //gets set when action is set to charge,
		charge_vector_(charge_power, 0), //set the angle later, brah
		home_point(&right_home_point),
		speed_multiplier_(1),
		smoke_timer_(0.03),
		explode_timer_(0.06),
		death_exploding_timer_(0.15),
		death_blood_timer_(0.06),
		die_time_(10),
		position_in_move_queue(0),
		number_of_moves(4)
	{
		set_move_queue();
		direction_ = SDL_FLIP_HORIZONTAL;
		add_new_texture_with_collision(Pic::carock_idle, Pic::carock_idle_alt, Collision::carock_idle);
		add_new_texture_with_collision(Pic::carock_casting, Pic::carock_casting_alt, Collision::carock_casting);
	}

	~Super_Carock_Body()
	{
		delete sparkle_timer_;
	}

	void use_mushroom();
	void part_logic();

	void teleport(Point new_point)
	{
		location_ = new_point;
	}

	void set_home_point_to_left()
	{
		home_point = &left_home_point;
		location_ = *home_point;
		normal_sine_wave_speed_ = -300;
		direction_ = SDL_FLIP_NONE;
	}

	void set_home_point_to_right()
	{
		home_point = &right_home_point;
		location_ = *home_point;
		normal_sine_wave_speed_ = 300;
		direction_ = SDL_FLIP_HORIZONTAL;
	}

	void die();

	void switch_to_normal_state()
	{
		change_state(Normal);
		attack_timer_->change_max_time(default_fire_frequency / speed_multiplier_);
		new_attack_timer_.change_max_time(default_new_attack_frequency / speed_multiplier_);
		circle_speed_ = 0;
		location_ = *home_point;
		angle_ = 0;
	}

	//test if we're in the right position
	inline bool ready_to_switch_state()
	{
		return true;
	}

private:
	enum Boss_State
	{
		Introduction,
		Intro_To_Main,
		Growing,
		Normal,
		Circling,
		Follow,
		Follow_To_Normal_Transition,
		Charge,
		Charge_2,
		Return_From_Charge,
		Explode
	};

	void change_state(Boss_State new_state)
	{
		ready_for_a_new_attack_ = false;
		current_state_ = new_state;
		reset_actions();
	}

	void pick_attack();
	void set_move_queue();

	Boss_State current_state_;
	Point circle_around_this_;
	bool clock_wise_;
	double circle_speed_;
	Frequency_Timer* sparkle_timer_;
	Frequency_Timer* attack_timer_;
	Frequency_Timer alternating_sine_wave_timer_;
	Frequency_Timer new_attack_timer_;
	Frequency_Timer smoke_timer_;
	Frequency_Timer explode_timer_;
	Frequency_Timer death_exploding_timer_;
	Frequency_Timer death_blood_timer_;
	Countdown die_time_;
	bool ready_for_a_new_attack_;
	const static double alternating_sine_wave_position_a_;
	const static double alternating_sine_wave_position_b_;
	bool position_a_;
	const Point* home_point;
	const static Point right_home_point;
	const static Point left_home_point;
	const static double default_fire_frequency;
	const static double default_new_attack_frequency;
	const static double follow_velocity;
	double normal_sine_wave_speed_;
	double speed_multiplier_;

	//Used for Charge attack
	Point charge_target_;
	Vector charge_vector_;
	const static double charge_power;
	int move_queue[4];
	int position_in_move_queue;
	const int number_of_moves;
};


//Our first real boss!
class Super_Carock : public Enemy
{
public:
	Super_Carock();
	void use_mushroom();
	void kill_this_enemy();

private:
	Super_Carock_Body* super_carock_body_;
};


class Mushroom : public Smart_Object
{
public:
	Mushroom(double x, double y, Super_Carock* target) :
		Smart_Object(x, y),
		target_enemy_(target),
		started_(false)
	{
		add_new_texture(Pic::mushroom);
	}

	void logic();

private:
	Super_Carock* target_enemy_;
	bool started_;
};


class Boss_Platform : public Special_Platform
{
public:
	Boss_Platform(Point end_point, double angle, Super_Carock* boss) :
		Special_Platform(0, 0, 64, 16),
		position_angle_(angle),
		the_state_(Normal),
		boss_(boss),
		end_point_(end_point)
	{
		add_new_texture(Pic::boss_platform);
		location_.x = mid_point.x + cos_degrees(position_angle_.angle()) * distance;
		location_.y = mid_point.y + sin_degrees(position_angle_.angle()) * distance;
	}

	void logic();
	void set_position();

private:
	enum Boss_Platform_State
	{
		Normal,
		Part_2,
		Done
	};

	Boss_Platform_State the_state_;
	const static Point mid_point;
	const static double distance;
	Angle position_angle_;
	Super_Carock* boss_;
	Point end_point_;
	const static double end_speed;
};

#endif
