#ifndef TIMER_H_
#define TIMER_H_
#include <SDL.h>
//There will be one timer in each program that keeps track of
class Timer
{
public:
	Timer();

	inline void start()
	{
		started_ = true;
		paused_ = false;
		ticks_at_last_call_ = SDL_GetTicks();
		starting_ticks_ = ticks_at_last_call_;
	}

	inline void stop()
	{
		started_ = false;
		paused_ = false;
	}

	inline void Timer::pause()
	{
		if ((started_ == true) && (paused_ == false))
		{
			paused_ = true;
			paused_ticks_ = SDL_GetTicks() - starting_ticks_;
		}
	}

	inline void unpause()
	{
		if (paused_ == true)
		{
			paused_ = false;
			ticks_at_last_call_ = SDL_GetTicks() - paused_ticks_;
			paused_ticks_ = 0;
		}
	}

	//Call this function once per main loop.
	inline void update_deltaT()
	{
		deltaT_ = SDL_GetTicks() - ticks_at_last_call_;
		//I was having collision detection problems with tiles when the program
		//was running very fast
		if (deltaT_ <= 2)
		{
			SDL_Delay(2);
			deltaT_ = SDL_GetTicks() - ticks_at_last_call_;
		}
		ticks_at_last_call_ = SDL_GetTicks();
		//The idea is if it takes longer than 1/60th of a second to complete a loop
		if (deltaT_ >= 16)
		{
			deltaT_ = 16;
		}
		//deltaT will be measured in milliseconds
	}

	//This is the one we use for calculating speed.
	inline double deltaT_in_seconds() { return static_cast<double>(deltaT_) / 1000; }
	inline int deltaT() { return deltaT_; }
	inline bool started() { return started_; }
	inline bool paused() { return paused_; }
	inline int ticks() { return ticks_at_last_call_; }
private:
	bool first_call_;
	int ticks_at_last_call_;
	int paused_ticks_;
	bool paused_;
	bool started_;
	//Time since last call in seconds
	int deltaT_;
	int starting_ticks_;
};

#endif
