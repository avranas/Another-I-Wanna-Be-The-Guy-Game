#ifndef SPECIAL_PLATFORM_H_
#define SPECIAL_PLATFORM_H_
#include <SDL.h>
#include "globals.h"
#include "Image Processing.h"
#include <fstream>
#include <string>

class Special_Platform : public Displayable_Object
{
public:
	Special_Platform(double x, double y, double w, double h) :
		Displayable_Object(x, y),
		width_(w),
		height_(h),
		recent_collision_found_(false),
		velocity_x_(0),
		velocity_y_(0),
		previous_y_(y),
		test_for_collision_(true)
	{}

	virtual ~Special_Platform() {}
	virtual void logic() = 0;
	void move_platform_and_stick_to_kid();

	inline double left()const
	{
		return location_.x;
	}

	inline double top()const
	{
		return location_.y;
	}

	inline double right()const
	{
		return location_.x + width_;
	}

	inline double bottom()const
	{
		return location_.y + height_;
	}

	inline void set_x_velocity(double value)
	{
		velocity_x_ = value;
	}

	inline void set_y_velocity(double value)
	{
		velocity_y_ = value;
	}

	inline double x_velocity()const
	{
		return velocity_x_;
	}

	inline double y_velocity()const
	{
		return velocity_y_;
	}

	double previous_y()const
	{
		return previous_y_;
	}

	void set_previous_y(double value)
	{
		previous_y_ = value;
	}

	bool test_for_collision()
	{
		return test_for_collision_;
	}

	inline void collision_found()
	{
		recent_collision_found_ = true;
	}

	//If the kid is standing on the platform return true and change\
	recent_collision_found to false so we can check again the next frame.
	inline bool check_collision()
	{
		if (recent_collision_found_)
		{
			recent_collision_found_ = false;
			return true;
		}
		return false;
	}

protected:
	const double width_;
	const double height_;
	double previous_y_;
	double velocity_x_;
	double velocity_y_;
	bool recent_collision_found_;
	//Flip this to false if you want the platform to not test for collisions.
	bool test_for_collision_;
};

//This repeats its instructions.
class Moving_Platform : public Special_Platform
{
public:
	Moving_Platform(double x, double y, double w, double h, string file_name) :
		Special_Platform(x, y, w, h),
		current_instruction_(0),
		timer_(0)
	{
		Pic::platform_1->init();
		the_pics_.push_back(Pic::platform_1);
		ifstream platform_instructions("Instructions/" + file_name);
		while (!platform_instructions.eof())
		{
			double data = 0;
			platform_instructions >> data;
			x_velocity_list_.push_back(data);
			platform_instructions >> data;
			y_velocity_list_.push_back(data);
			platform_instructions >> data;
			max_time_.push_back(data);
		}
	}

	void logic();

private:
	vector<double> x_velocity_list_;
	vector<double> y_velocity_list_;
	vector<double> max_time_;
	int current_instruction_;
	double timer_;
};

//This runs through its instructions only once.
class Moving_Platform_2 : public Special_Platform
{
public:
	Moving_Platform_2(double x, double y, double w, double h, string file_name) :
		Special_Platform(x, y, w, h),
		current_instruction_(0),
		timer_(0),
		the_state_(Waiting)
	{
		add_new_texture(Pic::platform_2);
		ifstream platform_instructions("Instructions/" + file_name);
		while (!platform_instructions.eof())
		{
			double data = 0;
			platform_instructions >> data;
			x_velocity_list_.push_back(data);
			platform_instructions >> data;
			y_velocity_list_.push_back(data);
			platform_instructions >> data;
			max_time_.push_back(data);
		}
	}

	enum Platform_State
	{
		Waiting,
		Moving,
		Done
	};

	void logic();

private:
	vector<double> x_velocity_list_;
	vector<double> y_velocity_list_;
	vector<double> max_time_;
	int current_instruction_;
	double timer_;
	Platform_State the_state_;
};

class Falling_Platform : public Special_Platform
{
public:
	Falling_Platform(double x, double y, double w, double h) :
		Special_Platform(x, y, w, h),
		timer_(0),
		the_state_(Waiting),
		force_from_gravity_(0),
		fell_left_(false)
	{
		Pic::falling_platform_idle->init();
		the_pics_.push_back(Pic::falling_platform_idle);
		Pic::falling_platform_falling->init();
		the_pics_.push_back(Pic::falling_platform_falling);
		Pic::falling_platform_fallen->init();
		the_pics_.push_back(Pic::falling_platform_fallen);
		Pic::falling_platform_falling_reverse->init();
		the_pics_.push_back(Pic::falling_platform_falling_reverse);
		Pic::falling_platform_fallen_reverse->init();
		the_pics_.push_back(Pic::falling_platform_fallen_reverse);
	}
private:
	enum Falling_Platform_State
	{
		Waiting,
		Activated,
		Falling,
		Fallen
	};

	void logic();

	Falling_Platform_State the_state_;
	double timer_;
	const static double HOVERING_TIME;
	double force_from_gravity_;
	const static double TIME_UNTIL_FALLEN;
	bool fell_left_;
};

class Yellow_Switch : public Special_Platform
{
public:
	Yellow_Switch(double x, double y) :
		Special_Platform(x, y, 32, 32),
		the_state_(Waiting)
	{
		Pic::yellow_switch_idle->init();
		Pic::yellow_switch_pressed->init();
		the_pics_.push_back(Pic::yellow_switch_idle);
		the_pics_.push_back(Pic::yellow_switch_pressed);
	}

	void logic();

private:
	enum Switch_State
	{
		Waiting,
		Pressed
	};

	Switch_State the_state_;
};

class Green_Switch : public Special_Platform
{
public:
	Green_Switch(double x, double y) :
		Special_Platform(x, y, 32, 32),
		the_state_(Waiting)
	{
		Pic::green_switch_idle->init();
		Pic::green_switch_pressed->init();
		the_pics_.push_back(Pic::green_switch_idle);
		the_pics_.push_back(Pic::green_switch_pressed);
	}

	void logic();

private:
	enum Switch_State
	{
		Waiting,
		Pressed
	};

	Switch_State the_state_;

};



class Leaping_Platform : public Special_Platform
{
public:
	Leaping_Platform(double x, double y, double jump_power, double jump_frequency, double jump_height, double time_delay) :
		Special_Platform(x, y, 58, 40),
		jump_power_(jump_power),
		jump_frequency_(jump_frequency),
		jump_height_(jump_height),
		timer_(time_delay),
		starting_location_(y),
		y_velocity_(0),
		force_from_gravity_(0)
	{
		Pic::falling_platform_idle->init();
		the_pics_.push_back(Pic::falling_platform_idle);
	}

	void logic();

private:
	double force_from_gravity_;
	double y_velocity_;
	double starting_location_;
	double timer_;
	const double jump_power_;
	const double jump_frequency_;
	const double jump_height_;
};

class Spring : public Special_Platform
{
public:
	Spring(double x, double y, double jump_power, double distance, double speed, bool explodes) :
		Special_Platform(x, y, 28, 16),
		jump_power_(jump_power),
		spring_state(Idle),
		active_timer_(0),
		explodes_(explodes),
		distance_(distance),
		timer_(0),
		starting_x(x),
		speed_(speed)
	{
		Pic::spring_idle->init();
		the_pics_.push_back(Pic::spring_idle);
		Pic::spring_active->init();
		the_pics_.push_back(Pic::spring_active);
	}

	void logic();
private:
	enum Spring_State
	{
		Idle,
		Active,
		Exploding
	};
	Spring_State spring_state;
	double jump_power_;
	double active_timer_;
	const static double ACTIVE_TIME;
	bool explodes_;
	double timer_;
	double distance_;
	double starting_x;
	double speed_;
};
class Super_Spring : public Special_Platform
{
public:
	Super_Spring(double x, double y, double jump_power) :
		Special_Platform(x, y, 56, 64),
		jump_power_(jump_power),
		spring_state_(Idle),
		active_timer_(0)
	{
		add_new_texture(Pic::super_spring_idle);
		add_new_texture(Pic::super_spring_active);
		add_new_texture(Pic::super_spring_fail);
	}

	void logic();
private:
	enum Super_Spring_State
	{
		Idle,
		Active,
		Fail
	};
	Super_Spring_State spring_state_;
	double jump_power_;
	double active_timer_;
	const static double ACTIVE_TIME;
};

class Trolley : public Special_Platform
{
public:
	Trolley(double x, double y, double far_left, double far_right) :
		Special_Platform(x, y, 128, 42),
		the_state_(Idle),
		timer_(0),
		base_velocity_x(0),
		far_left_(far_left),
		far_right_(far_right),
		x_at_last_press_(0)
	{
		Pic::trolley_idle->init();
		the_pics_.push_back(Pic::trolley_idle);
		Pic::trolley_moving_left->init();
		the_pics_.push_back(Pic::trolley_moving_left);
		Pic::trolley_moving_right->init();
		the_pics_.push_back(Pic::trolley_moving_right);
	}

	void logic();

private:
	enum Trolley_State
	{
		Idle,
		Moving_Right,
		Moving_Left
	};

	double far_left_;
	double far_right_;
	Trolley_State the_state_;
	double timer_;
	double base_velocity_x;
	const double static BUTTON_WIDTH;
	const double static BUTTON_HEIGHT;
	const static double TOTAL_DISTANCE;
	const static double MAX_MOVING_TIME;
	double x_at_last_press_;
};

class One_Way_Platform : public Special_Platform
{
public:
	One_Way_Platform(double x, double y, double w, double h, Texture* pic) :
		Special_Platform(x, y, w, h)
	{
		pic->init();
		the_pics_.push_back(pic);
	}

	void logic();

private:
};

#endif
