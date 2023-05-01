#ifndef BULLET_H_
#define BULLET_H_
#include "Image Processing.h"

class Bullet : public Displayable_Object
{
public:
	Bullet(Point bullet_entryway, SDL_RendererFlip direction) :
		Displayable_Object(bullet_entryway.x, bullet_entryway.y),
		previous_position_(Point(bullet_entryway.x, bullet_entryway.y))
	{
		direction_ = direction;
		the_pics_.push_back(Pic::bullet);
	}

	void logic();

	void render_bullet_box()
	{
		Rect hit_box = the_box();
		SDL_Rect render_box;
		render_box.x = static_cast<int>(hit_box.x);
		render_box.y = static_cast<int>(hit_box.y);
		render_box.w = static_cast<int>(hit_box.w);
		render_box.h = static_cast<int>(hit_box.h);
		SDL_RenderDrawRect(g_renderer, &render_box);
	}

	inline Rect the_box()const
	{
		Rect new_rect;
		if (direction_ == SDL_FLIP_NONE)
		{
			new_rect.x = previous_position_.x;
			new_rect.y = previous_position_.y;
			new_rect.w = (location_.x + BULLET_WIDTH) - previous_position_.x;
			new_rect.h = BULLET_HEIGHT;
		}
		else
		{
			new_rect.x = location_.x;
			new_rect.y = location_.y;
			new_rect.w = (previous_position_.x + BULLET_WIDTH) - location_.x;
			new_rect.h = BULLET_HEIGHT;
		}
		return new_rect;
	}

private:
	Point previous_position_;
	const static double BULLET_WIDTH;
	const static double BULLET_HEIGHT;
	const static double BULLET_SPEED;
};

namespace Game
{
	extern vector<Bullet*> the_bullets;
}

//Iterates through Game::the_bullets. If a collision is found, return true and destory the bullet
bool box_collides_with_bullet(Rect* box);

#endif