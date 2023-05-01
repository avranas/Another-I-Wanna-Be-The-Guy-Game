#ifndef THE_KID_H_
#define THE_KID_H_
#include <SDL.h>
#include "SDL_rect.h"
#include "globals.h"
#include "Worlds and Maps.h"
#include "Bullet.h"
#include "Music and Sound.h"

class The_Kid : public Displayable_Object
{
public:
	The_Kid() :
		Displayable_Object(0, 0),
		left_(false),
		right_(false),
		xVelocity_(0),
		yVelocity_(0),
		force_from_gravity_(0),
		jumping_time_(0),
		first_jump_(true),
		second_jump_(false),
		additional_velocity_y_(0),
		additional_velocity_x_(0),
		immobilized_(0),
		dead_(false),
		loaded_bullet_(false)
	{
		current_animation_ = 0;
		add_new_texture(Pic::the_kid_idle);
		add_new_texture(Pic::the_kid_walking);
		add_new_texture(Pic::the_kid_jumping);
		add_new_texture(Pic::the_kid_falling);
	}

	void The_Kid::kill_the_kid();

	inline bool dead()
	{
		return dead_;
	}

	inline void load_bullet()
	{
		loaded_bullet_ = true;
	}

	void test_for_collision(Tiled_Map* the_map);
	void test_for_collision(std::vector<Tile*>* extra_walls);

	inline Point bullet_entryway()
	{
		Point new_point;
		//Move these values around to change the starting point when a bullet is fired.
		if (direction_ == SDL_FLIP_NONE)
		{
			new_point.x = location_.x + BULLET_OFFSET_X;
			new_point.y = location_.y + BULLET_OFFSET_Y;
		}
		else
		{
			new_point.x = location_.x - 5; //bleh
			new_point.y = location_.y + BULLET_OFFSET_Y;
		}
		return new_point;
	}

	inline void set_velocity_x(double value)
	{
		xVelocity_ = value;
	}

	inline void set_velocity_y(double value)
	{
		yVelocity_ = value;
	}

	inline void remove_additional_velocity_y()
	{
		additional_velocity_y_ = 0;
	}

	inline void remove_additional_velocity_x()
	{
		additional_velocity_x_ = 0;
	}

	inline void immobilize(int value)
	{
		immobilized_ = value;
	}

	//When an outside force makes the kid leap upward.
	//For example: When the kid jumps on a spring, this function is called. 
	inline void push_up(double jump_power)
	{
		force_from_gravity_ = -jump_power;
		jumping_time_ = 1;
	}

	//A bullet can't be loaded when you're dead.
	inline bool loaded_bullet()
	{
		return loaded_bullet_ && !dead_;
	}

	void remove_bullet()
	{
		loaded_bullet_ = false;
	}

	inline void jump()
	{
		if (jumping_time_ > 0 || immobilized_ > 0 || dead_)
			return;
		if (!first_jump_)
		{
			Mix_PlayChannel(-1, Sound::jump_one, 0);
			first_jump_ = true;
			force_from_gravity_ = -JUMP_POWER;
			jumping_time_ = 1;
		}
		else if (!second_jump_)
		{
			Mix_PlayChannel(-1, Sound::jump_two, 0);
			second_jump_ = true;
			//Important!!:: the value after "= -JUMP_POWER;" here, is the difference\
			in jump height from the first_jump! You know, in case you wanted to play with\
			that...
			force_from_gravity_ = -JUMP_POWER * 0.85;
			jumping_time_ = 1;
		}
		else
			return;
	}

	inline void stop_jumping()
	{
		jumping_time_ = 0;
		yVelocity_ /= 1.75;
		force_from_gravity_ /= 1.75;
	}

	inline void bump_head()
	{
		force_from_gravity_ = 0;
		yVelocity_ = 0;
	}

	inline void land()
	{
		first_jump_ = false;
		second_jump_ = false;
		jumping_time_ = 0;
		force_from_gravity_ = 0;
		yVelocity_ = 0;
	}

	//this is for testing purposes.
	inline void move_to_position(double x, double y)
	{
		location_.x = x;
		location_.y = y;
	}

	inline Rect kid_hit_box()
	{
		//This was copied (and modified) from Collision_Data::test_for_collision().
		//You can add width_ratio and height ratio if you want to change the kid's size 
		return Rect(
			hit_box_left(),
			hit_box_top(),
			hit_box_w(),
			hit_box_h()
		);
	}

	inline Point projected_point()
	{
		Point new_point(projection_left(), projection_top());
		return new_point;
	}

	inline double hit_box_left()const
	{
		return location_.x + KID_HIT_BOX_OFFSET_X;
	}

	inline double hit_box_right()const
	{
		return location_.x + THE_KID_WIDTH + KID_HIT_BOX_OFFSET_X;
	}

	inline double hit_box_top()const
	{
		return location_.y + KID_HIT_BOX_OFFSET_Y;;
	}

	inline double hit_box_bottom()const
	{
		return location_.y + THE_KID_HEIGHT + KID_HIT_BOX_OFFSET_Y;
	}

	inline double hit_box_center_x()const
	{
		return hit_box_left() + (THE_KID_WIDTH / 2);
	}

	inline double hit_box_center_y()const
	{
		return location_.y + (THE_KID_HEIGHT / 2);
	}

	inline double hit_box_center_w()const
	{
		return THE_KID_WIDTH / 2;
	}

	inline double hit_box_center_h()const
	{
		return THE_KID_HEIGHT / 2;
	}

	inline double hit_box_w()const
	{
		return THE_KID_WIDTH;
	}

	inline double hit_box_h()const
	{
		return THE_KID_HEIGHT;
	}

	inline double projection_left()const
	{
		return hit_box_left() + xVelocity_ + additional_velocity_x_;
	}

	inline double projection_top()const
	{
		return hit_box_top() + yVelocity_ + additional_velocity_y_;
	}

	inline double projection_bottom()const
	{
		return hit_box_bottom() + yVelocity_ + additional_velocity_y_;
	}

	inline double projection_right()const
	{
		return hit_box_right() + xVelocity_ + additional_velocity_x_;
	}

	inline void render_hit_box()
	{
		SDL_Rect render_this(static_cast<int>(hit_box_left() - Game::camera->x()), static_cast<int>(hit_box_top() - Game::camera->y()), static_cast<int>(THE_KID_WIDTH), static_cast<int>(THE_KID_HEIGHT));
		SDL_RenderDrawRect(g_renderer, &render_this);
	}

	inline void face_left()
	{
		direction_ = SDL_FLIP_HORIZONTAL;
	}

	inline void face_right()
	{
		direction_ = SDL_FLIP_NONE;
	}

	inline void start_walking_right()
	{
		right_ = true;
	}

	inline void stop_walking_right()
	{
		xVelocity_ = 0;
		right_ = false;
	}

	inline void start_walking_left()
	{
		left_ = true;
	}

	inline void stop_walking_left()
	{
		xVelocity_ = 0;
		left_ = false;
	}

	inline void set_hit_box_left(double value)
	{
		location_.x = value - KID_HIT_BOX_OFFSET_X;
	}

	inline void set_hit_box_right(double value)
	{
		location_.x = value - KID_HIT_BOX_OFFSET_X - THE_KID_WIDTH;
	}

	inline void set_hit_box_top(double value)
	{
		location_.y = value - KID_HIT_BOX_OFFSET_Y;
	}

	inline void set_hit_box_bottom(double value)
	{
		location_.y = value - KID_HIT_BOX_OFFSET_Y - THE_KID_HEIGHT;
	}

	inline void set_additional_velocity_y(double value)
	{
		additional_velocity_y_ = value;
	}

	inline bool test_collision_with_platform(const Special_Platform& platform)
	{
		return (
			hit_box_bottom() <= platform.previous_y() &&
			projection_bottom() >= platform.top() &&
			projection_left() <= platform.right() &&
			projection_right() >= platform.left()
			);
	}

	//How about we put this in render()?
	inline void pick_an_animation()
	{

		if (xVelocity_ > 0 && immobilized_ == 0)
		{
			direction_ = SDL_FLIP_NONE;
		}
		if (xVelocity_ < 0 && immobilized_ == 0)
		{
			direction_ = SDL_FLIP_HORIZONTAL;
		}
		if (yVelocity_ > 0)
		{
			switch_to_animation(3);
		}
		else if (yVelocity_ < 0)
		{
			switch_to_animation(2);
		}
		else if (xVelocity_ != 0 && immobilized_ == 0)
		{
			switch_to_animation(1);
		}
		else
		{
			switch_to_animation(0);
		}
	}

	void handle_movement();

	bool in_air()
	{
		return yVelocity_ != 0;
	}

private:
	const static double THE_KID_WIDTH;
	const static double THE_KID_VELOCITY;
	const static double THE_KID_HEIGHT;
	const static double KID_HIT_BOX_OFFSET_X;
	const static double KID_HIT_BOX_OFFSET_Y;
	const static double MAX_JUMPING_TIME;
	const static double BULLET_OFFSET_X;
	const static double BULLET_OFFSET_Y;
	const static int HIT_BOX_OFFSET_X = 8;
	const static int HIT_BOX_OFFSET_Y = 0;
	bool left_;
	bool right_;
	double xVelocity_;
	double force_from_gravity_;
	double yVelocity_;
	const static double JUMP_POWER;
	double additional_velocity_x_;
	double additional_velocity_y_;
	bool first_jump_;
	bool second_jump_;
	double jumping_time_;
	int immobilized_;
	bool dead_;
	bool loaded_bullet_;
};

namespace Game
{
	extern The_Kid* the_kid;
}

#endif
