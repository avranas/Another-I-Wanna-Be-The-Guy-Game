#include "stdafx.h"
#include "timer.h"
Timer::Timer() :
	paused_ticks_(0),
	paused_(false),
	started_(false),
	deltaT_(0),
	ticks_at_last_call_(SDL_GetTicks())
{}