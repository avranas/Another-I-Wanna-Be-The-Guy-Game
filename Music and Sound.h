#ifndef MUSIC_AND_SOUND_H_
#define MUSIC_AND_SOUND_H_
#include <SDL.h>
#include <SDL_mixer.h>
#include "globals.h"

//When you add another track, be sure to modify Music_Manager::switch_to_track_number()
namespace Music
{
	extern Mix_Music* tutorial_bgm;
	extern Mix_Music* world_one_bgm_part_1;
	extern Mix_Music* world_one_bgm_part_2;
	extern Mix_Music* world_two_bgm;
	extern Mix_Music* title_part_one;
	extern Mix_Music* title_part_two;
	extern Mix_Music* death_theme;
	extern Mix_Music* boss_music_part_one;
	extern Mix_Music* boss_music_part_two;
	extern Mix_Music* victory;
	extern Mix_Music* star_get;
}

namespace Sound
{
	extern Mix_Chunk* disappearing_block;
	extern Mix_Chunk* save;
	extern Mix_Chunk* the_kid_dies;
	extern Mix_Chunk* jump_one;
	extern Mix_Chunk* jump_two;
	extern Mix_Chunk* shoot;
	extern Mix_Chunk* door_open;
	extern Mix_Chunk* fireball;
	extern Mix_Chunk* quack;
	extern Mix_Chunk* block_break;
	extern Mix_Chunk* victory;
	extern Mix_Chunk* spring;
	extern Mix_Chunk* meow;
	extern Mix_Chunk* switch_flip;
	extern Mix_Chunk* thunderbolt;
	extern Mix_Chunk* bomb;
	extern Mix_Chunk* use_mushroom;
	extern Mix_Chunk* drop_mushroom;
	extern Mix_Chunk* dag;
	extern Mix_Chunk* whoa;
	extern Mix_Chunk* patton;
	extern Mix_Chunk* you_must_die;
	extern Mix_Chunk* gattle;
	extern Mix_Chunk* sonic_boom;
	extern Mix_Chunk* oh_boy;
	extern Mix_Chunk* you_will_die;
	extern Mix_Chunk* pew;
	extern Mix_Chunk* i_won;
	extern Mix_Chunk* boom;
	extern Mix_Chunk* menu;
}

void load_all_music();
void load_all_sounds();
void music_finished();

namespace Game
{
	extern Mix_Music* next_track_;
}

class Music_Manager
{
public:
	Music_Manager(Mix_Music* default_track) :
		current_music_(default_track),
		music_paused_for_some_time_(false)
	{

	}

	Mix_Music* current_track()
	{
		return current_music_;
	}

	void pause_music_for_some_time(int time)
	{
		music_paused_for_some_time_ = true;
		timer_ = time;
	}

	void play_temp_music(Mix_Music* temp_music, int loops)
	{
		temporary_music_ = temp_music;
		Mix_PlayMusic(temporary_music_, loops);
	}

	void go_to_next_track();

	void revert_to_previous_music()
	{
		Mix_PlayMusic(current_music_, -1);
	}

	void start_music()
	{
		Mix_PlayMusic(current_music_, -1);
	}

	void pause_music()
	{
		if (Mix_PlayingMusic())
			Mix_PauseMusic();
	}

	void unpause_music()
	{
		if (Mix_PausedMusic())
			Mix_ResumeMusic();
	}

	//This is for Main_Game::logic() as you know.
	void logic()
	{
		Mix_HookMusicFinished(music_finished);
		if (!Mix_PlayingMusic())
		{
			if (music_paused_for_some_time_)
			{
				timer_ -= g_timer.deltaT();
				if (timer_ <= 0)
				{
					timer_ = 0;
					music_paused_for_some_time_ = false;
				}
			}
		}
	}

	void switch_to_track_number(int number);

	void loop_this_track(Mix_Music* this_one)
	{
		current_music_ = this_one;
		Game::next_track_ = this_one;
		Mix_PlayMusic(current_music_, -1);
	}

	//Volume control
	void play_this_track_now(Mix_Music* this_one)
	{
		current_music_ = this_one;
		Mix_PlayMusic(current_music_, 0);
	}

private:
	bool music_paused_for_some_time_;
	int timer_;
	Mix_Music* current_music_;
	Mix_Music* temporary_music_;
};

void set_next_track(Mix_Music* next_track);

namespace Game
{
	extern Music_Manager* background_music;
}

#endif