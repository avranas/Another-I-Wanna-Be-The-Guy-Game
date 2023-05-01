#ifndef SAVE_BOX_H
#define SAVE_BOX_H
#include <SDL.h>
#include "Image Processing.h"
#include "timer.h"
#include "globals.h"
#include "Smart Object.h"
#include "Special Platform.h"
#include <fstream>
#include <vector>

class Save_Switch : public Special_Platform
{
public:
	Save_Switch(double x, double y, bool wussify) :
		Special_Platform(x, y, 32, 32),
		the_state_(Checking),
		wuss_(wussify)
	{
		add_new_texture(Pic::save_switch_idle);
		add_new_texture(Pic::save_switch_pressed);
		add_new_texture(Pic::wuss_switch_idle);
		add_new_texture(Pic::wuss_switch_pressed);
	}

	void logic();

private:
	enum Switch_State
	{
		Checking,
		Waiting,
		Pressed
	};
	Switch_State the_state_;
	bool wuss_;
};

class Save_Box : public Smart_Object
{
public:
	Save_Box(double x, double y) :
		Smart_Object(x, y),
		saving_(false),
		save_animation_timer_(0)
	{
		Pic::save_box_idle->init();
		Pic::save_box_saving->init();
		the_pics_.push_back(Pic::save_box_idle);
		the_pics_.push_back(Pic::save_box_saving);
	}

	inline Rect the_box()
	{
		Rect new_rect(location_.x, location_.y, WIDTH, HEIGHT);
		return new_rect;
	}

	void logic();

private:
	int save_animation_timer_;
	bool saving_;
	const static int SAVE_ANIMATION_DURATION = 600;
	const static double HEIGHT;
	const static double WIDTH;
};

class Wuss_Box : public Save_Box
{
public:
	Wuss_Box(double x, double y) :
		Save_Box(x, y)
	{
		the_pics_.clear();
		Pic::wuss_box_idle->init();
		Pic::wuss_box_saving->init();
		the_pics_.push_back(Pic::wuss_box_idle);
		the_pics_.push_back(Pic::wuss_box_saving);
	}
};

class Common_Save_Data
{
public:
	Common_Save_Data(string file_name) :
		file_name_(file_name)
	{
		open_from_file();
	}

	void open_from_file()
	{
		ifstream save_file(file_name_);
		save_file >> blood_intensity_;
		save_file >> sound_volume_;
		save_file >> music_volume_;
	}

	int music_volume()
	{
		return music_volume_;
	}

	int sound_volume()
	{
		return sound_volume_;
	}

	int blood_intensity()
	{
		return blood_intensity_;
	}

	void update_and_save(int blood_intensity, int sound_volume, int music_volume)
	{
		blood_intensity_ = blood_intensity;
		sound_volume_ = sound_volume;
		music_volume_ = music_volume;
		write_to_file();
	}

	void write_to_file()
	{
		ofstream save_data(file_name_);
		save_data.clear();
		save_data << blood_intensity_ << "\n";
		save_data << sound_volume_ << "\n";
		save_data << music_volume_ << "\n";
	}

private:
	string file_name_;
	int blood_intensity_;
	int sound_volume_;
	int music_volume_;
};

class Save_File
{
public:
	Save_File(string file_name) :
		file_name_(file_name)
	{
		open_from_file();
	}

	//Called at the end of every function that changes save data.
	void open_from_file()
	{
		ifstream save_file(file_name_);
		save_file >> player_name_;
		save_file >> the_set_difficulty_;
		save_file >> world_;
		save_file >> screen_x_;
		save_file >> screen_y_;
		save_file >> character_position_x_;
		save_file >> character_position_y_;
		save_file >> death_counter_;
		save_file >> reset_counter_;
		save_file >> time_;
		save_file >> has_star_;
		save_file >> has_teleporter_;
	}

	bool has_star()
	{
		return has_star_;
	}

	string file_name()
	{
		return file_name_;
	}

	inline double character_position_x()
	{
		return character_position_x_;
	}

	inline double character_position_y()
	{
		return character_position_y_;
	}

	inline int world()
	{
		return world_;
	}

	inline int screen_x()
	{
		return screen_x_;
	}

	inline int screen_y()
	{
		return screen_y_;
	}

	inline int death_counter()
	{
		return death_counter_;
	}

	inline int reset_counter()
	{
		return reset_counter_;
	}

	inline long time()
	{
		return time_;
	}

	inline int difficulty()
	{
		return the_set_difficulty_;
	}

	inline void add_time(long value)
	{
		time_ += value;
	}

	bool has_teleporter()
	{
		return has_teleporter_;
	}

	void update_location_and_save(double position_x, double position_y);
	void get_star_and_save();
	void get_star_in_hard_mode();
	void get_teleporter_and_save();
	void increment_deaths_and_save();
	void increment_resets_and_save();
	void update_time_and_save();

private:
	void write_to_file()
	{
		ofstream save_data(file_name_);
		save_data.clear();
		save_data << player_name_ << "\n";
		save_data << the_set_difficulty_ << "\n";
		save_data << world_ << "\n";
		save_data << screen_x_ << "\n";
		save_data << screen_y_ << "\n";
		save_data << character_position_x_ << "\n";
		save_data << character_position_y_ << "\n";
		save_data << death_counter_ << "\n";
		save_data << reset_counter_ << "\n";
		save_data << time_ << "\n";
		save_data << has_star_ << "\n";
		save_data << has_teleporter_ << "\n";
	}

	string player_name_;
	double character_position_x_;
	double character_position_y_;
	int world_;
	int screen_x_;
	int screen_y_;
	int death_counter_;
	int reset_counter_;
	//Time spent playing a file in ms.
	long time_;
	const string file_name_;
	int file_position_;
	int the_set_difficulty_;
	bool has_star_;
	bool has_teleporter_;
};

extern Save_File* save_file;
extern Common_Save_Data* common_save_data;
inline void save_the_game(double new_position_x, double new_position_y);

#endif