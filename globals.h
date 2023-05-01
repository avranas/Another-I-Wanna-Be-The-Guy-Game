#ifndef GLOBALS_H_
#define GLOBALS_H_
#pragma warning(disable:4010)
#include <SDL.h>
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <list>
#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include "GameState.h"
#include "timer.h"

using namespace std;

//Toggle this on to enable debug mode
extern const bool g_debug_mode_on;

//Will be used all over the place for SDL_RenderCopy
extern SDL_Window* g_window;
//Will be used all over the place for SDL_RenderCopy
extern SDL_Renderer* g_renderer;
//Event handling will be done in multiple places
extern SDL_Event g_event;
//We will need to switch the next game state in multiple places
extern GameStateType g_next_game_state;
//Game controller
//extern SDL_Joystick* gGameController;
//We may need to exit the game at any time
extern bool g_main_loop_is_running;
//The whole program will need the screen data
extern int g_screen_width;
extern int g_screen_height;
//I might need these, I might not. I probably won't.
extern const int g_base_screen_width;
extern const int g_base_screen_height;
//I will need this:
extern SDL_Rect g_viewport;
extern double g_height_modifier;
extern double g_width_modifier;
extern bool g_window_is_minimized;
extern double g_FPS;
//One timer for everything
extern Timer g_timer;
extern const double g_pi;
//Or this.
extern const double g_sqrt2over2;
//default drawing colors
extern int g_default_red;
extern int g_default_blue;
extern int g_default_green;
extern int g_default_alpha;
extern const double g_gravity;
extern const double g_terminal_velocity;

void change_screen_height(int value);
void change_screen_width(int value);

//Maybe I'll want to change this later
const static double G_TILE_WIDTH = 16;
const static double G_TILE_HEIGHT = 16;
const static int TILES_PER_SCREEN_X = 64;
const static int TILES_PER_SCREEN_Y = 36;

inline SDL_RendererFlip bool_to_SDL_RendererFlip(bool flip)
{
	if (flip)
		return SDL_FLIP_HORIZONTAL;
	else
		return SDL_FLIP_NONE;
}

//2022 comment - Why did I not put these in their own file?? /facepalm
inline int round_to_nearest(double value)
{
	return static_cast<int>(floor(value + 0.5));
}

inline bool collide_two_sdl_rects(const SDL_Rect& lhs, const SDL_Rect& rhs);
bool rect_rect_collision(const Rect& lhs, const Rect& rhs);
inline double distance_between_points(const Point& lhs, const Point& rhs);
double calculate_angle_between_points_in_radians(const Point& lhs, const Point& rhs);
double calculate_angle_between_points_in_degrees(const Point& lhs, const Point& rhs);
inline bool point_circle_collision(const Circle& theCircle, const Point* thePoint);
inline bool point_point_collision(const Point* lhs, const Point* rhs);
inline bool rect_point_collision(const Rect& theRect, const Point& thePoint);
inline bool circle_circle_collision(const Circle& lhs, const Circle& rhs);
bool circle_rect_collision(const Circle& theCircle, const Rect& theRect);
bool line_line_collision(const Line& lhs, const Line& rhs);
bool line_rect_collision(const Line& the_line, const Rect& the_rect);
//Tells you how much needs to pass in one sprite's object based on its FPS\
(This assumes that its less than the global FPS, which is set to 60, and can be changed in Globals.cpp)
int time_per_frame(int FPS);
inline double center_of_screen_x() { return static_cast<double>(g_screen_width) / 2; }
inline double center_of_screen_y() { return static_cast<double>(g_screen_height) / 2; }
void modify_velocity_with_gravity(double* velocity, double* force_from_gravity);
void modify_velocity_with_gravity_and_limit(double* velocity, double* force_from_gravity);
void modify_velocity_with_force(double* velocity, double* force_from_gravity, double force);
void modify_velocity_without_gravity(double* velocity, double* force_from_gravity);
bool did_it_pass(double old_value, double new_value, const double magic_number);
bool did_it_pass_neg(double old_value, double new_value, const double magic_number);
Point turn_point_in_a_box(const Point& movingPoint, const double width, const double height, const double angle);

inline string take_ms_return_english(long value)
{
	//Removed milliseconds. Figured it was too much
	//long milliseconds = value % 1000;
	long seconds = (value / 1000);
	long minutes = seconds / 60;
	long hours = minutes / 60;
	string new_string;
	if (hours != 0)
	{
		seconds -= 60 * minutes;
		minutes -= 60 * hours;
		new_string += to_string(hours);
		new_string += ":";
		if (minutes < 10)
			new_string += "0";
	}
	if (hours == 0)
		seconds -= 60 * minutes;
	new_string += to_string(minutes);
	new_string += ":";
	if (seconds < 10)
		new_string += "0";

	new_string += to_string(seconds);
	//new_string += ".";
	//new_string += to_string(milliseconds);
	return new_string;
}

inline double sin_degrees(double value)
{
	return sin(value * g_pi / 180);
}
inline double cos_degrees(double value)
{
	return cos(value * g_pi / 180);
}inline double tan_degrees(double value)
{
	return tan(value * g_pi / 180);
}

inline void pos_x_in_world(double* pos_x, int screen_x)
{
	*pos_x = *pos_x + static_cast<double>(screen_x * g_screen_width);
}

inline void pos_y_in_world(double* pos_y, int screen_y)
{
	*pos_y = *pos_y + static_cast<double>(screen_y * g_screen_height);
}

inline Point get_rand_point_in_box(const Rect& box)
{
	Point new_point;
	new_point.x = rand() % static_cast<int>(box.w) + box.x;
	new_point.y = rand() % static_cast<int>(box.h) + box.y;
	return new_point;
}

//Uses the global timer to keep track of how much time has passed since initialization
//Set the time to '-1' to allow the timer to last forever.
class Countdown
{
public:
	Countdown(double how_long) :
		time_left_(how_long)
	{}

	//Call this once per loop. Returns true if time is up.
	inline bool countdown_logic()
	{
		if (time_left_ == -1)
			return false;
		time_left_ -= g_timer.deltaT_in_seconds();
		return time_left_ <= 0;
	}

	inline double time_left()
	{
		return time_left_;
	}

private:
	double time_left_;
};

class Frequency_Timer
{
public:
	Frequency_Timer(double how_often) :
		timer_(0),
		max_time_(how_often),
		recent_cycle_(false)
	{}

	//Call this once per loop. Returns true if time
	inline bool frequency_timer_logic()
	{
		timer_ += g_timer.deltaT_in_seconds();
		if (timer_ >= max_time_)
		{
			timer_ -= max_time_;
			recent_cycle_ = true;
			return true;
		}
		recent_cycle_ = false;
		return false;
	}

	bool recent_cycle()
	{
		return recent_cycle_;
	}

	double time_spent()
	{
		return timer_;
	}

	double time_left()
	{
		return max_time_ - timer_;
	}

	void change_max_time(double value)
	{
		max_time_ = value;
	}

private:
	double timer_;
	double max_time_;
	bool recent_cycle_;
};

//An angle in degrees. Zero degrees points straight to the right.
class Angle
{
public:
	Angle(double angle) :
		angle_(angle)
	{

	}

	void increase_by(double value)
	{
		angle_ += value;
		int cycles = static_cast<int>(angle_) / 360;
		angle_ -= cycles * 360;
		if (angle_ < 0)
			angle_ = 360 + angle_;
	}

	void decrease_by(double value)
	{
		angle_ -= value;
		int cycles = static_cast<int>(angle_) / 360;
		angle_ += cycles * 360;
		if (angle_ < 0)
			angle_ = 360 + angle_;
	}

	void set_angle(double value)
	{
		angle_ = value;
		int cycles = static_cast<int>(angle_) / 360;
		if (cycles > 0)
			angle_ -= cycles * 360;
		else if (cycles < 0)
			angle_ += cycles * 360;
		else if (angle_ < 0)
			angle_ = 360 + angle_;
	}

	double angle()
	{
		return angle_;
	}

private:
	double angle_;
};


class Vector
{
public:
	//Angle in degrees!
	Vector(double magnitude, double angle) :
		magnitude_(magnitude),
		angle_(angle)
	{}

	double magnitude() { return magnitude_; }

	inline void set_angle(double value)
	{
		angle_.set_angle(value);
	}

	double magnitude_x()
	{
		return magnitude_ * cos_degrees(angle_.angle());
	}

	double magnitude_y()
	{
		return magnitude_ * sin_degrees(angle_.angle());
	}

	void decrease_magnitude(double value)
	{
		magnitude_ -= value;
	}

	void decrease_magnitude_with_zero_minimum(double value)
	{
		magnitude_ -= value;
		if (magnitude_ < 0)
			magnitude_ = 0;
	}

private:
	double magnitude_;
	Angle angle_;
};

#endif