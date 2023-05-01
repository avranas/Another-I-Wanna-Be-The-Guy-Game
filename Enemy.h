#ifndef ENEMY_H_
#define ENEMY_H_
#include <SDL.h>
#include "globals.h"
#include "Image Processing.h"
#include <fstream>
#include <vector>
#include "Collision Information.h"
#include "Bullet.h"

class Smart_Object;
//An enemy does collision tests with the kid, has logic, and takes damage from bullets.
class Enemy;

class Enemy_Part : public Displayable_Object
{
public:
	Enemy_Part(double x, double y, Enemy* parent_object);

	virtual ~Enemy_Part()
	{
		delete particle_generator_;
	}

	//Remember not to add new parts here, you will likely invalidate an iterator during runtime.
	virtual void part_logic() = 0;
	bool kid_part_collision();
	bool part_bullet_collision(Bullet* bullet);
	inline void delete_enemy_part()
	{
		ready_to_delete_ = true;
		delete this;
	}

	bool harmless()
	{
		return harmless_;
	}

	virtual void die()
	{
		make_harmless();
		prepare_to_delete();
	}

	inline void flip_to_alt_pic()
	{
		temp_texture_ = change_pic_for_some_time(the_alt_pics_[current_animation_], current_animation_);
	}

	inline void draw_collision_data()
	{
		if (direction_ == SDL_FLIP_NONE)
			collision_data_[current_animation_]->render(location_.x, location_.y, angle_, width_ratio_, height_ratio_);
		else
			collision_data_[current_animation_]->render_mirrored(location_.x, location_.y, angle_, width_ratio_, height_ratio_);
	}

	inline void revert_to_original_pic()
	{
		revert_to_this_pic(temp_texture_);
	}

	inline void modify_radius_of_circle(double value)
	{
		width_ratio_ = value / width();
	}

	//Never change the size of a circle with this; use modify_radius_of_circle() instead.
	inline void modify_height_of_current_collision(double value)
	{
		height_ratio_ = value / height();
	}

	//Never change the size of a circle with this; use modify_radius_of_circle() instead.
	inline void modify_width_of_current_collision(double value)
	{
		width_ratio_ = value / width();
	}

	void kill_this_part()
	{
		dying_ = true;
	}

	Particle_Generator* particle_generator()
	{
		return particle_generator_;
	}

	void make_harmless()
	{
		harmless_ = true;
	}

	void make_harmfull()
	{
		harmless_ = false;
	}

protected:
	vector<Collision_Data*> collision_data_;
	//The following is used for when the enemy is hit and changes its animation temporarily.
	//immortal enemies and those with 1 HP don't need alt_pics
	vector<Texture*> the_alt_pics_;
	//When I switch to alternative pics, this is the temp slot for the old one.
	Texture* temp_texture_;
	Enemy* parent_object_;
	bool dying_;

	inline void add_new_texture_with_collision(Texture* new_pic, Texture* new_alt_pic, Collision_Data* new_collision)
	{
		new_pic->init();
		the_pics_.push_back(new_pic);
		collision_data_.push_back(new_collision);
		new_alt_pic->init();
		the_alt_pics_.push_back(new_alt_pic);
	}

	//This is also for the new action thingy. Return true if done
	void reset_actions()
	{
		sine_wave_time_ = 0;
		sine_start_x_ = 0;
		sine_start_y_ = 0;
		rotating_time_ = 0;
		circling_time_ = 0;
		circling_angle_.set_angle(0);
		moving_time_ = 0;
		acceling_time_ = 0;
		x_accel_force_ = 0;
		y_accel_force_ = 0;
		x_velocity_ = 0;
		y_velocity_ = 0;
	}

	//returns true if the point is passed.
	bool target_a_point(double x, double y, double speed)
	{
		double new_angle = calculate_angle_between_points_in_degrees(location_, Point(x, y));
		double new_position_x = location_.x + cos_degrees(new_angle) * speed * g_timer.deltaT_in_seconds();
		double new_position_y = location_.y + sin_degrees(new_angle) * speed * g_timer.deltaT_in_seconds();
		bool x_good = false;
		bool y_good = false;
		if ((new_position_x >= x && location_.x < x) ||
			(new_position_x <= x && location_.x > x))
			x_good = true;
		if ((new_position_y >= y && location_.y < y) ||
			(new_position_y <= y && location_.y > y))
			y_good = true;
		if (y_good && x_good)
		{
			location_.y = y;
			location_.x = x;
			return true;
		}
		location_.x = new_position_x;
		location_.y = new_position_y;
		return false;
	}

	bool action_verticle_sine_wave(double speed, double distance, double how_long)
	{
		if (sine_wave_time_ == 0)
		{
			sine_start_y_ = location_.y;
		}
		else if (sine_wave_time_ >= how_long && how_long != -1)
		{
			sine_wave_time_ = 0;
			return true;
		}
		sine_wave_time_ += g_timer.deltaT_in_seconds() * speed;
		location_.y = sine_start_y_ + sin_degrees(sine_wave_time_) * distance;
		return false;
	}

	bool action_sine_wave(double speed, double distance, double angle, double how_long)
	{
		//This function has a bug. When direction != 0 I get the intended action of\
		circle_around_a_point(). Fix this. But use this to make that work too. Thank you.
		if (sine_wave_time_ == 0)
		{
			sine_start_x_ = location_.x;
			sine_start_y_ = location_.y;
		}
		else if (sine_wave_time_ >= how_long && how_long != -1)
		{
			sine_wave_time_ = 0;
			return true;
		}
		double new_distance_x = distance * cos_degrees(angle);
		double new_distance_y = distance * sin_degrees(angle);
		sine_wave_time_ += g_timer.deltaT_in_seconds() * speed;
		location_.x = sine_start_x_ + cos_degrees(sine_wave_time_) * new_distance_x;
		location_.y = sine_start_y_ + sin_degrees(sine_wave_time_) * new_distance_y;
		return false;
	}

	bool action_rotate(double angle_speed, double how_long)
	{
		if (rotating_time_ >= how_long && how_long != -1)
			return true;
		rotating_time_ += g_timer.deltaT_in_seconds();
		angle_ += angle_speed * g_timer.deltaT_in_seconds();
		return false;
	}

	bool action_circle_around_a_point(Point this_point, double circle_speed, double distance, double how_long)
	{
		//start circling
		if (circling_time_ == 0)
		{
			circling_angle_.set_angle(calculate_angle_between_points_in_degrees(this_point, location_));
		}
		circling_time_ += g_timer.deltaT_in_seconds();
		circling_angle_.increase_by(g_timer.deltaT_in_seconds() * circle_speed);
		location_.x = this_point.x + distance * cos_degrees(circling_angle_.angle());
		location_.y = this_point.y + distance * sin_degrees(circling_angle_.angle());
		angle_ = circling_angle_.angle();
		if (circling_time_ >= how_long && how_long != -1)
			return true;
		return false;
	}

	bool action_move(double x_velocity, double y_velocity, double how_long)
	{
		if (moving_time_ >= how_long)
			return true;
		moving_time_ += g_timer.deltaT_in_seconds();
		location_.x += x_velocity * g_timer.deltaT_in_seconds();
		location_.y += y_velocity * g_timer.deltaT_in_seconds();
		return false;
	}

	bool action_accel(double x_accel, double y_accel, double how_long)
	{
		if (acceling_time_ >= how_long && how_long != -1)
			return true;
		acceling_time_ += g_timer.deltaT_in_seconds();
		modify_velocity_with_force(&x_velocity_, &x_accel_force_, x_accel);
		modify_velocity_with_force(&y_velocity_, &y_accel_force_, y_accel);
		location_.x += x_velocity_;
		location_.y += y_velocity_;
		return false;
	}

private:
	void add_new_texture(Texture* the_pic);
	Particle_Generator* particle_generator_;
	//This is all for the new action thing
	double sine_wave_time_;
	double sine_start_x_;
	double sine_start_y_;
	double rotating_time_;
	double circling_time_;
	Angle circling_angle_;
	double moving_time_;
	double acceling_time_;
	double x_accel_force_;
	double y_accel_force_;
	double x_velocity_;
	double y_velocity_;
	bool harmless_;
};

class Enemy
{
public:
	//Set HP to -1 if you want your enemy to be immortal.
	Enemy(int HP) :
		HP_(HP),
		MAX_HP_(HP),
		dying_(false),//usually I want to do something before the enemy is deleted.
		ready_to_delete_(false),
		time_left_in_alt_pic(0)
	{}

	~Enemy() {}

	inline void add_a_new_part(Enemy_Part* new_part)
	{
		the_parts_.push_back(new_part);
	}

	void draw_collision_boundaries()
	{
		for (auto iter = the_parts_.begin(); iter != the_parts_.end(); ++iter)
			(*iter)->draw_collision_data();
	}

	//All enemies perform this in each loop
	inline void common_logic()
	{
		if (time_left_in_alt_pic > 100)
		{
			time_left_in_alt_pic -= g_timer.deltaT();
			if (time_left_in_alt_pic <= 100)
			{
				time_left_in_alt_pic = 0;
				for (auto iter = the_parts_.begin(); iter != the_parts_.end(); ++iter)
					(*iter)->revert_to_original_pic();
			}
		}
		for (auto iter = the_parts_.begin(); iter != the_parts_.end(); ++iter)
		{
			if (!dying_)
				(*iter)->part_logic();
			else
				(*iter)->die();
		}
	}

	inline void render_enemy()
	{
		for_each(the_parts_.begin(), the_parts_.end(), mem_fun(&Displayable_Object::draw_to_screen));
	}

	inline bool kid_collision()
	{
		for (auto iter = the_parts_.begin(); iter != the_parts_.end(); ++iter)
			if ((*iter)->kid_part_collision() && !(*iter)->harmless())
				return true;
		return false;
	}

	inline bool bullet_collision(Bullet* bullet)
	{
		for (auto iter = the_parts_.begin(); iter != the_parts_.end(); ++iter)
			if ((*iter)->part_bullet_collision(bullet))
			{
				bullet->prepare_to_delete();
				take_damage();
				return true;
			}
		return false;
	}

	//This is all the stuff that happens once the enemy dies. If you want to play a certain sound,\
	add some blood, play an animation, change a state, delete the enemy from the screen: do it here.
	virtual void kill_this_enemy()
	{
		dying_ = true;
	}

	inline double HP_percentage() { return static_cast<double>(HP_) / static_cast<double>(MAX_HP_); }
	void take_damage();
	inline bool dead() { return dying_; }

	inline void prepare_to_delete()
	{
		dying_ = true;
		ready_to_delete_ = true;
		for (auto iter = the_parts_.begin(); iter != the_parts_.end(); ++iter)
			(*iter)->prepare_to_delete();
	}

	inline bool delete_enemy_if_ready()
	{
		if (ready_to_delete_)
		{
			for_each(the_parts_.begin(), the_parts_.end(), mem_fun(&Enemy_Part::delete_enemy_part));
			delete this;
		}
		return ready_to_delete_;
	}

	vector<Enemy_Part*> the_parts_;

protected:
	//This is only for bosses, bro.
	//Call this in all constructors for bosses, bro. And don't forget.
	void modify_health_based_on_difficulty();
	int HP_;
	int MAX_HP_;
	bool dying_;
	int time_left_in_alt_pic;
	bool ready_to_delete_;
};

inline void collision_test_with_enemies(Bullet* the_bullet);

//First boss!
class Barry_Body : public Enemy_Part
{
public:
	Barry_Body(double x, double y, Enemy* parent_object) :
		Enemy_Part(x, y, parent_object), //130
		barry_action(Intro_Walk),
		previous_action(Intro_Walk),
		timer_(0),
		action_timer_(0),
		fireball_count(0),
		fireball_angle(0),
		turns_until_next_hadouken(1),
		entry_point(BARRY_HAZARD_ENTRY),
		blood_frequency_timer_(0),
		die_timer_(0)
	{
		//I need do this because I messed up.
		height_ratio_ = 0.9;
		width_ratio_ = 0.9;
		add_new_texture_with_collision(Pic::barry_idle, Pic::barry_idle_alt, Collision::barry_idle);
		add_new_texture_with_collision(Pic::barry_attack, Pic::barry_attack_alt, Collision::barry_attack);
		add_new_texture_with_collision(Pic::barry_dying, Pic::no_pic, Collision::no_collision);
	}
	void part_logic();

private:
	enum Barry_Action
	{
		Intro_Walk,
		Use_Hadouken,
		Firespin_Warning,
		Use_Firespin,
		Use_Fireball_Rain,
		Use_Thundercloud,
		Use_Spike_Rain,
		Wait
	};

	void pick_random_attack();

	inline void finish_attack()
	{
		action_timer_ = 0;
		barry_action = Wait;
	}

	Point entry_point;
	Barry_Action barry_action;
	Barry_Action previous_action;
	double timer_;
	double action_timer_;
	Point const static BARRY_HAZARD_ENTRY;
	const static double HADOUKEN_FREQUENCY;
	const static double FIRESPIN_FREQUENCY;
	const static double FIREBALL_RAIN_FREQUENCY;
	const static double WAIT_TIME;
	const static double FIRESPIN_WARNING_TIME;
	int fireball_count;
	const static int FIRESPIN_COUNT = 17;
	const static int FIRERAIN_COUNT = 19;
	//I use this for the blood splatter during the death sequence.
	double blood_frequency_timer_;
	double fireball_angle;
	double die_timer_;
	int turns_until_next_hadouken;
};

class Frog_Body : public Enemy_Part
{
public:
	Frog_Body(double x, double y, Enemy* parent_object) :
		Enemy_Part(x, y, parent_object),
		timer_(0),
		y_velocity(0),
		force_from_gravity(0),
		starting_location_y(124),
		current_frequency(0)
	{
		add_new_texture_with_collision(Pic::frog_idle, Pic::no_pic, Collision::frog_idle);
		add_new_texture_with_collision(Pic::frog_jumping, Pic::no_pic, Collision::frog_jumping);
	}

	void part_logic();

private:
	const double starting_location_y;
	double current_frequency;
	double current_jump_power;
	const static double SPEED;
	const static int MAX_JUMP_POWER = 80;
	const static int MIN_JUMP_POWER = 270;
	const static int MIN_JUMP_FREQUENCY = 80;
	const static int MAX_JUMP_FREQUENCY = 100;
	double timer_;
	double y_velocity;
	double force_from_gravity;
};

class Frog : public Enemy
{
public:
	Frog(double x, double y) :
		Enemy(1)
	{
		the_parts_.push_back(new Frog_Body(x, y, this));
	}
	void kill_this_enemy();
private:
};



class Barry : public Enemy
{
public:
	Barry() :
		Enemy(13)
	{
		the_parts_.push_back(new Barry_Body(1025, 60, this));
		//Do this for all bosses!
		modify_health_based_on_difficulty();
	}
	void kill_this_enemy();
private:
};

class Pounder_Body : public Enemy_Part
{
public:
	Pounder_Body(double x, double y, Enemy* parent_object) :
		Enemy_Part(x, y, parent_object),
		timer_(0),
		top_fist_activated_(false)
	{
		add_new_texture_with_collision(Pic::pounder_idle, Pic::pounder_idle_alt, Collision::pounder);
	}
	void part_logic();
	void kill_this_enemy();
private:
	double timer_;
	int frequency_;
	bool top_fist_activated_;
};

class Pounder_Top_Fist : public Enemy_Part
{
public:
	Pounder_Top_Fist(double x, double y, Enemy* parent_object) :
		Enemy_Part(x, y, parent_object),
		timer_(0),
		activated_timer_(0)
	{
		add_new_texture_with_collision(Pic::pounder_top_fist, Pic::pounder_top_fist_alt, Collision::pounder_top_fist);
		hide_arm();
	}
	void part_logic();
private:
	void hide_arm();
	int timer_;
	double activated_timer_;
	const static int TIME_ON_SCREEN;
};

class Pounder_Side_Fist : public Enemy_Part
{
public:
	Pounder_Side_Fist(double x, double y, Enemy* parent_object, int frequency, double speed) :
		Enemy_Part(x, y, parent_object),
		frequency_(frequency),
		speed_(speed),
		timer_(0),
		starting_location_y(y),
		the_state_(Waiting)
	{
		add_new_texture_with_collision(Pic::pounder_side_fist, Pic::pounder_side_fist_alt, Collision::pounder_side_fist);
	}
	void part_logic();

private:
	enum Fist_State
	{
		Moving_Up,
		Moving_Down,
		Waiting
	};

	double speed_;
	Fist_State the_state_;
	int timer_;
	const static int TIME_ON_SCREEN;
	const static double MAX_DISTANCE;
	const double starting_location_y;
	int frequency_;
};

class Pounder : public Enemy
{
public:
	Pounder(double x, double y, int frequency, double speed) :
		Enemy(10)
	{
		//The order matters because of layering
		add_a_new_part(new Pounder_Top_Fist(x + 8, y - 64, this));
		add_a_new_part(new Pounder_Body(x, y, this));
		add_a_new_part(new Pounder_Side_Fist(x + 16, y + 32, this, frequency, speed));
	}

	void kill_this_enemy();

private:
};

class Carock_Body : public Enemy_Part
{
public:
	//This constructor does things differently. Param 1 is the screen Carock is on. Param 2 is the same; y_position
	Carock_Body(int screen, double y, Enemy* parent_object) :
		Enemy_Part(screen* g_screen_width + starting_postion_x_, y, parent_object),
		the_casting_state_(Idle),
		the_position_state_(Do_Nothing),
		state_timer_(0),
		position_timer_(0),
		offset_y(0),
		starting_location_y_(location_.y),
		distance(110),
		speed(9),
		move_this_much(470),
		left_or_right_timer_(0),
		transition_distance_(0),
		start_screen_(screen)
	{
		//This gives Carock a random starting position
		position_timer_ = rand() % static_cast<int>(speed);
		direction_ = SDL_FLIP_HORIZONTAL;

		add_new_texture_with_collision(Pic::carock_idle, Pic::no_pic, Collision::carock_idle);
		add_new_texture_with_collision(Pic::carock_casting, Pic::no_pic, Collision::carock_casting);
	}

	void part_logic();
private:
	enum Casting_State
	{
		Idle,
		Casting,
	};

	enum Position_State
	{
		Do_Nothing,
		Screen_1,
		Screen_Transition_1,
		Screen_2,
		Screen_Transition_2,
		Waiting_In_Screen_3,
		Screen_3,
		Moving_Away
	};

	const static Rect finish_activation_box;
	Casting_State the_casting_state_;
	Position_State the_position_state_;
	double state_timer_;
	double position_timer_;
	double left_or_right_timer_;
	const static double SCREEN_1_CASTING_FREQUENCY;
	const static double CASTING_TIME;
	const static double SCREEN_3_ALTERNATING_TIMER;
	double move_this_much;
	double distance;
	double offset_y;
	double starting_location_y_;
	double speed;
	double transition_distance_;
	const static double starting_postion_x_;
	int start_screen_;
};


class Carock : public Enemy
{
public:
	Carock(int screen_x, double pos_y) :
		Enemy(-1)
	{
		the_parts_.push_back(new Carock_Body(screen_x, pos_y, this));
	}

	void kill_this_enemy();

private:
};

#endif