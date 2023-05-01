#ifndef TITLE_SCREEN_H_
#define TITLE_SCREEN_H_
#include "globals.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "Text_Box.h"
#include "Image Processing.h"
#include "Music and Sound.h"
#include "Save Box.h"

class Title_Pic : public Displayable_Object
{
public:
	Title_Pic(double x, double y) :
		Displayable_Object(x, y)
	{
		add_new_texture(Pic::title);
	}

	void logic()
	{
		if (location_.y > FINAL_LOCATION_Y)
		{
			location_.y -= TITLE_SPEED * g_timer.deltaT_in_seconds();
		}
		else
		{
			g_next_game_state = StateTitleScreen;
		}
	}

private:
	const static double TITLE_SPEED;
	const static double FINAL_LOCATION_Y;
};

class Introduction
{
public:
	Introduction() :
		title_pic(new Title_Pic(10, 888))
	{
		common_save_data = new Common_Save_Data("Common Save Data.txt");
		Mix_Volume(-1, common_save_data->sound_volume());
		Mix_VolumeMusic(common_save_data->music_volume());
		Game::background_music->play_this_track_now(Music::title_part_one);
		set_next_track(Music::title_part_two);
	}

	~Introduction()
	{
		delete title_pic;
		title_pic = nullptr;
	}

	void handle_events()
	{
		while (SDL_PollEvent(&g_event))
		{
			if (g_event.type == SDL_QUIT)
			{
				g_main_loop_is_running = false;
			}
			if (g_event.type == SDL_WINDOWEVENT)
			{
				switch (g_event.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
					SDL_Log("Window %d resized to %dx%d",
						g_event.window.windowID, g_event.window.data1,
						g_event.window.data2);
					change_screen_width(g_event.window.data1);
					change_screen_height(g_event.window.data2);
					break;
				case SDL_WINDOWEVENT_MINIMIZED:
					g_window_is_minimized = true;
					break;
				case SDL_WINDOWEVENT_RESTORED:
					g_window_is_minimized = false;
					break;
				default:
					break;
				}
			}
			if (g_event.type == SDL_KEYDOWN)
			{
				g_next_game_state = StateTitleScreen;
			}
		}
	}

	void logic()
	{
		Game::background_music->logic();
		title_pic->logic();
	}

	void render()
	{
		SDL_RenderClear(g_renderer);
		title_pic->draw_to_screen();
		SDL_RenderPresent(g_renderer);
	}

private:
	Title_Pic* title_pic;
};

//You know, the pointing finger on the title screen. 
class Finger : public Displayable_Object
{
public:
	Finger() :
		Displayable_Object(0, 0)
	{
		add_new_texture(Pic::finger);
	}

	void put_in_position(int position)
	{
		location_.x = 35;
		location_.y = 191 + position * 35;
	}

private:
};

class Option_Meter
{
public:
	Option_Meter(std::string option_name, double x, double y, int position) :
		option_name_(new Text_Box(x, y, option_name.c_str(), Font::Calibri_24, Color::title_choices, 1)),
		value_(new Text_Box(x, y, to_string(position).c_str(), Font::Calibri_24, Color::title_choices, 1)),
		location_(Point(x, y)),
		position_(position),
		meter_(new Displayable_Object(x, y + 6, Pic::bar)),
		ball_(new Displayable_Object(x, y + 6, Pic::ball))
	{
		update_value_text_and_save();
	}

	int value() {
		return position_;
	}

	~Option_Meter()
	{
		delete meter_;
		meter_ = nullptr;
		delete ball_;
		ball_ = nullptr;
		delete option_name_;
		option_name_;
		delete value_;
		value_ = nullptr;
	}

	void render()
	{
		value_->new_location_x(meter_offset + percent_offset);
		value_->simple_render();
		option_name_->simple_render();

		SDL_Rect meter_destination = meter_->box_on_window();
		meter_destination.x += meter_offset;

		SDL_RenderCopy(
			g_renderer,
			meter_->current_pic(),
			&meter_->box_in_file(),
			&meter_destination);

		SDL_Rect ball_destination = ball_->box_on_window();
		ball_destination.x += position_ * 2 + meter_offset;


		SDL_RenderCopy(
			g_renderer,
			ball_->current_pic(),
			&ball_->box_in_file(),
			&ball_destination);
	}

	void increment()
	{
		if (position_ < MAX_POSITION)
		{
			position_ += 2;
			update_value_text_and_save();
		}
	}

	void decrement()
	{
		if (position_ > MIN_POSITION)
		{
			position_ -= 2;
			update_value_text_and_save();
		}
	}

	void set_new_value(int value)
	{
		position_ = value;
		update_value_text_and_save();
	}

	void update_value_text_and_save();

private:
	Point location_;
	Displayable_Object* meter_;
	Displayable_Object* ball_;
	const static Point ball_offset_;
	int position_;
	const static int MIN_POSITION = 0;
	const static int MAX_POSITION = 100;
	Text_Box* option_name_;
	Text_Box* value_;
	const static int meter_offset = 162;
	const static int percent_offset = 316;
};

class Title_Screen
{
public:
	Title_Screen();
	~Title_Screen();
	void handle_events();
	void logic();
	void render();
private:
	class File_Information
	{
	private:
		Text_Box* Resets;
		Text_Box* Time;
		Text_Box* Difficulty;
		Displayable_Object* star_pic_;
		Displayable_Object* teleporter_pic_;
	public:
		void render()
		{
			//World->simple_render();
			//Deaths->simple_render();
			Resets->simple_render();
			Time->simple_render();
			Difficulty->simple_render();
			star_pic_->simple_render();
			teleporter_pic_->simple_render();

		}

		File_Information(string file_name)
		{
			const SDL_Color file_info_color = SDL_Color(0x00, 0xFF, 0x00, 0x00);
			ifstream read_this(file_name);
			string new_string;
			//Name
			read_this >> new_string;
			//difficulty
			int difficulty_value;
			read_this >> difficulty_value;
			switch (difficulty_value)
			{
			case 0:
				new_string = "Easy Mode";
				break;
			case 1:
				new_string = "Normal Mode";
				break;
			case 2:
				new_string = "Stupid Hard Mode";
				break;
			}
			Difficulty = new Text_Box(380, 190, new_string.c_str(), Font::Calibri_24, Color::file_info, 1);
			//position
			read_this >> new_string;
			read_this >> new_string;
			//world
			read_this >> new_string;
			string newer_string = "World: " + new_string;
			//screen
			read_this >> new_string;
			read_this >> new_string;
			//deaths
			read_this >> new_string;
			//new_string += " deaths";
			//Deaths = new Text_Box(380, 225, "", Font::Calibri_24, Color::file_info, 1);
			//resets
			read_this >> new_string;
			new_string += " resets";
			Resets = new Text_Box(380, 225, new_string.c_str(), Font::Calibri_24, Color::file_info, 1);
			//time
			long time = 0;
			read_this >> time;
			new_string = "Time: ";
			new_string += take_ms_return_english(time);
			Time = new Text_Box(380, 260, new_string.c_str(), Font::Calibri_24, Color::file_info, 1);
			bool has_star = false;
			read_this >> has_star;
			Texture* use_this = nullptr;
			if (has_star)
				use_this = Pic::star_hud_filled;
			else
				use_this = Pic::no_pic;
			star_pic_ = new Displayable_Object(380, 295, use_this);
			bool has_teleporter = false;
			read_this >> has_teleporter;
			if (has_teleporter)
				use_this = Pic::teleporter_hud_filled;
			else
				use_this = Pic::no_pic;
			teleporter_pic_ = new Displayable_Object(430, 295, use_this);
		}
		~File_Information()
		{
			//delete World;
			//World = nullptr;
			//delete Deaths;
			//Deaths = nullptr;
			delete Resets;
			Resets = nullptr;
			delete Difficulty;
			Difficulty = nullptr;
			delete Time;
			Time = nullptr;
			delete teleporter_pic_;
			teleporter_pic_ = nullptr;
			delete star_pic_;
			star_pic_ = nullptr;
		}
	};

	enum Title_Screen_State
	{
		Main,
		File_Select,
		Difficulies,
		Options,
		Credits
	};

	void handle_keyboard_input();
	void delete_this_file(int finger_position);
	inline void update_file_pic();
	inline void switch_to_this_state(Title_Screen_State this_one)
	{
		switch (this_one)
		{
		case Main:
			important_message_->change_the_text("Version 1.0");
			max_positions_ = 3;
			break;
		case File_Select:
			update_file_info();
			important_message_->change_the_text("Select a file with [Enter]. Delete a file with [Delete].");
			max_positions_ = 10;
			break;
		case Difficulies:
			important_message_->change_the_text("Select a difficulty setting! You better not choose easy!");
			max_positions_ = 3;
			break;
		case Credits:
			important_message_->change_the_text("This game was made by Alex Vranas in 2015, and updated in 2023 to be used as a portfolio project.");
			max_positions_ = 1;
			break;
		case Options:
			important_message_->change_the_text("If you want to play with a controller, use a separate program like JoyToKey :) Google it!");
			max_positions_ = 3;
			break;
		}
		finger_position_ = 0;
		the_screen_state_ = this_one;
	}

	void update_file_info();
	Title_Screen_State the_screen_state_;
	bool taking_keyboard_input_;
	Text_Box* new_file_name_text_;
	vector<Text_Box*> the_text_boxes_;
	vector<Displayable_Object*> pretty_things_;
	vector<Text_Box*> the_file_names_;
	Text_Box* important_message_;
	Text_Box* back_message_;
	File_Information* the_file_information_;
	const static string EMPTY_FILE_NAME;
	int finger_position_;
	int max_positions_;
	Finger the_finger_;
	int selected_file_;
	string new_file_name_;
	Cool_Background* cool_background_;
	bool delete_warning_;
	Option_Meter* blood_intensity_;
	Option_Meter* sound_volume_;
	Option_Meter* music_volume_;
};

#endif
