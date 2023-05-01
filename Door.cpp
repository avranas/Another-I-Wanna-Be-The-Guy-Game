#include "stdafx.h"
#include "Simple Object.h"
#include "The Kid.h"

const double Gate::BUTTON_WIDTH = 9;
const double Gate::BUTTON_HEIGHT = 16;

void Gate::logic()
{
	switch (the_state_)
	{
	case Closed:
		for (auto bullet_iter = Game::the_bullets.begin(); bullet_iter != Game::the_bullets.end(); ++bullet_iter)
			if (rect_rect_collision(button_, (*bullet_iter)->the_box()))
				std::cout << "OPEN THE GATE" << std::endl;
		break;
	case Opening:
		break;
	case Open:
		break;
	}
}
