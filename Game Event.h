#ifndef GAME_EVENT_H_
#define GAME_EVENT_H_
#include <SDL.h>
#include "globals.h"
#include "Image Processing.h"
#include "Enemy.h"

//A game event is to trigger some actions (like spawn monsters, open/close,\
doors, whatever) whenever the kid collides with a box. Maybe other things can trigger\
events?
class Game_Event
{
public:
	Game_Event(double x, double y, double w, double h) :
		event_box_(Rect(x, y, w, h)),
		activated_(false),
		done_(false)
	{}

	virtual void logic() = 0;
	virtual ~Game_Event() {}
	void activate_if_ready();

	void remove_event()
	{
		done_ = true;
	}

	void reset_event()
	{
		activated_ = false;
	}

	//return true if activated is true and done is false
	inline bool activated()
	{
		return activated_ && done_ == false;
	}

	bool done()
	{
		if (done_)
			delete this;
		return done_;
	}

protected:
	//When The Kid laps over this rect, the Event begins
	Rect event_box_;
	bool activated_;
	bool done_;
};

class First_Event : public Game_Event
{
public:
	First_Event() :
		Game_Event(160, 352, 32, 200),
		timer_(0),
		the_state_(Pause_Character_Movement),
		current_spawn_point(0)
	{
		for (int x = 0; x != NUMBER_OF_ENEMIES; ++x)
		{
			spawn_times[x] = 400 * x + 1;
		}

	}

	void logic();
private:
	enum First_Event_Steps
	{
		Pause_Character_Movement,
		Close_Doors,
		Spawn_Some_Stuff,
		Wait,
		Open_Doors
	};

	const static int NUMBER_OF_ENEMIES = 32;
	double spawn_times[NUMBER_OF_ENEMIES];
	Enemy* event_enemies[NUMBER_OF_ENEMIES];
	const static double PAUSE_CHARACTER_TIME;
	const static double SPAWN_LOCATION_A_X;
	const static double SPAWN_LOCATION_Y;
	const static double SPAWN_LOCATION_B_X;
	const static double SPAWN_LOCATION_C_X;
	const static double SPAWN_LOCATION_D_X;
	First_Event_Steps the_state_;
	double timer_;
	int current_spawn_point;
};

class Yellow_Switch_Event : public Game_Event
{
public:
	Yellow_Switch_Event() :
		Game_Event(0, 0, 0, 0),
		current_position_(0),
		timer_(0)
	{
		int counter = 0;
		int current_time = 1;
		int current_position_x = 26;
		int current_position_y = 32;
		for (current_position_x = 26; current_position_x != 40; current_position_x++)
		{
			counter_x[counter] = current_position_x;
			counter_y[counter] = current_position_y;
			time[counter] = current_time;
			counter++;
			current_time += 500;
		}
		current_position_y--;
		current_time += 1500;
		for (current_position_x = 30; current_position_x != 36; current_position_x++)
		{
			counter_x[counter] = current_position_x;
			counter_y[counter] = current_position_y;
			time[counter] = current_time;
			counter++;
			current_time += 2000;
		}
		current_time += 5000;
		current_position_y--;
		for (current_position_x = 32; current_position_x != 34; current_position_x++)
		{
			counter_x[counter] = current_position_x;
			counter_y[counter] = current_position_y;
			time[counter] = current_time;
			counter++;
			current_time += 100;
		}
		activated_ = true;
	}

	void logic();

private:
	int timer_;
	int counter_x[22];
	int counter_y[22];
	int time[22];
	int current_position_;
};

class Moon_Spawner : public Game_Event
{
public:
	Moon_Spawner();
	void logic();
private:
	const static double SPAWN_FREQUENCY;
	double timer_;
};


class Barry_Spawn_Event : public Game_Event
{
public:
	Barry_Spawn_Event() :
		Game_Event(0, 480, 1260, 48)
	{}

	void logic();

private:
};

class Left_Spring_Pressed_In_Barry_Fight : public Game_Event
{
public:
	Left_Spring_Pressed_In_Barry_Fight() :
		Game_Event(48, 511, 32, 16),
		timer_(0)
	{}

	void logic();

private:
	const static double SPEED;
	double timer_;
};

class Right_Spring_Pressed_In_Barry_Fight : public Game_Event
{
public:
	Right_Spring_Pressed_In_Barry_Fight() :
		Game_Event(944, 511, 32, 16),
		timer_(0)
	{}

	void logic();

private:
	const static double SPEED;
	double timer_;
};

class Barry_Dead_Event : public Game_Event
{
public:
	Barry_Dead_Event() :
		Game_Event(0, 0, 0, 0),
		timer_(0)
	{
		activated_ = true;
	}
	void logic();
private:
	double timer_;
};

class Big_Explosion_Event : public Game_Event
{
public:
	Big_Explosion_Event(Rect box, int number_of_explosions, int frequency) :
		Game_Event(0, 0, 0, 0),
		box_(box),
		timer_(0),
		number_of_explosions_left_(number_of_explosions),
		frequency_(frequency)
	{
		activated_ = true;
	}

	void logic();

private:
	Rect box_;
	int timer_;
	int number_of_explosions_left_;
	int frequency_;
};

class Destroy_Bridge_Event : public Game_Event
{
public:
	Destroy_Bridge_Event(bool left);
	void logic();

private:
	int timer_;
	const int frequency_;
	int position_;
	bool left_;
};

class Frog_Event : public Game_Event
{
public:
	Frog_Event() :
		Game_Event(784, 16, 32, 128),
		timer_(0),
		the_state_(Pause_Character_Movement),
		current_spawn_point(0)
	{
		for (int x = 0; x != NUMBER_OF_ENEMIES; ++x)
		{
			spawn_times[x] = 400 * x + 1;
		}

	}

	void logic();

private:
	enum Frog_Event_Steps
	{
		Pause_Character_Movement,
		Close_Doors,
		Spawn_Some_Stuff,
		Wait,
		Open_Doors
	};

	const static int NUMBER_OF_ENEMIES = 32;
	double spawn_times[NUMBER_OF_ENEMIES];
	Enemy* event_enemies[NUMBER_OF_ENEMIES];
	const static double PAUSE_CHARACTER_TIME;
	const static double SPAWN_LOCATION_A_X;
	const static double SPAWN_LOCATION_Y;
	const static double SPAWN_LOCATION_B_X;
	const static double SPAWN_LOCATION_C_X;
	const static double SPAWN_LOCATION_D_X;
	Frog_Event_Steps the_state_;
	double timer_;
	int current_spawn_point;
};

class Create_A_Door : public Game_Event
{
public:
	Create_A_Door(int start_x, int start_y, int how_many, int frequency) :
		Game_Event(0, 0, 0, 0),
		spot_x_(start_x),
		spot_y_(start_y),
		how_many_(how_many),
		frequency_(frequency),
		timer_(0)
	{
		activated_ = true;
	}

	void logic();

private:
	int spot_x_;
	int spot_y_;
	int how_many_;
	int frequency_;
	double timer_;
};

//Destroys a line of tiles in the y direction going from bottom to top
//It works the same way as Create_A_Door, except destroys the tiles
class Close_A_Door : public Game_Event
{
public:
	//Put in the tile where the top of the door is. The same one you used
	//to create a door.
	Close_A_Door(int start_x, int start_y, int how_many, int frequency, char tile_id) :
		Game_Event(0, 0, 0, 0),
		spot_x_(start_x),
		spot_y_(start_y),
		how_many_(how_many),
		frequency_(frequency),
		timer_(0),
		tile_id_(tile_id)
	{
		activated_ = true;
	}

	void logic();

private:
	int spot_x_;
	int spot_y_;
	int how_many_;
	int frequency_;
	double timer_;
	char tile_id_;
};

class Moving_Spikes_Screen : public Game_Event
{
public:
	//Put in the tile where the top of the door is. The same one you used
	//to create a door
	Moving_Spikes_Screen() :
		Game_Event(0, 0, 0, 0)
	{
		activated_ = true;
		entry_pos_x_[0] = 4832;
		entry_frequency_[0] = 1400;
		entry_timer_[0] = 0;
		new_velocity[0] = 110;
		entry_pos_y_[0] = -96;
		entry_pos_x_[1] = 4896;
		entry_frequency_[1] = 1400;
		entry_timer_[1] = 0;
		new_velocity[1] = -110;
		entry_pos_y_[1] = 672;
	}

	void logic();

private:
	int entry_timer_[2];
	int entry_frequency_[2];
	int entry_pos_x_[2];
	int entry_pos_y_[2];
	int new_velocity[2];
};

class Falling_Spikes_Screen_1 : public Game_Event
{
public:
	Falling_Spikes_Screen_1() :
		Game_Event(5472, 48, 96, 500),
		entry_frequency_(500),
		timer_(0)
	{
		for (int x = 0; x != 4; x++)
		{
			entry_pos_x_[x] = 32 * x + 5472;
		}
	}

	void logic();

private:
	const int entry_frequency_;
	int timer_;
	int entry_pos_x_[4];
};

class Falling_Spikes_Screen_2 : public Game_Event
{
public:
	//Put in the tile where the top of the door is. The same one you used
	//to create a door.
	Falling_Spikes_Screen_2() :
		Game_Event(6208, 0, 100, 384)
	{
		for (int x = 0; x != 5; x++)
		{
			entry_pos_x_[x] = 160 * (x + 1) + 6198;
			entry_frequency_[x] = 350 - (x * 45);
			entry_timer_[x] = 0;
		}
	}

	void logic();

private:
	int entry_timer_[5];
	int entry_frequency_[5];
	int entry_pos_x_[5];
};
//Controls the trap in World 1 screen 4. I need to do this so blood will stick to\
the tiles and not the spikes, so nothing seems out of the ordinary.
class Troll_Spike_Platform : public Game_Event
{
public:
	Troll_Spike_Platform() :
		Game_Event(0, 0, 0, 0),
		activation_box_(Rect(5600, 0, 96, 1000))
	{
		activated_ = true;
	}

private:
	void logic();
	const Rect activation_box_;
};

class End_Game_Action : public Game_Event
{
public:
	End_Game_Action();
	void logic();
private:
	Countdown i_won_;
	Frequency_Timer fireworks_frequency_;
	bool i_won_done_;
	const int firework_entry_left_x;
	const int firework_entry_right_x;
};

#endif
