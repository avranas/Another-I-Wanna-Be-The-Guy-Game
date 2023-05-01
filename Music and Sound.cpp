#include "stdafx.h"
#include "Music and Sound.h"
#include <iostream>

//When you add another track, be sure to modify\
Music_Manager::switch_to_track_number() and load_all_music()
namespace Music
{
	Mix_Music* tutorial_bgm = nullptr;
	Mix_Music* world_one_bgm_part_1 = nullptr;
	Mix_Music* world_one_bgm_part_2 = nullptr;
	Mix_Music* world_two_bgm = nullptr;
	Mix_Music* title_part_one = nullptr;
	Mix_Music* title_part_two = nullptr;
	Mix_Music* death_theme = nullptr;
	Mix_Music* boss_music_part_one = nullptr;
	Mix_Music* boss_music_part_two = nullptr;
	Mix_Music* victory = nullptr;
	Mix_Music* star_get = nullptr;
}

namespace Sound
{
	Mix_Chunk* disappearing_block = nullptr;
	Mix_Chunk* save = nullptr;
	Mix_Chunk* the_kid_dies = nullptr;
	Mix_Chunk* jump_one = nullptr;
	Mix_Chunk* jump_two = nullptr;
	Mix_Chunk* shoot = nullptr;
	Mix_Chunk* door_open = nullptr;
	Mix_Chunk* fireball = nullptr;
	Mix_Chunk* quack = nullptr;
	Mix_Chunk* block_break = nullptr;
	Mix_Chunk* victory = nullptr;
	Mix_Chunk* spring = nullptr;
	Mix_Chunk* meow = nullptr;
	Mix_Chunk* switch_flip = nullptr;
	Mix_Chunk* thunderbolt = nullptr;
	Mix_Chunk* bomb = nullptr;
	Mix_Chunk* use_mushroom = nullptr;
	Mix_Chunk* drop_mushroom = nullptr;
	Mix_Chunk* dag = nullptr;
	Mix_Chunk* whoa = nullptr;
	Mix_Chunk* patton = nullptr;
	Mix_Chunk* you_must_die = nullptr;
	Mix_Chunk* gattle = nullptr;
	Mix_Chunk* sonic_boom = nullptr;
	Mix_Chunk* oh_boy = nullptr;
	Mix_Chunk* you_will_die = nullptr;
	Mix_Chunk* pew = nullptr;
	Mix_Chunk* i_won = nullptr;
	Mix_Chunk* boom = nullptr;
	Mix_Chunk* menu = nullptr;
}

void Music_Manager::go_to_next_track()
{
	play_this_track_now(Game::next_track_);
}

void Music_Manager::switch_to_track_number(int number)
{
	switch (number)
	{
	case 0:
		loop_this_track(Music::tutorial_bgm);
		break;
	case 1:
		play_this_track_now(Music::world_one_bgm_part_1);
		set_next_track(Music::world_one_bgm_part_2);
		break;
	case 2:
		loop_this_track(Music::world_two_bgm);
		break;
	}
}

Mix_Music* load_a_music_file(string file_name)
{
	Mix_Music* return_this = Mix_LoadMUS(file_name.c_str());
	if (return_this == nullptr)
		cout << "Oh no! The music file: " << file_name.c_str() << " didn't load!" << " %s\n", Mix_GetError();
	return return_this;
}

Mix_Chunk* load_a_sound_file(string file_name)
{
	Mix_Chunk* return_this = Mix_LoadWAV(file_name.c_str());
	if (return_this == nullptr)
		cout << "Oh no! The sound file: " << file_name.c_str() << " didn't load!" << " %s\n", Mix_GetError();
	return return_this;
}

void load_all_music()
{
	using namespace Music;
	tutorial_bgm = load_a_music_file("Music/Tutorial.ogg");
	world_one_bgm_part_1 = load_a_music_file("Music/World One Part 1.ogg");
	world_one_bgm_part_2 = load_a_music_file("Music/World One Part 2.ogg");
	world_two_bgm = load_a_music_file("Music/World Two.ogg");
	title_part_one = load_a_music_file("Music/Title Part One.ogg");
	title_part_two = load_a_music_file("Music/Title Part Two.ogg");
	death_theme = load_a_music_file("Music/Death Theme.ogg");
	boss_music_part_one = load_a_music_file("Music/Boss Music Part 1.ogg");
	boss_music_part_two = load_a_music_file("Music/Boss Music Part 2.ogg");
	victory = load_a_music_file("Music/Victory.ogg");
	star_get = load_a_music_file("Music/Star Get.ogg");
}

void load_all_sounds()
{
	using namespace Sound;
	disappearing_block = load_a_sound_file("Sound/Disappearing Block.wav");
	save = load_a_sound_file("Sound/Save.wav");
	the_kid_dies = load_a_sound_file("Sound/Bomb.ogg");
	jump_one = load_a_sound_file("Sound/Jump One.wav");
	jump_two = load_a_sound_file("Sound/Jump Two.wav");
	shoot = load_a_sound_file("Sound/Shoot.wav");
	door_open = load_a_sound_file("Sound/Door Open.wav");
	fireball = load_a_sound_file("Sound/Fireball.wav");
	quack = load_a_sound_file("Sound/Quack.wav");
	block_break = load_a_sound_file("Sound/Block Break.wav");
	spring = load_a_sound_file("Sound/Spring.ogg");
	meow = load_a_sound_file("Sound/meow.wav");
	switch_flip = load_a_sound_file("Sound/Switch Flip.ogg");
	thunderbolt = load_a_sound_file("Sound/Thunderbolt.ogg");
	bomb = load_a_sound_file("Sound/Bomb.ogg");
	use_mushroom = load_a_sound_file("Sound/Use Mushroom.wav");
	drop_mushroom = load_a_sound_file("Sound/Drop Mushroom.wav");
	you_must_die = load_a_sound_file("Sound/You Must Die.wav");
	gattle = load_a_sound_file("Sound/gattle.wav");
	sonic_boom = load_a_sound_file("Sound/Sonic Boom.wav");
	oh_boy = load_a_sound_file("Sound/Oh Boy.ogg");
	you_will_die = load_a_sound_file("Sound/You Will Die.wav");
	pew = load_a_sound_file("Sound/pew.wav");
	i_won = load_a_sound_file("Sound/I Won.ogg");
	boom = load_a_sound_file("Sound/boom.ogg");
	menu = load_a_sound_file("Sound/menu.ogg");
}

void music_finished()
{
	if (Game::next_track_ != nullptr)
		Game::background_music->loop_this_track(Game::next_track_);
}

void set_next_track(Mix_Music* next_track)
{
	Game::next_track_ = next_track;
}

namespace Game
{
	Music_Manager* background_music = new Music_Manager(Music::tutorial_bgm);
	Mix_Music* next_track_ = nullptr;
}
