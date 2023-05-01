#ifndef SIMPLE_OBJECT_H_
#define SIMPLE_OBJECT_H_
#include <SDL.h>
#include "globals.h"
#include "Image Processing.h"
#include <fstream>
#include <string>

//An object with some logic that can be rendered to the screen.
class Smart_Object : public Displayable_Object
{
public:
	Smart_Object(double x, double y) :
		Displayable_Object(x, y)
	{}
	virtual void logic() = 0;
	virtual ~Smart_Object() {}
};

class Blood : public Smart_Object
{
public:
	Blood(double x, double y) :
		Smart_Object(x, y)
	{
		angle_ = rand() % 360;
	}

	void logic();

protected:
	const static double BLOOD_WIDTH;
	const static double BLOOD_HEIGHT;
	double velocity_x_;
	double velocity_y_;
	double force_from_gravity_;
};

class Blood_1 : public Blood
{
public:
	Blood_1(double x, double y, double velocity_x, double velocity_y) :
		Blood(x, y)
	{
		velocity_x_ = velocity_x;
		velocity_y_ = velocity_y;
		force_from_gravity_ = velocity_y;
		add_new_texture(Pic::Blood_1);
	}
};

class Blood_2 : public Blood
{
public:
	Blood_2(double x, double y, double velocity_x, double velocity_y) :
		Blood(x, y)
	{
		velocity_x_ = velocity_x;
		velocity_y_ = velocity_y;
		force_from_gravity_ = velocity_y;
		add_new_texture(Pic::Blood_2);
	}
};

class Blood_3 : public Blood
{
public:
	Blood_3(double x, double y, double velocity_x, double velocity_y) :
		Blood(x, y)
	{
		velocity_x_ = velocity_x;
		velocity_y_ = velocity_y;
		force_from_gravity_ = velocity_y;
		add_new_texture(Pic::Blood_3);
	}
};

void add_some_blood(int this_much, double x, double y);

class Gate : public Smart_Object
{
public:
	Gate(double x, double y) :
		Smart_Object(x, y),
		button_(Rect(x, y, BUTTON_WIDTH, BUTTON_HEIGHT)),
		the_state_(Closed),
		opening_timer_(0)
	{
		Pic::gate_closed->init();
		Pic::gate_opening->init();
		Pic::gate_open->init();
		the_pics_.push_back(Pic::gate_closed);
		the_pics_.push_back(Pic::gate_opening);
		the_pics_.push_back(Pic::gate_open);
	}
	void logic();

private:
	enum Gate_State
	{
		Closed,
		Opening,
		Open
	};
	Gate_State the_state_;
	double opening_timer_;
	const static double BUTTON_OFFSET_X;
	const static double BUTTON_OFFSET_Y;
	const static double BUTTON_WIDTH;
	const static double BUTTON_HEIGHT;
	const static double OPENING_TIME;
	const Rect button_;
};

class Axe : public Smart_Object
{
public:
	Axe(double x, double y) :
		Smart_Object(x, y),
		the_state_(Waiting),
		timer_(0),
		tile_counter_(1)
	{
		Pic::axe->init();
		the_pics_.push_back(Pic::axe);
		Pic::no_pic->init();
		the_pics_.push_back(Pic::no_pic);
	}

	void logic();

	Rect hit_box()
	{
		return Rect(location_.x, location_.y, WIDTH, HEIGHT);
	}

private:
	enum Axe_State
	{
		Waiting,
		Destroying,
		Done
	};

	int tile_counter_;
	Axe_State the_state_;
	double timer_;
	const static double WIDTH;
	const static double HEIGHT;
};

class Thundercloud : public Smart_Object
{
public:
	Thundercloud(double x, double y, double wait_time) :
		Smart_Object(x, y),
		waiting_time(wait_time),
		timer_(0),
		the_cloud_state_(Waiting)
	{
		Pic::thundercloud_waiting->init();
		the_pics_.push_back(Pic::thundercloud_waiting);
		Pic::thundercloud_warning->init();
		the_pics_.push_back(Pic::thundercloud_warning);
		Pic::thundercloud_active->init();
		the_pics_.push_back(Pic::thundercloud_active);
	}

	void logic();

private:
	enum Cloud_State
	{
		Waiting,
		Warning,
		Active
	};

	const double waiting_time;
	const static double WARNING_TIME;
	const static double ACTIVE_TIME;
	Cloud_State the_cloud_state_;
	double timer_;
};

class Big_Fireball : public Smart_Object
{
public:
	Big_Fireball(Point* entry_point) :
		Smart_Object(entry_point->x, entry_point->y),
		follow_this_(entry_point),
		timer_(0)
	{
		Pic::big_fireball->init();
		the_pics_.push_back(Pic::big_fireball);
	}

	void logic();

private:
	Point* follow_this_;
	double timer_;
	const static double SCREEN_TIME;
};

class Link : public Smart_Object
{
public:
	Link(double x, double y) :
		Smart_Object(x, y),
		timer_(0),
		dig_frequency_(1000)
	{
		Pic::link_shoveling->init();
		the_pics_.push_back(Pic::link_shoveling);
	}

	void logic();

private:
	double timer_;
	double dig_frequency_;
};

class Explosion : public Smart_Object
{
public:
	Explosion(double x, double y);
	void logic();

private:
	double timer_;
	const static double EXPLOSION_TIME;
};

class Mini_Axe : public Smart_Object
{
public:
	Mini_Axe(double x, double y, bool left);
	void logic();

private:
	Rect hit_box_;
	bool left_;
};

void randomly_place_explosion_in_a_box(Rect box);

#endif