#include "stdafx.h"
#include "globals.h"
#include <iostream>

//2022 comment: This solved a mysterious linker error - Thanks StackOverflow!
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

SDL_Window* g_window;
SDL_Renderer* g_renderer;
SDL_Event g_event;
//SDL_Joystick* gGameController;
GameStateType g_next_game_state = StateMainGame;
//There is no reason this should be defined differently here...
bool g_main_loop_is_running = true;
int g_screen_width = 1024; //1024
int g_screen_height = 576; //576
const int g_base_screen_width = g_screen_width;
const int g_base_screen_height = g_screen_height;
double g_height_modifier = (double)g_screen_height / (double)g_base_screen_height;
double g_width_modifier = (double)g_screen_height / (double)g_base_screen_height;
SDL_Rect g_viewport;
bool g_window_is_minimized = false;
double g_FPS = 60.0f;
Timer g_timer;
const double g_pi = 3.14159265f;
const double g_sqrt2over2 = 0.70711f;

int g_default_red = 0x00;
int g_default_blue = 0x00;
int g_default_green = 0x00;
int g_default_alpha = 0x00;

const double g_gravity = 1090;
const double g_terminal_velocity = 390;

//Toggle this to true to enable debug mode	
const bool g_debug_mode_on = false;

//SDL_RendererFlip bool_to_SDL_RendererFlip(bool flip);

void change_screen_height(int value)
{
	//g_screen_height = value;
	//g_height_modifier = static_cast<double>(value) / static_cast<double>(g_base_screen_height);
	SDL_RenderSetLogicalSize(g_renderer, g_base_screen_width, g_base_screen_height);
}

void change_screen_width(int value)
{
	//g_screen_width = value;
	//g_width_modifier = static_cast<double>(value) / static_cast<double>(g_base_screen_width);
	SDL_RenderSetLogicalSize(g_renderer, g_base_screen_width, g_base_screen_height);
}

//2022 comment: Why why whyyy are these not in their own file??

bool rect_rect_collision(const Rect& lhs, const Rect& rhs)
{
	if (lhs.y + lhs.h <= rhs.y)
		return false;
	if (lhs.y >= rhs.y + rhs.h)
		return false;
	if (lhs.x + lhs.w <= rhs.x)
		return false;
	if (lhs.x >= rhs.x + rhs.w)
		return false;
	return true;
}

bool collide_two_sdl_rects(const SDL_Rect& lhs, const SDL_Rect& rhs)
{
	if (lhs.y + lhs.h <= rhs.y)
		return false;
	if (lhs.y >= rhs.y + rhs.h)
		return false;
	if (lhs.x + lhs.w <= rhs.x)
		return false;
	if (lhs.x >= rhs.x + rhs.w)
		return false;
	return true;
}

int time_per_frame(int FPS)
{
	return 1000 / FPS;
}

//lhs is the starting point
double calculate_angle_between_points_in_degrees(const Point& lhs, const Point& rhs)
{
	double deltaX = rhs.x - lhs.x;
	double deltaY = rhs.y - lhs.y;
	//This is to eliminate problems with dividing by zero.
	if (deltaX == 0)
		deltaX = 0.00001f;
	double angle = (((atan(deltaY / deltaX)) * 180 / g_pi));
	//If the cursor is behind the character add 180
	if (lhs.x > rhs.x)
		angle += 180;
	return angle;
}

double calculate_angle_between_points_in_radians(const Point& lhs, const Point& rhs)
{
	double deltaX = rhs.x - lhs.x;
	double deltaY = rhs.y - lhs.y;
	//This is to eliminate problems with dividing by zero.
	if (deltaX == 0)
		deltaX = 0.00001f;
	double angle = (atan(deltaY / deltaX));
	//If the cursor is behind the character add 180
	if (lhs.x > rhs.x)
		angle += g_pi;
	//angle += (g_pi / 2);
	return angle;
}

//Results will be positive
double distance_between_points(const Point& lhs, const Point& rhs)
{
	double x_distance = abs(rhs.x - lhs.x);
	double y_distance = abs(rhs.y - lhs.y);
	//cout << y_distance << ';' << x_distance << endl;

	return sqrt((x_distance * x_distance) + (y_distance * y_distance));
}

bool point_circle_collision(const Circle* theCircle, const Point* thePoint)
{
	double xDistance = theCircle->x - thePoint->x;
	double yDistance = theCircle->y - thePoint->y;
	double vDistance = sqrt((xDistance * xDistance) + (yDistance * yDistance));
	if (vDistance < theCircle->r)
		return true;
	return false;
}

bool point_point_collision(const Point* lhs, const Point* rhs)
{
	if (lhs->x == rhs->x && rhs->y == lhs->y)
		return true;
	return false;
}

bool Rect_point_collision(const Rect& theRect, const Point& thePoint)
{
	if (thePoint.x >= theRect.x && thePoint.x <= theRect.x + theRect.w &&
		thePoint.y >= theRect.y && thePoint.y <= theRect.y + theRect.h)
		return true;
	return false;
}

bool circle_circle_collision(const Circle& lhs, const Circle& rhs)
{
	double xDistance = lhs.x - rhs.x;
	double yDistance = lhs.y - rhs.y;
	double vDistance = sqrt((xDistance * xDistance) + (yDistance * yDistance));
	if (vDistance < lhs.r + rhs.r)
		return true;
	return false;
}

bool circle_rect_collision(const Circle& theCircle, const Rect& theRect)
{
	double yDifference = 0;
	if ((theRect.y + theRect.h) < theCircle.y)
		yDifference = theCircle.y - (theRect.y + theRect.h);
	else if (theRect.y > theCircle.y)
		yDifference = theCircle.y - theRect.y;
	double xDifference = sqrt(theCircle.r * theCircle.r - yDifference * yDifference);
	if (theRect.x > theCircle.x)
	{
		xDifference *= -1;
		if (theRect.x < theCircle.x - xDifference)
			return true;
		return false;
	}
	if (theRect.x + theRect.w > theCircle.x - xDifference)
		return true;
	return false;
}

bool line_line_collision(const Line& lhs, const Line& rhs)
{
	double lhsSlope;
	double rhsSlope;
	if (lhs.PointA.x == lhs.PointB.x)
		lhsSlope = numeric_limits<double>::infinity();
	else if (lhs.PointA.y == lhs.PointB.y)
		lhsSlope = 0;
	else
		lhsSlope = (double)(lhs.PointB.y - lhs.PointA.y) / (lhs.PointB.x - lhs.PointA.x);
	if (rhs.PointA.x == rhs.PointB.x)
		rhsSlope = numeric_limits<double>::infinity();
	else if (rhs.PointA.y == rhs.PointB.y)
		rhsSlope = 0;
	else
		rhsSlope = (double)(rhs.PointB.y - rhs.PointA.y) / (rhs.PointB.x - rhs.PointA.x);
	double PoIx;
	double PoIy;
	double rhsYint = rhs.PointA.y - rhsSlope * rhs.PointA.x;	//Choosing PointA or PointB doesn't make a difference
	double lhsYint = lhs.PointA.y - lhsSlope * lhs.PointA.x;
	if (lhsSlope == rhsSlope && lhsYint == rhsYint)
	{
		if (((lhs.PointB.x >= rhs.PointA.x && lhs.PointB.x <= rhs.PointB.x) || (lhs.PointA.x >= rhs.PointA.x && lhs.PointA.x <= rhs.PointB.x))
			&& (((lhs.PointB.y >= rhs.PointA.y && lhs.PointB.y <= rhs.PointB.y) || (lhs.PointA.y >= rhs.PointA.y && lhs.PointA.y <= rhs.PointB.y))))
			return true;
		if (((lhs.PointA.x >= rhs.PointB.x && lhs.PointA.x <= rhs.PointA.x) || (lhs.PointB.x >= rhs.PointB.x && lhs.PointB.x <= rhs.PointA.x))
			&& (((lhs.PointA.y >= rhs.PointB.y && lhs.PointA.y <= rhs.PointA.y) || (lhs.PointB.y >= rhs.PointB.y && lhs.PointB.y <= rhs.PointA.y))))
			return true;
		if (((rhs.PointB.x >= lhs.PointA.x && rhs.PointB.x <= lhs.PointB.x) || (rhs.PointA.x >= lhs.PointA.x && rhs.PointA.x <= lhs.PointB.x))
			&& (((rhs.PointB.y >= lhs.PointA.y && rhs.PointB.y <= lhs.PointB.y) || (rhs.PointA.y >= lhs.PointA.y && rhs.PointA.y <= lhs.PointB.y))))
			return true;
		if (((rhs.PointA.x >= lhs.PointB.x && rhs.PointA.x <= lhs.PointA.x) || (rhs.PointB.x >= lhs.PointB.x && rhs.PointB.x <= lhs.PointA.x))
			&& (((rhs.PointA.y >= lhs.PointB.y && rhs.PointA.y <= lhs.PointA.y) || (rhs.PointB.y >= lhs.PointB.y && rhs.PointB.y <= lhs.PointA.y))))
			return true;
		return false;
	}
	else
	{
		if (lhsSlope == numeric_limits<double>::infinity())
		{
			PoIx = lhs.PointA.x;
			PoIy = (PoIx * rhsSlope) + (rhs.PointA.y - rhsSlope * rhs.PointA.x);
		}
		else if (rhsSlope == numeric_limits<double>::infinity())
		{
			PoIx = rhs.PointA.x;
			PoIy = lhs.PointA.y - lhsSlope * lhs.PointA.x;
		}
		else
		{
			PoIx = ((rhsYint - lhsYint) / (lhsSlope - rhsSlope));
			PoIy = rhsSlope * PoIx + rhsYint;
		}
	}
	//This shows the point of intersection. It might be useful to return.
//	Point point_of_intersection(PoIx, PoIy);
//	point_of_intersection.render();
	if (!((PoIx >= lhs.PointA.x && PoIx <= lhs.PointB.x) || (PoIx >= lhs.PointB.x && PoIx <= lhs.PointA.x)))
		return false;
	if (!((PoIy >= lhs.PointA.y && PoIy <= lhs.PointB.y) || (PoIy >= lhs.PointB.y && PoIy <= lhs.PointA.y)))
		return false;
	if (!((PoIx >= rhs.PointA.x && PoIx <= rhs.PointB.x) || (PoIx >= rhs.PointB.x && PoIx <= rhs.PointA.x)))
		return false;
	if (!((PoIy >= rhs.PointA.y && PoIy <= rhs.PointB.y) || (PoIy >= rhs.PointB.y && PoIy <= rhs.PointA.y)))
		return false;
	return true;
}

bool line_rect_collision(const Line& the_line, const Rect& the_rect)
{
	//find point x to test on top
	double line_slope = (the_line.PointB.y - the_line.PointA.y) / (the_line.PointB.x - the_line.PointA.x);
	double x_on_line = ((the_rect.y - the_line.PointA.y) / line_slope) + the_line.PointA.x;
	if (x_on_line >= the_rect.x && x_on_line <= the_rect.x + the_rect.w)
	{
		if (
			(the_line.PointA.y >= the_rect.y && the_line.PointB.y <= the_rect.y) ||
			(the_line.PointA.y <= the_rect.y && the_line.PointB.y >= the_rect.y)
			)
			return true;
	}
	//find point x to test on bottom
	//Only change here is " + the_rect.h"
	x_on_line = ((the_rect.y + the_rect.h - the_line.PointA.y) / line_slope) + the_line.PointA.x;
	if (x_on_line >= the_rect.x && x_on_line <= the_rect.x + the_rect.w)
	{
		if (
			(the_line.PointA.y >= the_rect.y + the_rect.h && the_line.PointB.y <= the_rect.y + the_rect.h) ||
			(the_line.PointA.y <= the_rect.y + the_rect.h && the_line.PointB.y >= the_rect.y + the_rect.h)
			)
			return true;
	}
	//find point y to test on left
	double y_on_line = (line_slope * (the_rect.x - the_line.PointA.x)) + the_line.PointA.y;
	if (y_on_line >= the_rect.y && y_on_line <= the_rect.y + the_rect.h)
	{
		if (
			(the_line.PointA.x >= the_rect.x && the_line.PointB.x <= the_rect.x) ||
			(the_line.PointA.x <= the_rect.x && the_line.PointB.x >= the_rect.x)
			)
			return true;
	}
	//find point y to test on right
	//Only change here is " + the_rect.w"
	y_on_line = (line_slope * (the_rect.x + the_rect.w - the_line.PointA.x)) + the_line.PointA.y;
	if (y_on_line >= the_rect.y && y_on_line <= the_rect.y + the_rect.h)
	{
		if (
			(the_line.PointA.x >= the_rect.x + the_rect.w && the_line.PointB.x <= the_rect.x + the_rect.w) ||
			(the_line.PointA.x <= the_rect.x + the_rect.w && the_line.PointB.x >= the_rect.x + the_rect.w)
			)
			return true;
	}
	return false;
}

void modify_velocity_with_gravity_and_limit(double* velocity, double* force_from_gravity)
{
	double newVelocity = 0;
	newVelocity = *force_from_gravity + (g_gravity * g_timer.deltaT_in_seconds());
	*velocity = g_timer.deltaT_in_seconds() * ((*force_from_gravity + newVelocity) / 2);
	if (newVelocity > g_terminal_velocity)
		newVelocity = g_terminal_velocity;
	*force_from_gravity = newVelocity;
}

void modify_velocity_with_gravity(double* velocity, double* force_from_gravity)
{
	double newVelocity = 0;
	newVelocity = *force_from_gravity + (g_gravity * g_timer.deltaT_in_seconds());
	*velocity = g_timer.deltaT_in_seconds() * ((*force_from_gravity + newVelocity) / 2);
	*force_from_gravity = newVelocity;
}

void modify_velocity_with_force(double* velocity, double* force_from_gravity, double force)
{
	double newVelocity = 0;
	newVelocity = *force_from_gravity + (force * g_timer.deltaT_in_seconds());
	*velocity = g_timer.deltaT_in_seconds() * ((*force_from_gravity + newVelocity) / 2);
	*force_from_gravity = newVelocity;
}

void modify_velocity_without_gravity(double* velocity, double* force_from_gravity)
{
	double newVelocity = 0;
	newVelocity = *force_from_gravity;
	*velocity = g_timer.deltaT_in_seconds() * ((*force_from_gravity + newVelocity) / 2);
	*force_from_gravity = newVelocity;
}

bool did_it_pass(double old_value, double new_value, const double magic_number)
{
	if (old_value < magic_number && new_value >= magic_number)
		return true;
	return false;
}

bool did_it_pass_neg(double old_value, double new_value, const double magic_number)
{
	if (old_value < magic_number && new_value <= magic_number)
		return true;
	return false;
}

//turn_point_in_a_box() optimized to the best of my abilities (in about a minute)
Point turn_point_in_a_box(const Point& movingPoint, const double midPointX, const double midPointY, const double angle)
{
	double distanceToMidX = midPointX - movingPoint.x + 0.000001;
	double distanceToMidY = midPointY - movingPoint.y + 0.000001;
	double newAngle = (angle / (180 / g_pi)) - (atan(distanceToMidX / distanceToMidY));
	double radius = sqrt(distanceToMidX * distanceToMidX + distanceToMidY * distanceToMidY);
	double distanceFromMidY = cos(newAngle) * radius;
	double distanceFromMidX = sin(newAngle) * radius;
	Point finalPoint;
	if (movingPoint.y > midPointY)
	{
		finalPoint.y = midPointY + distanceFromMidY;
		finalPoint.x = midPointX - distanceFromMidX;
	}
	else
	{
		finalPoint.y = midPointY - distanceFromMidY;
		finalPoint.x = midPointX + distanceFromMidX;
	}
	return finalPoint;
}
