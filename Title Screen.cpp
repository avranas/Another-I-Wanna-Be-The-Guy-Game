#include "stdafx.h"
#include "Title Screen.h"
#include "Save Box.h"
#include "Music and Sound.h"

const string Title_Screen::EMPTY_FILE_NAME = "NewGame!";
const double Title_Pic::TITLE_SPEED = 100;
const double Title_Pic::FINAL_LOCATION_Y = 12;

void Title_Screen::update_file_pic()
{
	ifstream file_names("Save_Files/File Map.txt");
	string new_entry;
	string garbage_entry;
	the_file_names_.clear();
	SDL_Color use_this_color(0x00, 0x00, 0x00, 0x00);
	for (int x = 0; x != 10; x++)
	{
		file_names >> new_entry;
		file_names >> garbage_entry;
		if (new_entry == EMPTY_FILE_NAME)
			use_this_color = SDL_Color(249, 183, 0x00, 0x00);
		else
			use_this_color = SDL_Color(198, 72, 0x00, 0x00);
		the_file_names_.push_back(new Text_Box(90, 190 + x * 35, new_entry.c_str(), Font::Calibri_24, &use_this_color, 1));
	}
}

void Option_Meter::update_value_text_and_save()
{
	string new_value;
	new_value = to_string(position_);
	new_value += "%";
	value_->change_the_text(new_value.c_str());
}

Title_Screen::Title_Screen() :
	finger_position_(0),
	the_screen_state_(Main),
	max_positions_(3),
	selected_file_(0),
	taking_keyboard_input_(false),
	new_file_name_text_(nullptr),
	important_message_(new Text_Box(10, 152, "Important Message", Font::Calibri_24, Color::important_message, 1)),
	the_file_information_(nullptr),
	delete_warning_(false),
	blood_intensity_(new Option_Meter("Blood Intensity", 90, 190, 50)),
	sound_volume_(new Option_Meter("Sound Volume", 90, 225, 50)),
	music_volume_(new Option_Meter("Music Volume", 90, 260, 50)),
	back_message_(new Text_Box(10, 540, "[Esc] to go back", Font::Calibri_24, Color::white, 1))
{
	blood_intensity_->set_new_value(common_save_data->blood_intensity());
	sound_volume_->set_new_value(common_save_data->sound_volume());
	music_volume_->set_new_value(common_save_data->music_volume());
	cool_background_ = new Cool_Background(Pic::title_background);
	important_message_->change_the_text("Version 1.0");
	SDL_Color white_color = SDL_Color(0xFF, 0xFF, 0xFF, 0x00);
	pretty_things_.push_back(new Displayable_Object(642, 196, Pic::big_kid));
	pretty_things_.push_back(new Title_Pic(10, 10));
	the_text_boxes_.push_back(new Text_Box(10, 690, "[Esc] to go back", Font::Calibri_24, Color::white, 1));
	the_text_boxes_.push_back(new Text_Box(90, 190, "Game", Font::Calibri_24, Color::title_choices, 1));
	the_text_boxes_.push_back(new Text_Box(90, 225, "Options", Font::Calibri_24, Color::title_choices, 1));
	the_text_boxes_.push_back(new Text_Box(90, 260, "Credits", Font::Calibri_24, Color::title_choices, 1));
	the_text_boxes_.push_back(new Text_Box(90, 190, "Easy -- Play this one if you're a wuss.", Font::Calibri_24, Color::title_choices, 1));
	the_text_boxes_.push_back(new Text_Box(90, 225, "Normal -- Play this one.", Font::Calibri_24, Color::title_choices, 1));
	the_text_boxes_.push_back(new Text_Box(90, 260, "Stupid Hard -- This is pretty much impossible.", Font::Calibri_24, Color::title_choices, 1));
	update_file_pic();
	if (Game::background_music->current_track() == Music::title_part_one)
		Game::background_music->go_to_next_track();
}

Title_Screen::~Title_Screen()
{
	delete blood_intensity_;
	blood_intensity_ = nullptr;
	delete music_volume_;
	music_volume_ = nullptr;
	delete sound_volume_;
	sound_volume_ = nullptr;
	delete the_file_information_;
	the_file_information_ = nullptr;
	delete important_message_;
	important_message_ = nullptr;
	for (int x = 0; x != pretty_things_.size(); ++x)
	{
		delete pretty_things_[x];
		pretty_things_[x] = nullptr;
	}
	for (int x = 0; x != the_text_boxes_.size(); ++x)
	{
		delete the_text_boxes_[x];
		the_text_boxes_[x] = nullptr;
	}
	for (int x = 0; x != the_file_names_.size(); ++x)
	{
		delete the_file_names_[x];
		the_file_names_[x] = nullptr;
	}
	delete cool_background_;
	cool_background_ = nullptr;
}


string get_file_name(int finger_position)
{
	string file_name;
	ifstream read_file_map("Save_Files/File Map.txt");
	for (int x = 0; x <= finger_position; ++x)
	{
		read_file_map >> file_name;
		read_file_map >> file_name;
	}
	return file_name;
}

//0 for easy
//1 for normal
//2 for hard
string create_a_new_save_file(string player_name, int new_difficulty, int file_position)
{
	ifstream default_save_data("Save_Files/Default Save Data.txt");
	string file_path = "Save_Files/Save_File_";
	file_path += to_string(file_position);
	file_path += ".txt";
	ofstream new_file(file_path);
	new_file << player_name;
	new_file << "\n";
	new_file << new_difficulty;
	new_file << "\n";
	while (!default_save_data.eof())
	{
		string new_entry_one;
		default_save_data >> new_entry_one;
		new_file << new_entry_one;
		new_file << "\n";
	}
	//This is seperate because the default_save_data doesn't know what kind\
	of difficulty you want.
	ifstream file_mapper("Save_Files/File Map.txt");
	vector<string> file_mapper_copy;
	for (int x = 0; x != 10; ++x)
	{
		string new_entry_one;
		string new_entry_two;
		if (x == file_position)
		{
			string garbage;
			file_mapper >> garbage;
			file_mapper >> garbage;
			file_mapper_copy.push_back(player_name);
			file_mapper_copy.push_back(file_path);
		}
		else
		{
			file_mapper >> new_entry_one;
			file_mapper >> new_entry_two;
			file_mapper_copy.push_back(new_entry_one);
			file_mapper_copy.push_back(new_entry_two);
		}
	}
	file_mapper.clear();
	ofstream new_file_mapper("Save_Files/File Map.txt");
	for (int x = 0; x != 10; ++x)
	{
		new_file_mapper << file_mapper_copy[x * 2];
		new_file_mapper << " ";
		new_file_mapper << file_mapper_copy[x * 2 + 1];
		new_file_mapper << "\n";
	}
	return file_path;
}

void Title_Screen::delete_this_file(int finger_position)
{
	//Erase the data from the hard drive
	string file_name = "Save_Files/Save_File_";
	file_name += to_string(finger_position);
	file_name += ".txt";
	ifstream delete_this_file(file_name);
	delete_this_file.clear();
	//Change the file map
	ifstream file_map_in("Save_Files/File Map.txt");
	vector<string> file_mapper_copy;
	string new_entry_one;
	string new_entry_two;
	for (int x = 0; x != 10; ++x)
	{
		file_map_in >> new_entry_one;
		file_mapper_copy.push_back(new_entry_one);
		file_map_in >> new_entry_two;
		file_mapper_copy.push_back(new_entry_two);
	}
	ofstream file_map_out("Save_Files/File Map.txt");
	file_map_out.clear();
	for (int x = 0; x != 10; ++x)
	{
		if (x == finger_position)
		{
			file_map_out << EMPTY_FILE_NAME;
			file_map_out << " ";
			file_map_out << EMPTY_FILE_NAME;
		}
		else
		{
			file_map_out << file_mapper_copy[x * 2];
			file_map_out << " ";
			file_map_out << file_mapper_copy[x * 2 + 1];
		}
		file_map_out << "\n";
	}
}

void Title_Screen::update_file_info()
{
	delete the_file_information_;
	the_file_information_ = nullptr;
	string file_name = get_file_name(finger_position_);
	if (file_name == EMPTY_FILE_NAME)
		return;
	the_file_information_ = new File_Information(file_name);
}


void Title_Screen::handle_events()
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
			switch (g_event.key.keysym.sym)
			{
			case SDLK_DELETE:
				if (the_screen_state_ == File_Select)
				{
					important_message_->change_the_text("Are you sure you want to delete? Press the 'D' key to confirm, or any other key to cancel.");
					delete_warning_ = true;
					return;
					//Do this to skip delete_warning_ reset.
				}
				break;
			case SDLK_d:
				if (delete_warning_)
				{
					//Do actual deleting
					delete_this_file(finger_position_);
					update_file_info();
					update_file_pic();
				}
				break;
			case SDLK_DOWN:
				finger_position_++;
				if (max_positions_ != 1)
				{
					Mix_PlayChannel(-1, Sound::menu, 0);
				}
				if (finger_position_ == max_positions_)
				{
					finger_position_ = 0;
				}
				if (the_screen_state_ == File_Select)
				{
					update_file_info();
				}
				break;
			case SDLK_UP:
				finger_position_--;
				if (max_positions_ != 1)
				{
					Mix_PlayChannel(-1, Sound::menu, 0);
				}
				if (finger_position_ == -1)
				{
					finger_position_ = max_positions_ - 1;
				}
				if (the_screen_state_ == File_Select)
				{
					update_file_info();
				}
				break;
			case SDLK_RETURN:
				if (delete_warning_)
				{
					break;
				}
				Mix_PlayChannel(-1, Sound::menu, 0);
				switch (the_screen_state_)
				{
				case Main:
					switch (finger_position_)
					{
					case 0:
						switch_to_this_state(File_Select);
						break;
					case 1:
						switch_to_this_state(Options);
						break;
					case 2:
						switch_to_this_state(Credits);
						break;
					}
					break;
				case File_Select:
				{
					if (the_file_names_[finger_position_]->the_text() == EMPTY_FILE_NAME)
					{
						selected_file_ = finger_position_;
						switch_to_this_state(Difficulies);
						finger_position_ = 1;
					}
					else
					{
						Mix_HaltChannel(-1);
						string selected_file_name = get_file_name(finger_position_);
						delete save_file;
						save_file = nullptr;
						save_file = new Save_File(selected_file_name);
						g_next_game_state = StateMainGame;
					}
					break;
				}
				case Difficulies:
					g_next_game_state = StateMainGame;
					delete save_file;
					save_file = nullptr;
					new_file_name_ = "Slot-";
					new_file_name_ += std::to_string(selected_file_);
					switch (finger_position_)
					{
					case 0:
						save_file = new Save_File(create_a_new_save_file(new_file_name_, finger_position_, selected_file_));
						break;
					case 1:
						save_file = new Save_File(create_a_new_save_file(new_file_name_, finger_position_, selected_file_));
						break;
					case 2:
						save_file = new Save_File(create_a_new_save_file(new_file_name_, finger_position_, selected_file_));
						break;
					}
					update_file_pic();
					break;
				case Options:
					common_save_data->update_and_save(blood_intensity_->value(),
						sound_volume_->value(),
						music_volume_->value()
					);
				case Credits:
					switch_to_this_state(Main);
					break;
				}
				break;
			case SDLK_ESCAPE:
				if (the_screen_state_ != Main)
					Mix_PlayChannel(-1, Sound::menu, 0);
				switch (the_screen_state_)
				{
				case Main:
					break;
				case Difficulies:
					switch_to_this_state(File_Select);
					new_file_name_ = '_';
					update_file_pic();
					break;
				case Options:
					common_save_data->update_and_save(blood_intensity_->value(),
						sound_volume_->value(),
						music_volume_->value()
					);
				case Credits:
				case File_Select:
					the_file_information_ = nullptr;
					delete the_file_information_;
					switch_to_this_state(Main);
					break;
				}
				break;
			case SDLK_LEFT:
				if (the_screen_state_ == Options)
				{
					switch (finger_position_)
					{
					case 0:
						blood_intensity_->decrement();
						break;
					case 1:
						sound_volume_->decrement();
						Mix_Volume(-1, sound_volume_->value());
						Mix_PlayChannel(-1, Sound::menu, 0);
						break;
					case 2:
						music_volume_->decrement();
						Mix_VolumeMusic(music_volume_->value());
						break;
					}
				}
				break;
			case SDLK_RIGHT:
				if (the_screen_state_ == Options)
				{
					switch (finger_position_)
					{
					case 0:
						blood_intensity_->increment();
						break;
					case 1:
						sound_volume_->increment();
						Mix_Volume(-1, sound_volume_->value());
						Mix_PlayChannel(-1, Sound::menu, 0);
						break;
					case 2:
						music_volume_->increment();
						Mix_VolumeMusic(music_volume_->value());
						break;
					}
				}
				break;
				break;
			}
			delete_warning_ = false;
			if (the_screen_state_ == File_Select)
				important_message_->change_the_text("Select a file with [Enter]! Delete a file with [Delete].");
		}
	}
}

const Point Option_Meter::ball_offset_ = Point(-5, -5);

void Title_Screen::logic()
{
	cool_background_->move();
}

void Title_Screen::render()
{
	SDL_RenderClear(g_renderer);
	cool_background_->render();
	for (auto iter = pretty_things_.begin(); iter != pretty_things_.end(); ++iter)
		(*iter)->simple_render();
	for (auto iter = the_text_boxes_.begin(); iter != the_text_boxes_.begin() + 1; ++iter)
		(*iter)->simple_render();
	switch (the_screen_state_)
	{
	case Main:
		for (auto iter = the_text_boxes_.begin() + 1; iter != the_text_boxes_.begin() + 4; ++iter)
			(*iter)->simple_render();
		break;
	case File_Select:
		for (auto iter = the_file_names_.begin(); iter != the_file_names_.end(); ++iter)
			(*iter)->simple_render();
		break;
	case Difficulies:
		for (auto iter = the_text_boxes_.begin() + 4; iter != the_text_boxes_.begin() + 7; ++iter)
			(*iter)->simple_render();
		break;
	case Options:
		blood_intensity_->render();
		sound_volume_->render();
		music_volume_->render();
		break;
	case Credits:
		break;
	}
	if (the_file_information_ != nullptr)
		the_file_information_->render();
	the_finger_.put_in_position(finger_position_);
	if (the_screen_state_ != Credits)
		the_finger_.simple_render();
	important_message_->simple_render();
	if (the_screen_state_ != Main)
		back_message_->simple_render();
	SDL_RenderPresent(g_renderer);
}