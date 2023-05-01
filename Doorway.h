#ifndef DOORWAY_H_
#define DOORWAY_H_
#include <SDL.h>
#include "Image Processing.h"
#include "timer.h"
#include "globals.h"
class Doorway : public Displayable_Object
{
public:
	Doorway(double x, double y, string file_name) :
		Displayable_Object(x, y)
	{
		Pic::doorway_idle->init();
		the_pics_.push_back(Pic::doorway_idle);
		ifstream doorway_data(file_name);
		doorway_data >> goes_to_world_;
		doorway_data >> goes_to_screen_x_;
		doorway_data >> goes_to_screen_y_;
		doorway_data >> goes_to_position_x_;
		doorway_data >> goes_to_position_y_;
	}

	bool are_we_lined_up(const Rect& hit_box)
	{
		return rect_rect_collision(hit_box, Rect(location_.x, location_.y, WIDTH, HEIGHT));
	}

	inline int goes_to_world()
	{
		return goes_to_world_;
	}

	inline int goes_to_screen_x()
	{
		return goes_to_screen_x_;
	}

	inline int goes_to_screen_y()
	{
		return goes_to_screen_y_;
	}

	inline double goes_to_position_x()
	{
		return goes_to_position_x_;
	}

	inline double goes_to_position_y()
	{
		return goes_to_position_y_;
	}

private:
	const static double WIDTH;
	const static double HEIGHT;
	int goes_to_world_;
	int goes_to_screen_x_;
	int goes_to_screen_y_;
	double goes_to_position_x_;
	double goes_to_position_y_;
};

//When the kid collides with the activation_box_, he teleports to a new location.
class Magic_Teleport_Box
{
public:
	Magic_Teleport_Box(Rect activation_box, int goes_to_world, int goes_to_screen_x,
		int goes_to_screen_y, double goes_to_position_x, double goes_to_position_y) :
		activation_box_(activation_box),
		goes_to_world_(goes_to_world),
		goes_to_screen_x_(goes_to_screen_x),
		goes_to_screen_y_(goes_to_screen_y),
		goes_to_position_x_(goes_to_position_x),
		goes_to_position_y_(goes_to_position_y)
	{}

	inline bool overlaps(Rect* kid_box)
	{
		return rect_rect_collision(activation_box_, *kid_box);
	}

	inline int goes_to_world()
	{
		return goes_to_world_;
	}

	inline int goes_to_screen_x()
	{
		return goes_to_screen_x_;
	}

	inline int goes_to_screen_y()
	{
		return goes_to_screen_y_;
	}

	inline double goes_to_position_x()
	{
		return goes_to_position_x_;
	}

	inline double goes_to_position_y()
	{
		return goes_to_position_y_;
	}

private:
	Rect activation_box_;
	int goes_to_world_;
	int goes_to_screen_x_;
	int goes_to_screen_y_;
	double goes_to_position_x_;
	double goes_to_position_y_;
};

#endif
