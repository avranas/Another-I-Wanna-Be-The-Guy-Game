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

class Firework : public Smart_Object
{
public:
	Firework(double x, double y) :
		Smart_Object(x, y),
		countdown_to_explosion_((static_cast<double>((rand() % 750) + 500) / 1000)),
		velocity_x_(0),
		velocity_y_(0)
	{
		width_ratio_ = 0.6;
		double new_angle = static_cast<double>(rand() % 40 + 250);
		angle_ = new_angle - 270;
		Vector new_vector(initial_power, new_angle);
		velocity_x_ = new_vector.magnitude_x();
		velocity_y_ = new_vector.magnitude_y();
		add_new_texture(Pic::firework);
	}
	void logic();
private:
	Countdown countdown_to_explosion_;
	double velocity_x_;
	double velocity_y_;
	const static double initial_power;
};

class Spark : public Smart_Object
{
public:
	Spark(double x, double y, Vector the_vector) :
		Smart_Object(x, y),
		the_vector_(the_vector),
		time_left_(0.5),
		create_trail_timer_(0.016),
		force_from_gravity_(0),
		extra_velocity_(0)
	{
		add_new_texture(Pic::spark);
	}
	void logic();

private:
	Vector the_vector_;
	Countdown time_left_;
	Frequency_Timer create_trail_timer_;
	double force_from_gravity_;
	double extra_velocity_;
};



class Trail : public Particle
{
public:
	Trail(double x, double y) :
		Particle(Pic::sparkle, 0.1, x, y, 0, 0, false)
	{

	}
private:
};

class Alex : public Smart_Object
{
public:
	Alex(double x, double y) :
		Smart_Object(x, y),
		left_edge(x - 8),
		right_edge(x + 8),
		going_left_(true)
	{
		add_new_texture(Pic::alex_2);
	}
	void logic();
private:
	const double left_edge;
	const double right_edge;
	bool going_left_;
};


class Cool_Background_Tile : public Smart_Object
{
public:
	Cool_Background_Tile(double x, double y, Texture* pic, double top, double bottom) :
		Smart_Object(x, y),
		top_(top),
		bottom_(bottom)
	{
		add_new_texture(pic);
	}
	void logic();
private:
	double top_;
	double bottom_;
};

class Congrats : public Smart_Object
{
public:
	Congrats(double x, double y) :
		Smart_Object(x, y),
		timer_(0)
	{
		add_new_texture(Pic::congrats);
	}

	void logic();
private:
	double timer_;
};

class Game_Over_Text : public Smart_Object
{
public:
	Game_Over_Text(double x, double y) :
		Smart_Object(x, y),
		timer_(0),
		offset_x_(x),
		offset_y_(y)
	{
		add_new_texture(Pic::game_over);
	}

	void logic();
private:
	double timer_;
	const double offset_x_;
	const double offset_y_;
};

class Portal : public Smart_Object
{
public:
	Portal(double x, double y) :
		Smart_Object(x, y),
		timer_(0)
	{
		add_new_texture(Pic::portal);
	}
	void logic();
private:
	double timer_;
};



class Star : public Smart_Object
{
public:
	Star(double x, double y) :
		Smart_Object(x, y)
	{
		add_new_texture(Pic::star);
	}
	void logic();
};

class Teleporter : public Smart_Object
{
public:
	Teleporter(double x, double y) :
		Smart_Object(x, y),
		timer_(0),
		got_(false)
	{
		add_new_texture(Pic::teleporter);
		add_new_texture(Pic::t_key);
	}
	void logic();
private:
	double timer_;
	bool got_;
};

class Fake_Mushroom : public Smart_Object
{
public:
	Fake_Mushroom(double x, double y) :
		Smart_Object(x, y)
	{
		add_new_texture(Pic::fake_mushroom);
		add_new_texture(Pic::fake_mushroom_2);
	}
	void logic()
	{
		delete_if_in_disappear_box();
	}
	void delete_if_in_disappear_box();
private:
	const static Rect disappear_box;
};

class Blood : public Smart_Object
{
public:
	Blood(double x, double y) :
		Smart_Object(x, y)
	{

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
		Pic::Blood_1->init();
		the_pics_.push_back(Pic::Blood_1);
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
		Pic::Blood_2->init();
		the_pics_.push_back(Pic::Blood_2);
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
		Pic::Blood_3->init();
		the_pics_.push_back(Pic::Blood_3);
	}
};

class Dead_Kid_1 : public Blood
{
public:
	Dead_Kid_1(double x, double y, double velocity_x, double velocity_y) :
		Blood(x, y)
	{
		velocity_x_ = velocity_x;
		velocity_y_ = velocity_y;
		force_from_gravity_ = velocity_y;
		add_new_texture(Pic::Dead_Kid_1);
	}
};

class Dead_Kid_2 : public Blood
{
public:
	Dead_Kid_2(double x, double y, double velocity_x, double velocity_y) :
		Blood(x, y)
	{
		velocity_x_ = velocity_x;
		velocity_y_ = velocity_y;
		force_from_gravity_ = velocity_y;
		add_new_texture(Pic::Dead_Kid_2);
	}
};

class Dead_Kid_3 : public Blood
{
public:
	Dead_Kid_3(double x, double y, double velocity_x, double velocity_y) :
		Blood(x, y)
	{
		velocity_x_ = velocity_x;
		velocity_y_ = velocity_y;
		force_from_gravity_ = velocity_y;
		add_new_texture(Pic::Dead_Kid_3);
	}
};

void add_some_blood(int this_much, double x, double y);

class Chain : public Smart_Object
{
public:
	Chain(double x, double y, double distance, double speed, double radius);
	void logic()
	{
		timer_ += g_timer.deltaT_in_seconds() * speed_;
		location_.x = starting_location_.x + cos(timer_) * distance_;
		location_.y = starting_location_.y + sin(timer_) * distance_;
	}

private:
	double timer_;
	const double speed_;
	const Point starting_location_;
	const double distance_;
	const double radius_;
};

class Pendulum_Chain : public Smart_Object
{
public:
	Pendulum_Chain(double x, double y, double distance, double speed, double radius);

	void logic()
	{
		timer_ += g_timer.deltaT_in_seconds() * speed_;
		double time_modifier = ((sin(timer_)) * 1.57);
		double offset_x = sin(time_modifier) * distance_;
		location_.x = starting_location_.x + offset_x;
		double offset_y = sqrt((distance_ * distance_) - (offset_x * offset_x));
		location_.y = starting_location_.y + offset_y;
	}

private:
	double timer_;
	const double speed_;
	const Point starting_location_;
	const double distance_;
	const double radius_;
};

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

//These can only clip to the top/left side of a 16x16 tile.\
Collision detection with bullets and modification of tile behavior\
Will be work JUST FINE if you initialize it correctly. TRUUST me. INITIAL\
COLLISION DETECTION WITH THE KID WILL NOT BE SO ENTER IN '0' IN THE\
SCREEN TILE FILE FOR THE TILES YOU WANT TO PUT THE DOOR ON, CAPS BECAUSE\
IMPORTANT. Always 8x64 and can face the left or right direction. 
class Metroid_Door : public Smart_Object
{
public:
	Metroid_Door(double x, double y, SDL_RendererFlip direction);
	void logic();

private:
	//I'd make this const but I might want to add the ability to move these during runtime someday.\
	I probably won't though.
	Rect hit_box_;
	int HP_;
	int flash_time_left;
	//When your bullets hit the door, it will flash red for this many milliseconds.
	const static int FLASH_TIME = 50;
	const static int DOOR_WIDTH = 8;
	const static int DOOR_HEIGHT = 64;
	const int top_tile_x_;
	const int top_tile_y_;
	bool door_opening_;
};


void randomly_place_explosion_in_a_box(Rect box);
Texture* get_random_explosion();

#endif