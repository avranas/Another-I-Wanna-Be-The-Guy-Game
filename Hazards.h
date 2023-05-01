#ifndef _HAZARDS_H
#define _HAZARDS_H

#include <SDL.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "globals.h"
#include "Image Processing.h"
#include "Collision Information.h"

class Hazard : public Displayable_Object
{
public:
	Hazard(double x, double y, Collision_Data* data) :
		Displayable_Object(x, y),
		collision_data_(data)
	{}
	virtual ~Hazard() {}
	void draw_collision_data()
	{
		if (direction_ == SDL_FLIP_NONE)
			collision_data_->render(location_.x, location_.y, angle_, width_ratio_, height_ratio_);
		else
			collision_data_->render_mirrored(location_.x, location_.y, angle_, width_ratio_, height_ratio_);
	}
	bool test_collision_with_the_kid();

protected:
	Collision_Data* collision_data_;
};

class Smart_Hazard : public Hazard
{
public:
	Smart_Hazard(double x, double y, Collision_Data* data) :
		Hazard(x, y, data)
	{}
	virtual ~Smart_Hazard() {}
	//Smart hazards can actually do stuff!
	virtual void logic() = 0;
};

class Spike : public Hazard
{
public:
	Spike(double x, double y, double angle) :
		Hazard(x, y, Collision::spike)
	{
		angle_ = angle;
		add_new_texture(Pic::spike);
	}
private:
};

class Spinning_Saw : public Smart_Hazard
{
public:
	Spinning_Saw(double x, double y, double distance, double speed, double radius);
	void logic()
	{
		angle_ += g_timer.deltaT();
		timer_ += g_timer.deltaT_in_seconds() * speed_;
		location_.x = starting_location_.x + cos(timer_) * distance_;
		location_.y = starting_location_.y + sin(timer_) * distance_;
	}
private:
	const double distance_;
	const double radius_;
	double timer_;
	const double speed_;
	const Point starting_location_;
};

class Pendulum : public Smart_Hazard
{
public:
	Pendulum(double x, double y, double distance, double speed, double radius);

	void logic()
	{
		angle_ += g_timer.deltaT();
		timer_ += g_timer.deltaT_in_seconds() * speed_;
		double time_modifier = ((sin(timer_)) * 1.57);
		double offset_x = sin(time_modifier) * distance_;
		location_.x = starting_location_.x + offset_x;
		double offset_y = sqrt((distance_ * distance_) - (offset_x * offset_x));
		location_.y = starting_location_.y + offset_y;
	}

private:
	const double distance_;
	const double radius_;
	double timer_;
	const double speed_;
	const Point starting_location_;
};

class Moon : public Smart_Hazard
{
public:
	Moon(double x, double y, string file_name) :
		Smart_Hazard(x, y, Collision::moon),
		the_state_(Waiting),
		current_instruction_(0),
		distance_through_instruction_(0),
		location_at_start_of_instruction_x_(x),
		location_at_start_of_instruction_y_(y)
	{
		add_new_texture(Pic::moon);
		ifstream moon_instructions("Instructions/" + file_name);
		double radius = 0;
		moon_instructions >> radius;
		//143 is the length of the radius of the moon in the .png file.
		width_ratio_ *= (radius / 143);
		height_ratio_ *= (radius / 143);
		moon_instructions >> activation_box.x;
		moon_instructions >> activation_box.y;
		moon_instructions >> activation_box.w;
		moon_instructions >> activation_box.h;
		//copied from moving_platform
		while (!moon_instructions.eof())
		{
			double data = 0;
			moon_instructions >> data;
			speed.push_back(data);
			moon_instructions >> data;
			direction_angle.push_back(data);
			moon_instructions >> data;
			distance.push_back(data);
		}
	}

	void logic();

private:
	enum Moon_State
	{
		Waiting,
		Activated,
		Done
	};
	Rect activation_box;
	Moon_State the_state_;
	const static double RADIUS;
	vector<double> speed;
	vector<double> direction_angle;
	vector<double> distance;
	int current_instruction_;
	double distance_through_instruction_;
	double location_at_start_of_instruction_x_;
	double location_at_start_of_instruction_y_;
};

class Troll_Spike : public Smart_Hazard
{
	enum Troll_Spike_State
	{
		Waiting,
		Activated,
		Falling,
		Gone
	};
public:
	Troll_Spike(double x, double y, double angle, Rect activation_box, double shake_for_this_long, double moving_angle, double velocity) :
		Smart_Hazard(x, y, Collision::spike),
		its_state_(Waiting),
		max_shaking_time(shake_for_this_long),
		shaking_time_(0),
		mini_shaking_time_(0),
		shaking_side_(0),
		moving_angle_(moving_angle),
		velocity_(velocity),
		activation_box_(activation_box)
	{
		angle_ = angle;
		add_new_texture(Pic::spike);
	}
	void logic();

private:
	const static double SIZE;
	const static double MAX_SHAKING_TIME;
	const static double FALLING_VELOCITY;
	const static double SHAKING_FPS;
	const double max_shaking_time;
	const double moving_angle_;
	double mini_shaking_time_;
	double velocity_;
	int shaking_side_;
	double shaking_time_;
	Troll_Spike_State its_state_;
	Rect activation_box_;
};

class Magic_Projectile : public Smart_Hazard
{
public:
	Magic_Projectile(double x, double y, double angle);
	void logic();
private:
	double time_on_screen_;
	const static double MAX_TIME_ON_SCREEN;
	const static double HEIGHT;
	const static double WIDTH;
	const static double SPEED;
	Vector object_vector_;
};

class Lava : public Hazard
{
public:
	Lava(double x, double y) :
		Hazard(x, y, Collision::lava)
	{
		add_new_texture(Pic::no_pic);
	}

private:
};

class Hadouken : public Smart_Hazard
{
public:
	Hadouken(double x, double y, double angle);
	void logic();
private:
	double timer_;
	const static double HEIGHT;
	const static double WIDTH;
	const static double SPEED;
	double base_velocity_x;
	double base_velocity_y;
};

class Fireball : public Smart_Hazard
{
public:
	Fireball(double x, double y, double angle, bool use_gravity);
	void logic();
private:
	double timer_;
	const static double HEIGHT;
	const static double WIDTH;
	const static double SPEED;
	double base_velocity_x;
	double base_velocity_y;
	bool use_gravity_;
	double force_from_gravity_;
	const static double RAINSPEED;
};

class Thunderbolt : public Smart_Hazard
{
public:
	Thunderbolt(double x, double y) :
		Smart_Hazard(x, y, Collision::thunderbolt),
		timer_(0)
	{
		add_new_texture(Pic::thunderbolt);
	}
	void logic();
private:
	const static double SCREEN_TIME;
	double timer_;
};

class Thrown_Rock : public Smart_Hazard
{
public:
	Thrown_Rock(double x, double y, double velocity_x) :
		Smart_Hazard(x, y, Collision::thrown_rock),
		timer_(0),
		base_velocity_x(velocity_x),
		base_velocity_y(0),
		force_from_gravity_(-600),
		rotating_left_(false)
	{
		if (rand() % 2)
			rotating_left_ = true;
		add_new_texture(Pic::thrown_rock);
	}

	void logic();

private:
	bool rotating_left_;
	double timer_;
	double base_velocity_x;
	double base_velocity_y;
	double force_from_gravity_;
};

class Windmill : public Smart_Hazard
{
public:
	Windmill(double x, double y, double speed, double size) :
		Smart_Hazard(x, y, Collision::windmill),
		speed_(speed)
	{
		height_ratio_ *= size;
		width_ratio_ *= size;
		add_new_texture(Pic::windmill);
	}

	void logic();

private:
	double speed_;
};

class Shockwave : public Smart_Hazard
{
public:
	Shockwave(double x, double y) :
		Smart_Hazard(x, y, Collision::shockwave),
		time_left_(2000)
	{
		add_new_texture(Pic::shockwave);
	}

	void logic();

private:
	double time_left_;
};

class Spike_Wall : public Smart_Hazard
{
public:
	Spike_Wall(double location_x, double location_y, Rect activation_box) :
		Smart_Hazard(location_x, location_y, Collision::spike_wall),
		activation_box_(activation_box),
		time_left_(21300),
		the_state_(Waiting)
	{
		add_new_texture(Pic::spike_wall);
	}

	void logic();

private:
	enum Spike_Wall_State
	{
		Waiting,
		Rising,
		Chasing,
		Done
	};
	Rect activation_box_;
	double time_left_;
	Spike_Wall_State the_state_;
};

class Missile : public Smart_Hazard
{
public:
	Missile(double x, double y) :
		Smart_Hazard(x, y, Collision::missile),
		angle_velocity_(0),
		angle_velocity_force_(0),
		missile_speed_(100),
		missile_force_(10000)
	{
		add_new_texture(Pic::missile);
	}

	void logic();

private:
	double angle_velocity_;
	double angle_velocity_force_;
	double missile_speed_;
	double missile_force_;
};

class Falling_Spike : public Smart_Hazard
{
public:
	Falling_Spike(double x, double y) :
		Smart_Hazard(x, y, Collision::spike),
		yVelocity_(0),
		force_from_gravity_(0)
	{
		angle_ = 180;
		add_new_texture(Pic::spike);
	}

	void logic();

private:
	double yVelocity_;
	double force_from_gravity_;
};

class Moving_Spike : public Smart_Hazard
{
public:
	Moving_Spike(double x, double y, double yVelocity) :
		Smart_Hazard(x, y, Collision::falling_spike),
		yVelocity_(yVelocity),
		time_left_(8000)
	{
		add_new_texture(Pic::falling_spike);
	}

	void logic();

private:
	double yVelocity_;
	int time_left_;
};

class Crushing_Spikes : public Smart_Hazard
{
public:
	Crushing_Spikes(double x, double y) :
		Smart_Hazard(x, y, Collision::crushing_spikes),
		the_state_(Waiting),
		crushing_time_left_(4100)
	{
		add_new_texture(Pic::crushing_spikes);
		activation_box_.x = 4608;
		activation_box_.y = 336;
		activation_box_.w = 192;
		activation_box_.h = 192;
	}

	void logic();

private:
	enum Object_State
	{
		Waiting,
		Crushing,
		Done
	};

	Object_State the_state_;
	Rect activation_box_;
	int crushing_time_left_;
};

class Chasing_Spikes : public Smart_Hazard
{
public:
	Chasing_Spikes(double x, double y) :
		Smart_Hazard(x, y, Collision::chasing_spikes),
		the_state_(Waiting),
		timer_(0),
		activation_box_(Rect(5992, 0, 100, 576))
	{
		add_new_texture(Pic::chasing_spikes);
	}

	void logic();

private:
	enum Object_State
	{
		Waiting,
		Turning,
		Waiting2,
		Chasing
	};

	Object_State the_state_;
	const Rect activation_box_;
	int timer_;
};

class Rising_Spikes : public Smart_Hazard
{
public:
	Rising_Spikes(double x, double y) :
		Smart_Hazard(x, y, Collision::rising_spikes),
		the_state_(Waiting),
		timer_(0),
		activation_box_(Rect(5312, 48, 96, 528))
	{
		add_new_texture(Pic::rising_spikes);
	}

	void logic();

private:
	enum Object_State
	{
		Waiting,
		Rising,
	};

	Object_State the_state_;
	const Rect activation_box_;
	int timer_;
};



class Rainbow_Spike : public Smart_Hazard
{
public:
	Rainbow_Spike(double x, double y) :
		Smart_Hazard(x, y, Collision::spike),
		the_state_(Waiting),
		timer_(0),
		activation_box_(Rect(5424, 0, 64, 576)),
		center_(Point(x + RADIUS, y))
	{
		add_new_texture(Pic::spike);
	}

	void logic();

private:
	enum Object_State
	{
		Waiting,
		Active
	};

	Object_State the_state_;
	const Rect activation_box_;
	int timer_;
	const Point center_;
	const static double RADIUS;
};

class Poking_Spike : public Smart_Hazard
{
public:
	Poking_Spike(double x, double y, double angle, double distance, double frequency, double init_percent_thru_cycle) :
		Smart_Hazard(x, y, Collision::spike),
		distance_(distance),
		frequency_(frequency),
		timer_(frequency* init_percent_thru_cycle),
		starting_location_y_(y),
		starting_location_x_(x)
	{
		angle_ = angle;
		add_new_texture(Pic::spike);
	}

	void logic();

private:
	const double starting_location_x_;
	const double starting_location_y_;
	const double distance_;
	const double frequency_;
	double timer_;
};

#endif