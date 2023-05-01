#include "stdafx.h"
#include "Collision Information.h"
#include "The Kid.h"
#include <string>

Collision_Data::Collision_Data(string file_name)
{
	cout << "Collision_Info/" + file_name << " loaded" << endl;
	ifstream collision_file("Collision_Info/" + file_name);
	collision_file >> width_;
	collision_file >> height_;
	mid_point_x_ = width_ / 2;
	mid_point_y_ = height_ / 2;
	int shape_type = 0;
	while (!collision_file.eof())
	{
		collision_file >> shape_type;
		switch (shape_type)
		{
		case 0:
		{
			Line new_line;
			collision_file >> new_line.PointA.x;
			collision_file >> new_line.PointA.y;
			collision_file >> new_line.PointB.x;
			collision_file >> new_line.PointB.y;
			the_lines_.push_back(new_line);
			Line mirrored_line;
			mirrored_line.PointA.x = width_ - new_line.PointA.x;
			mirrored_line.PointA.y = new_line.PointA.y;
			mirrored_line.PointB.x = width_ - new_line.PointB.x;
			mirrored_line.PointB.y = new_line.PointB.y;
			the_lines_mirrored_.push_back(mirrored_line);
			break;
		}
		case 1:
		{
			Circle new_circle;
			collision_file >> new_circle.x;
			collision_file >> new_circle.y;
			collision_file >> new_circle.r;
			the_circles_.push_back(new_circle);
			Circle mirrored_circle;
			mirrored_circle.x = width_ - new_circle.x;
			mirrored_circle.y = new_circle.y;
			mirrored_circle.r = new_circle.r;
			the_circles_mirrored_.push_back(mirrored_circle);
			break;
		}
		case 2:
		{
			Rect new_rect;
			collision_file >> new_rect.x;
			collision_file >> new_rect.y;
			collision_file >> new_rect.w;
			collision_file >> new_rect.h;
			the_rects_.push_back(new_rect);
			Rect mirrored_rect;
			mirrored_rect.x = width_ - new_rect.x - new_rect.w;
			mirrored_rect.y = new_rect.y;
			mirrored_rect.w = new_rect.w;
			mirrored_rect.h = new_rect.h;
			the_rects_mirrored_.push_back(mirrored_rect);
			break;
		}
		}
	}
}

//2022 comment: Yikes. Thank god I don't write code like this anymore
bool Collision_Data::test_for_mirrored_collision(Rect hit_box, double x, double y, double angle, double width_ratio, double height_ratio)
{
	hit_box.x -= x;
	hit_box.y -= y;
	for (auto iter = the_lines_mirrored_.begin(); iter != the_lines_mirrored_.end(); ++iter)
	{
		Line modified_line;
		modified_line.PointA.x = (*iter).PointA.x * width_ratio - (width_ * (width_ratio - 1) / 2);
		modified_line.PointA.y = (*iter).PointA.y * height_ratio - (height_ * (height_ratio - 1) / 2);
		modified_line.PointB.x = (*iter).PointB.x * width_ratio - (width_ * (width_ratio - 1) / 2);
		modified_line.PointB.y = (*iter).PointB.y * height_ratio - (height_ * (height_ratio - 1) / 2);
		modified_line.PointA = turn_point_in_a_box(modified_line.PointA, mid_point_x_ * width_ratio - (width_ * (width_ratio - 1) / 2), mid_point_y_ * height_ratio - (height_ * (height_ratio - 1) / 2), angle);
		modified_line.PointB = turn_point_in_a_box(modified_line.PointB, mid_point_x_ * width_ratio - (width_ * (width_ratio - 1) / 2), mid_point_y_ * height_ratio - (height_ * (height_ratio - 1) / 2), angle);
		if (line_rect_collision(modified_line, hit_box))
			return true;
	}
	for (auto iter = the_circles_mirrored_.begin(); iter != the_circles_mirrored_.end(); ++iter)
	{
		Point new_circle_location = turn_point_in_a_box(Point((*iter).x * width_ratio, (*iter).y * height_ratio), mid_point_x_ * width_ratio, mid_point_y_ * height_ratio, angle);
		Circle new_circle;
		new_circle.x = new_circle_location.x - (width_ * (width_ratio - 1) / 2);
		new_circle.y = new_circle_location.y - (height_ * (height_ratio - 1) / 2);
		new_circle.r = (*iter).r * width_ratio;
		if (circle_rect_collision(new_circle, hit_box))
			return true;
	}
	for (auto iter = the_rects_mirrored_.begin(); iter != the_rects_mirrored_.end(); ++iter)
	{
		Rect new_hit_box = (*iter);
		new_hit_box.x *= width_ratio - (width_ * (width_ratio - 1) / 2);
		new_hit_box.y *= height_ratio - (height_ * (height_ratio - 1) / 2);
		new_hit_box.w *= width_ratio - (width_ * (width_ratio - 1) / 2);
		new_hit_box.h *= height_ratio - (height_ * (height_ratio - 1) / 2);
		if (rect_rect_collision((*iter), hit_box))
			return true;
	}
	return false;
}

bool Collision_Data::test_for_collision(Rect hit_box, double x, double y, double angle, double width_ratio, double height_ratio)
{
	hit_box.x -= x;
	hit_box.y -= y;
	for (auto iter = the_lines_.begin(); iter != the_lines_.end(); ++iter)
	{
		Line modified_line;
		modified_line.PointA.x = (*iter).PointA.x * width_ratio - (width_ * (width_ratio - 1) / 2);
		modified_line.PointA.y = (*iter).PointA.y * height_ratio - (height_ * (height_ratio - 1) / 2);
		modified_line.PointB.x = (*iter).PointB.x * width_ratio - (width_ * (width_ratio - 1) / 2);
		modified_line.PointB.y = (*iter).PointB.y * height_ratio - (height_ * (height_ratio - 1) / 2);
		modified_line.PointA = turn_point_in_a_box(modified_line.PointA, mid_point_x_ * width_ratio - (width_ * (width_ratio - 1) / 2), mid_point_y_ * height_ratio - (height_ * (height_ratio - 1) / 2), angle);
		modified_line.PointB = turn_point_in_a_box(modified_line.PointB, mid_point_x_ * width_ratio - (width_ * (width_ratio - 1) / 2), mid_point_y_ * height_ratio - (height_ * (height_ratio - 1) / 2), angle);
		if (line_rect_collision(modified_line, hit_box))
			return true;
	}
	for (auto iter = the_circles_.begin(); iter != the_circles_.end(); ++iter)
	{
		Point new_circle_location = turn_point_in_a_box(Point((*iter).x * width_ratio, (*iter).y * height_ratio), mid_point_x_ * width_ratio, mid_point_y_ * height_ratio, angle);
		Circle new_circle;
		new_circle.x = new_circle_location.x - (width_ * (width_ratio - 1) / 2);
		new_circle.y = new_circle_location.y - (height_ * (height_ratio - 1) / 2);
		new_circle.r = (*iter).r * width_ratio;
		if (circle_rect_collision(new_circle, hit_box))
			return true;
	}
	for (auto iter = the_rects_.begin(); iter != the_rects_.end(); ++iter)
	{
		Rect new_hit_box = (*iter);
		new_hit_box.x *= width_ratio - (width_ * (width_ratio - 1) / 2);
		new_hit_box.y *= height_ratio - (height_ * (height_ratio - 1) / 2);
		new_hit_box.w *= width_ratio - (width_ * (width_ratio - 1) / 2);
		new_hit_box.h *= height_ratio - (height_ * (height_ratio - 1) / 2);
		if (rect_rect_collision((*iter), hit_box))
			return true;
	}
	return false;
}

void Collision_Data::render(double location_x, double location_y, double angle, double width_ratio, double height_ratio)
{
	location_x -= Game::camera->x();
	location_y -= Game::camera->y();
	SDL_Rect outline;
	outline.x = static_cast<int>(location_x - (width_ * (width_ratio - 1) / 2));
	outline.y = static_cast<int>(location_y - (height_ * (height_ratio - 1) / 2));
	outline.w = static_cast<int>(width_ * width_ratio);
	outline.h = static_cast<int>(height_ * height_ratio);
	SDL_RenderDrawRect(g_renderer, &outline);
	for (auto iter = the_lines_.begin(); iter != the_lines_.end(); ++iter)
	{
		Line modified_line;
		modified_line.PointA.x = (*iter).PointA.x * width_ratio - (width_ * (width_ratio - 1) / 2);
		modified_line.PointA.y = (*iter).PointA.y * height_ratio - (height_ * (height_ratio - 1) / 2);
		modified_line.PointB.x = (*iter).PointB.x * width_ratio - (width_ * (width_ratio - 1) / 2);
		modified_line.PointB.y = (*iter).PointB.y * height_ratio - (height_ * (height_ratio - 1) / 2);
		modified_line.PointA = turn_point_in_a_box(modified_line.PointA, mid_point_x_ * width_ratio - (width_ * (width_ratio - 1) / 2), mid_point_y_ * height_ratio - (height_ * (height_ratio - 1) / 2), angle);
		modified_line.PointB = turn_point_in_a_box(modified_line.PointB, mid_point_x_ * width_ratio - (width_ * (width_ratio - 1) / 2), mid_point_y_ * height_ratio - (height_ * (height_ratio - 1) / 2), angle);
		SDL_RenderDrawLine(g_renderer, static_cast<int>(modified_line.PointA.x + location_x),
			static_cast<int>(modified_line.PointA.y + location_y),
			static_cast<int>(modified_line.PointB.x + location_x),
			static_cast<int>(modified_line.PointB.y + location_y));
	}
	for (auto iter = the_rects_.begin(); iter != the_rects_.end(); ++iter)
	{
		if (angle != 0)
			cout << "You're attempting to rotate a Rect. This should never happen. Fix this." << endl;
		SDL_Rect box;
		box.x = static_cast<int>((*iter).x * width_ratio + location_x - (width_ * (width_ratio - 1) / 2));
		box.y = static_cast<int>((*iter).y * height_ratio + location_y - (height_ * (height_ratio - 1) / 2));
		box.w = static_cast<int>((*iter).w * width_ratio);
		box.h = static_cast<int>((*iter).h * height_ratio);
		SDL_RenderDrawRect(g_renderer, &box);
	}
	for (auto iter = the_circles_.begin(); iter != the_circles_.end(); ++iter)
	{
		double draw_x = 0;
		double draw_y = 0;
		Point new_circle_location = turn_point_in_a_box(Point((*iter).x * width_ratio, (*iter).y * height_ratio), mid_point_x_ * width_ratio, mid_point_y_ * height_ratio, angle);
		Circle new_circle;
		new_circle.x = new_circle_location.x - (width_ * (width_ratio - 1) / 2);
		new_circle.y = new_circle_location.y - (height_ * (height_ratio - 1) / 2);
		new_circle.r = (*iter).r * width_ratio;
		for (double x = 0; x != 360; ++x)
		{
			draw_x = new_circle.x - new_circle.r + location_x + new_circle.r + cos(x) * new_circle.r;
			draw_y = new_circle.y - new_circle.r + location_y + new_circle.r + sin(x) * new_circle.r;
			SDL_RenderDrawPoint(g_renderer, static_cast<int>(draw_x), static_cast<int>(draw_y));
		}
		SDL_RenderDrawPoint(g_renderer, static_cast<int>(new_circle_location.x + location_x), static_cast<int>(new_circle_location.y + location_y));
		SDL_RenderDrawPoint(g_renderer, static_cast<int>(mid_point_x_ * width_ratio + location_x), static_cast<int>(mid_point_y_ * height_ratio + location_y));
	}
}

//copied from above and modified
void Collision_Data::render_mirrored(double location_x, double location_y, double angle, double width_ratio, double height_ratio)
{
	location_x -= Game::camera->x();
	location_y -= Game::camera->y();
	SDL_Rect outline;
	outline.x = static_cast<int>(location_x - (width_ * (width_ratio - 1) / 2));
	outline.y = static_cast<int>(location_y - (height_ * (height_ratio - 1) / 2));
	outline.w = static_cast<int>(width_ * width_ratio);
	outline.h = static_cast<int>(height_ * height_ratio);
	SDL_RenderDrawRect(g_renderer, &outline);

	for (auto iter = the_lines_mirrored_.begin(); iter != the_lines_mirrored_.end(); ++iter)
	{
		Line modified_line;
		modified_line.PointA.x = (*iter).PointA.x * width_ratio - (width_ * (width_ratio - 1) / 2);
		modified_line.PointA.y = (*iter).PointA.y * height_ratio - (height_ * (height_ratio - 1) / 2);
		modified_line.PointB.x = (*iter).PointB.x * width_ratio - (width_ * (width_ratio - 1) / 2);
		modified_line.PointB.y = (*iter).PointB.y * height_ratio - (height_ * (height_ratio - 1) / 2);
		modified_line.PointA = turn_point_in_a_box(modified_line.PointA, mid_point_x_ * width_ratio - (width_ * (width_ratio - 1) / 2), mid_point_y_ * height_ratio - (height_ * (height_ratio - 1) / 2), angle);
		modified_line.PointB = turn_point_in_a_box(modified_line.PointB, mid_point_x_ * width_ratio - (width_ * (width_ratio - 1) / 2), mid_point_y_ * height_ratio - (height_ * (height_ratio - 1) / 2), angle);
		SDL_RenderDrawLine(g_renderer, static_cast<int>(modified_line.PointA.x + location_x),
			static_cast<int>(modified_line.PointA.y + location_y),
			static_cast<int>(modified_line.PointB.x + location_x),
			static_cast<int>(modified_line.PointB.y + location_y));
	}
	for (auto iter = the_rects_mirrored_.begin(); iter != the_rects_mirrored_.end(); ++iter)
	{
		if (angle != 0)
			cout << "You're attempting to rotate a Rect. This should never happen. Fix this." << endl;
		SDL_Rect box;
		box.x = static_cast<int>((*iter).x * width_ratio + location_x - (width_ * (width_ratio - 1) / 2));
		box.y = static_cast<int>((*iter).y * height_ratio + location_y - (height_ * (height_ratio - 1) / 2));
		box.w = static_cast<int>((*iter).w * width_ratio);
		box.h = static_cast<int>((*iter).h * height_ratio);
		SDL_RenderDrawRect(g_renderer, &box);
	}
	for (auto iter = the_circles_mirrored_.begin(); iter != the_circles_mirrored_.end(); ++iter)
	{
		double draw_x = 0;
		double draw_y = 0;
		Point new_circle_location = turn_point_in_a_box(Point((*iter).x * width_ratio, (*iter).y * height_ratio), mid_point_x_ * width_ratio, mid_point_y_ * height_ratio, angle);
		Circle new_circle;
		new_circle.x = new_circle_location.x - (width_ * (width_ratio - 1) / 2);
		new_circle.y = new_circle_location.y - (height_ * (height_ratio - 1) / 2);
		new_circle.r = (*iter).r * width_ratio;
		for (double x = 0; x != 360; ++x)
		{
			draw_x = new_circle.x - new_circle.r + location_x + new_circle.r + cos(x) * new_circle.r;
			draw_y = new_circle.y - new_circle.r + location_y + new_circle.r + sin(x) * new_circle.r;
			SDL_RenderDrawPoint(g_renderer, static_cast<int>(draw_x), static_cast<int>(draw_y));
		}
		SDL_RenderDrawPoint(g_renderer, static_cast<int>(new_circle_location.x + location_x), static_cast<int>(new_circle_location.y + location_y));
		SDL_RenderDrawPoint(g_renderer, static_cast<int>(mid_point_x_ * width_ratio + location_x), static_cast<int>(mid_point_y_ * height_ratio + location_y));
	}
}

namespace Collision
{
	Collision_Data* no_collision = new Collision_Data("No Collision.txt");
	Collision_Data* spike = new Collision_Data("Spike Info.txt");
	Collision_Data* moon = new Collision_Data("Moon Info.txt");
	Collision_Data* spinning_saw = new Collision_Data("Spinning Saw Info.txt");
	Collision_Data* magic_projectile = new Collision_Data("Magic Projectile Info.txt");
	Collision_Data* frog_idle = new Collision_Data("Frog Idle Info.txt");
	Collision_Data* frog_jumping = new Collision_Data("Frog Jumping Info.txt");
	Collision_Data* carock_idle = new Collision_Data("Carock Idle Info.txt");
	Collision_Data* carock_casting = new Collision_Data("Carock Casting Info.txt");
	Collision_Data* lava = new Collision_Data("Lava Info.txt");
	Collision_Data* barry_idle = new Collision_Data("Barry Idle Info.txt");
	Collision_Data* barry_attack = new Collision_Data("Barry Attack Info.txt");
	Collision_Data* hadouken = new Collision_Data("Hadouken Info.txt");
	Collision_Data* fireball = new Collision_Data("Fireball Info.txt");
	Collision_Data* thunderbolt = new Collision_Data("Thunderbolt Info.txt");
	Collision_Data* thrown_rock = new Collision_Data("Thrown Rock Info.txt");
	Collision_Data* windmill = new Collision_Data("Windmill Info.txt");
	Collision_Data* pounder = new Collision_Data("Pounder Info.txt");
	Collision_Data* pounder_top_fist = new Collision_Data("Pounder Top Fist Info.txt");
	Collision_Data* pounder_side_fist = new Collision_Data("Pounder Side Fist Info.txt");
	Collision_Data* shockwave = new Collision_Data("Shockwave Info.txt");
	Collision_Data* spike_wall = new Collision_Data("Spike Wall Info.txt");
	Collision_Data* missile = new Collision_Data("Missile Info.txt");
	Collision_Data* falling_spike = new Collision_Data("Falling Spike Info.txt");
	Collision_Data* crushing_spikes = new Collision_Data("Crushing Spikes Info.txt");
	Collision_Data* chasing_spikes = new Collision_Data("Chasing Spikes Info.txt");
	Collision_Data* rising_spikes = new Collision_Data("Rising Spikes Info.txt");
}