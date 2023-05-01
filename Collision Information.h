#ifndef COLLISION_DATA_H_
#define COLLISION_DATA_H_
#include <SDL.h>
#include "globals.h"
#include <vector>
#include <fstream>

class Collision_Data
{
public:
	Collision_Data(string file_name);
	bool test_for_collision(Rect hit_box, double x, double y, double angle, double width_ratio, double height_ratio);
	bool test_for_mirrored_collision(Rect hit_box, double x, double y, double angle, double width_ratio, double height_ratio);
	void render(double location_x, double location_y, double angle, double width_ratio, double height_ratio);
	void render_mirrored(double location_x, double location_y, double angle, double width_ratio, double height_ratio);
	double mid_point_x() { return mid_point_x_; }
	double mid_point_y() { return mid_point_y_; }

private:
	vector<Line> the_lines_;
	vector<Line> the_lines_mirrored_;
	vector<Circle> the_circles_;
	vector<Circle> the_circles_mirrored_;
	//Remember: rects don't support rotating. Never rotate rects.
	vector<Rect> the_rects_mirrored_;
	vector<Rect> the_rects_;
	double mid_point_x_;
	double mid_point_y_;
	double width_;
	double height_;
};

namespace Collision
{
	extern Collision_Data* no_collision;
	extern Collision_Data* spike;
	extern Collision_Data* moon;
	extern Collision_Data* spinning_saw;
	extern Collision_Data* magic_projectile;
	extern Collision_Data* frog_idle;
	extern Collision_Data* frog_jumping;
	extern Collision_Data* carock_idle;
	extern Collision_Data* carock_casting;
	extern Collision_Data* lava;
	extern Collision_Data* barry_idle;
	extern Collision_Data* barry_attack;
	extern Collision_Data* hadouken;
	extern Collision_Data* fireball;
	extern Collision_Data* thunderbolt;
	extern Collision_Data* thrown_rock;
	extern Collision_Data* windmill;
	extern Collision_Data* pounder;
	extern Collision_Data* pounder_top_fist;
	extern Collision_Data* pounder_side_fist;
	extern Collision_Data* shockwave;
	extern Collision_Data* spike_wall;
	extern Collision_Data* missile;
	extern Collision_Data* falling_spike;
	extern Collision_Data* crushing_spikes;
	extern Collision_Data* chasing_spikes;
	extern Collision_Data* rising_spikes;
}

#endif
