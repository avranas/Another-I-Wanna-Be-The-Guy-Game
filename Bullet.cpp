#include "stdafx.h"
#include "Bullet.h"

const double Bullet::BULLET_WIDTH = 4;
const double Bullet::BULLET_HEIGHT = 4;
const double Bullet::BULLET_SPEED = 600;

namespace Game
{
	vector<Bullet*> the_bullets;
}

void Bullet::logic()
{
	//move the bullets
	previous_position_.x = location_.x;
	previous_position_.y = location_.y;
	if (direction_ == SDL_FLIP_NONE)
		location_.x += BULLET_SPEED * g_timer.deltaT_in_seconds();
	else
		location_.x -= BULLET_SPEED * g_timer.deltaT_in_seconds();

	//check for collisions
	//I copied this from Blood::logic(). It can probably be done in some\
	kind of function. Easily, even. Don't feel like doing it, though.
}


bool box_collides_with_bullet(Rect* box)
{
	for (auto iter = Game::the_bullets.begin(); iter != Game::the_bullets.end(); ++iter)
		if (rect_rect_collision((*iter)->the_box(), *box))
		{
			(*iter)->prepare_to_delete();
			return true;
		}
	return false;
}